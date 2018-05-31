#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <utility>

#include <gsl/gsl>
#include <boost/process.hpp>

#include "clara/clara.hpp"

using clara::Opt; using clara::Arg; using clara::Help;
using string_span_t = std::string_view;

namespace bp = boost::process;

using namespace std::string_literals;

struct commandline_args {
    std::string args_file{};
    bool verbose{false};
    bool help_flag{false};
    std::vector<std::string> command{};
};

class Subprocess
{
public:
    Subprocess(const std::string& cmd, const std::string& args)
        : child_{bp::search_path(cmd), args} {}
    ~Subprocess() { child_.wait(); }
    Subprocess(Subprocess&&) = default;
    Subprocess& operator=(Subprocess&&) = default;
private:
    bp::child child_;
};

class Xargs_base
{
public:
    explicit Xargs_base(std::string command)
        {
            std::istringstream iss{std::move(command)};
            iss >> cmd_;
            init_args_ = std::move(iss.str());
        }

    Xargs_base(std::string command, std::string args)
        : cmd_{std::move(command)}, init_args_{std::move(args)} { }
    Xargs_base(Xargs_base&&) noexcept = default;
    Xargs_base& operator=(Xargs_base&) noexcept = default;
    const std::string& command() const { return cmd_; }
    const std::string& args() const { return init_args_; }
private:
    std::string cmd_{};
    std::string init_args_{};
};

class Xargs : public Xargs_base
{
public:
    using Xargs_base::Xargs_base;
    Xargs(std::string command, std::istream& is, bool verbose)
        : Xargs_base{std::move(command)}, source_{is}, verbose_{verbose}
        {
        }
    
    Xargs(std::string command, std::string args, std::istream& src, bool verbose)
        : Xargs_base{std::move(command), std::move(args)}
        , source_{src}
        , verbose_{verbose}
        { }

    ~Xargs()
    {
        for( auto& p : sub_ ) p.wait();
    }

    void operator()()
    {
        for(std::string line; getline(source_, line);/**/) {
            try{
                const auto a = args() + line;
                if(verbose_){
                    std::cerr << "--- " << command() << " " << a << std::endl;
                }
                sub_.emplace_back(bp::search_path(command()), a);
            }catch (const std::system_error& err){
                if(0 < err.code().value()) throw;
            }
        }
    }

private:
    std::istream& source_{std::cin};
    bool verbose_{false};
    std::vector<bp::child> sub_;
};



int main(int argc, char* argv[])
{
    auto cli_args = commandline_args{};
    auto cli
        = Opt( cli_args.args_file, "Args input file" )
            ["-a"]["--arg-file"]("Read arguments from a file")
        | Opt( cli_args.verbose )
            ["-t"]["--verbose"]("Print command on stderr before executing it")
        | Arg( cli_args.command, "[command [initial-arguments]")
        | Help( cli_args.help_flag );

    auto cli_result = cli.parse( clara::Args(argc, argv) );
    if( !cli_result || cli_args.help_flag ){
        std::cerr << cli << std::endl;
        return 1;
    }

    const auto cmd = cli_args.command.empty() ? "echo"s : cli_args.command.front();
    const auto args = [&cmd=cli_args.command](){
        if(!cmd.empty()){
            std::ostringstream oss;
            for(auto it = ++cmd.cbegin(); it != cmd.cend(); ++it)
                oss << *it << " ";
            return oss.str();
        }
        return ""s;
    }();
    auto infile = std::ifstream{cli_args.args_file};
    auto& input = infile.is_open() ? infile : std::cin;

    auto xargs = Xargs(cmd, args, input, cli_args.verbose);
    xargs();
}
