#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <functional>
#include <future>
#include <utility>
#include <cctype>
#include <gsl/gsl>

#include "clara/clara.hpp"
#include "sort.hpp"

using clara::Opt; using clara::Arg; using clara::Help;

struct commandline_args
{
    bool ignore_case{false};
    bool reverse_order{false};
    bool ignore_leading_blanks{false};
    bool dictionary_order{false};
    bool random_sort{false};
    bool check{false};
    std::vector<std::string> infiles{};
    std::string outfile{};
    bool help_flag{false};
};


using Lines = std::list<std::string>;

auto operator<<( std::ostream& os, const Lines& lines ) -> std::ostream&;

template<typename Predicate>
auto process_file( const std::string& fname, Predicate predicate ) -> Lines;

template<typename Container, typename Predicate>
auto process_files( const Container& files, Predicate predicate ) -> Lines;

auto get_predicate( const commandline_args& args )
-> std::function<bool(const std::string&, const std::string&)>;

auto write_lines( const std::string& fname, const Lines& container ) -> void;

int main( int argc, char* argv[] )
try{
    auto cli_args = commandline_args{};
    auto cli
        = Opt( cli_args.ignore_case )
             ["-f"]["--ignore-case"]("Treat all lowercase as uppercase")
        | Opt( cli_args.reverse_order )
             ["-r"]["--reverse"]("Reverse the comparison")
        | Opt( cli_args.ignore_leading_blanks )
             ["-b"]["--ignore-leading-blanks"]("Ignore leading whitespace")
        | Opt( cli_args.outfile, "Output file")
             ["-o"]["--output-file"]
        | Arg( cli_args.infiles, "[FILE]..." )
        | Help( cli_args.help_flag );

    auto cli_result = cli.parse( clara::Args(argc, argv) );
    if( !cli_result || cli_args.help_flag ){
        std::cout << cli << std::endl;
        return 1;
    }

    auto predicate = get_predicate(cli_args);
    if( !cli_args.infiles.empty() ){
        auto lines = process_files(cli_args.infiles, predicate );
        write_lines(cli_args.outfile, lines);
    }
    else{
        auto lines = jam::get_lines<Lines>(std::cin);
        jam::sort_lines(lines, predicate);
        write_lines(cli_args.outfile, lines);
    }
}
catch( const std::exception& ex ){
    std::cerr << ex.what() << std::endl;
}

auto operator<<( std::ostream& os, const Lines& lines ) -> std::ostream&
{
    for( const auto& line : lines )
        os << line << "\n";
    return os;
}

template<typename Predicate>
auto process_file( const std::string& fname, Predicate predicate ) -> Lines
{
    auto ifs = std::ifstream{fname};
    auto lines = jam::get_lines<Lines>(ifs);
    jam::sort_lines(lines, predicate);
    return lines;
}

template<typename Container, typename Predicate>
auto process_files( const Container& files, Predicate predicate ) -> Lines
{
    auto future_results = std::vector<std::future<Lines>>{};
    for( const auto& file : files ){
        future_results.push_back( std::async(
            process_file<Predicate>, file, predicate
        ));
    }
    auto results = Lines{};
    for( auto&& fr : future_results ){
        auto result = std::move(fr.get());
        results.splice( std::upper_bound(results.cbegin(), results.cend(), result.front()),
                        std::move(result) );
    }
    return results;
}


inline auto to_upper( const std::string& s ) -> std::string
{
    auto result = std::string(s.size(),'\0');
    std::transform(s.cbegin(), s.cend(), result.begin(),
                [](unsigned char ch){ return std::toupper(ch); } );
    return result;
}

inline auto ignore_leading_blanks( const std::string& s ) -> std::string
{
    const auto sbegin = s.find_first_not_of(" \t\n");
    if( sbegin == std::string::npos )
        return "";
    return s.substr(sbegin);
}


auto get_predicate( const commandline_args& args )
-> std::function<bool(const std::string&, const std::string&)>
{
    auto predicate = std::function<bool(const std::string&, const std::string&)>{};
    if( args.reverse_order ){
        if( args.ignore_case && args.ignore_leading_blanks )
            predicate =  jam::wrap_binary_predicate(
                std::greater<std::string>(), to_upper, ignore_leading_blanks
            );
        else if( args.ignore_case )
            predicate =  jam::wrap_binary_predicate( std::greater<std::string>(), to_upper);
        else if( args.ignore_leading_blanks )
            predicate =  jam::wrap_binary_predicate( std::greater<std::string>(),
                        ignore_leading_blanks );
        else
            predicate =  std::greater<std::string>();
    }
    else{
        if( args.ignore_case && args.ignore_leading_blanks )
            predicate =  jam::wrap_binary_predicate(
                std::less<std::string>(), to_upper, ignore_leading_blanks
            );
        else if( args.ignore_case )
            predicate =  jam::wrap_binary_predicate( std::less<std::string>(), to_upper);
        else if( args.ignore_leading_blanks )
            predicate =  jam::wrap_binary_predicate( std::less<std::string>(),
                        ignore_leading_blanks );
        else
            predicate =  std::less<std::string>();
    }
    return predicate;
}

void write_lines( const std::string& fname, const Lines& lines )
{
    auto ofs = std::ofstream{fname};
    std::ostream& os = ofs ? ofs : std::cout;
    for( const auto& line : lines ){
        os << line << "\n";
    }
}
