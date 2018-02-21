#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <future>
#include <thread>
#include <boost/filesystem.hpp>
#include <cstdio>   // for std::tmpnam
#include "clara/clara.hpp"


using std::vector;
using std::string;
using std::cout;
using std::endl;
using clara::Opt;
using clara::Arg;
using clara::Help;
namespace fs = boost::filesystem;


struct cli_arguments
{
string pattern;
string replacement;
vector<string> filenames;
std::pair<bool,string> inplace;
bool help_flag{false};

static const std::regex e_syntax;
};
const std::regex cli_arguments::e_syntax(R"(s/(.*)/(.*)/)");


struct bad_expression_exception : public std::runtime_error
{
    bad_expression_exception( const string& s ) : std::runtime_error(s) { };
};

struct bad_filename_exception : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};


class fileupdate_base
{
public:
    virtual ~fileupdate_base() noexcept = default;
protected:
    explicit fileupdate_base( const std::string& fname )
        : filename{fname}
        {
            if( !fs::is_regular_file(filename) )
                throw bad_filename_exception( fname + " is not a regular file" );
        }
    virtual void do_update( const std::regex& pattern,
                            const std::string& replacement ) = 0;
    static string make_temp( const string& fname ) 
    {
        return fname + std::tmpnam(nullptr);
    }

    fs::path filename;
};

class fileupdate_inplace : public fileupdate_base
{
public:
    explicit fileupdate_inplace( const std::string& fname )
        : fileupdate_base{fname}
        {
            
        }
    void operator()( const std::regex& pattern, const std::string& replacement );
    ~fileupdate_inplace() noexcept
    { try{
        tidy();
      }catch( const std::exception& e){
          std::cerr << "temp file remove failed" << endl;
      }
    }
protected:
    void do_update( const std::regex& pattern,
                    const std::string& replacement ) override;
    void tidy();
    
private:
    fs::path temp_filename{make_temp(filename.string())};
    bool update_done{false};
};

class fileupdate_backup : public fileupdate_base
{
public:
    fileupdate_backup( const std::string& fname, const std::string& sfx )
        : fileupdate_base{fname}
        , backup_filename{fname+"."+sfx}
        {
            if( fs::exists(backup_filename) )
                throw bad_filename_exception( "Invalid SUFFIX. File "
                        + backup_filename.string() + " already exists." );
        }
    void operator()( const std::regex& pattern, const std::string& replacement );
protected:
    void do_update( const std::regex& pattern,
                    const std::string& replacement ) override;
private:
    fs::path temp_filename{make_temp(filename.string())};
    fs::path backup_filename;
};

auto replace_pattern( std::istream&, std::ostream&, const std::regex& re ) -> void;
template<typename Container>
void replace_in_files( const Container& files, std::ostream&,
            const std::regex& pattern, const std::string& replacement );

template<typename Container, typename Functor>
void replace_pattern( const Container& files, const std::regex& pattern,
                      const std::string& replacement );

int main( int argc, char* argv[] )
try{
    auto cli_args = cli_arguments{};
    auto cli
        = Opt( [&cli_args]( const string& s)
               {
                   auto re = std::regex(cli_args.e_syntax);
                   auto match =  std::smatch{};
                   if( std::regex_search(s.cbegin(), s.cend(), match, re) ){
                       cli_args.pattern = match.str(1);
                       cli_args.replacement = match.str(2);
                   }
                   else
                        throw bad_expression_exception( "Invalid expression syntax: " + s );
               },
               "s/pattern/replacement/")
               ["-e"]["--expression"]
               .required()
        | Opt( [&cli_args]( const string& s )
               {
                   cli_args.inplace.first = true;
                   auto re = std::regex(R"(\.?(\w))");
                   auto match = std::smatch{};
                   if( !s.empty() ){
                    if( std::regex_search(s.cbegin(), s.cend(), match, re) ){
                        cli_args.inplace.second = match.str(1);
                    }else
                        throw std::runtime_error( "Invalid suffix: " + s );
                   }
               }
               , "[SUFFIX]")
               ["-i"]["--inplace"]
        | Arg( cli_args.filenames, "FILES..." )
        | Help( cli_args.help_flag );
    auto cli_result = cli.parse( clara::Args(argc, argv) );
    if( !cli_result || cli_args.help_flag ){
        cout << cli << endl;
        return 1;
    }
    auto pattern = std::regex( cli_args.pattern );
    if( cli_args.inplace.first && !cli_args.inplace.second.empty() ){
        std::vector<std::future<void>> futures;
        for( const auto& file : cli_args.filenames ){
            futures.push_back(std::async(
                    fileupdate_backup(file, cli_args.inplace.second),
                    pattern, cli_args.replacement
                    ));
            // auto updater = fileupdate_backup( file, cli_args.inplace.second );
            // updater(pattern, cli_args.replacement);
        }
        for( auto& f : futures )
            f.get();
    }
    else if( cli_args.inplace.first ){
        std::vector<std::future<void>> futures;
        for( const auto& file : cli_args.filenames ){
            futures.push_back( std::async(
                fileupdate_inplace(file),
                pattern, cli_args.replacement
            ));
            // auto updater = fileupdate_inplace( file );
            // updater(pattern, cli_args.replacement);
        }
        for( auto& f : futures )
            f.get();
    }
    else{
        replace_in_files( cli_args.filenames, cout, pattern, cli_args.replacement );
    }
}
catch( const std::exception& e )
{
    std::cerr << e.what() << endl;
    return 1;
}
catch( ... ){
    std::cerr << "Unknown exception" << endl;
    return 2;
}



auto replace_pattern( std::istream& is, std::ostream& os,
                      const std::regex& pattern, const std::string& replacement )-> void
{
    for( string line; getline(is, line); ){
        std::regex_replace(std::ostreambuf_iterator<char>(os),
                        line.cbegin(), line.cend(),
                        pattern, replacement
                        );
        // for( string word; iss >> word; ){
        //     os << std::regex_replace( word, pattern, replacement )
        //     << endl;
        // }
        os << endl;
    }
}


template<typename Container>
void replace_in_files( const Container& files, std::ostream& os, 
                    const std::regex& pattern, const std::string& replacement )
{
    for( const auto& file : files ){
        auto ifs = std::ifstream{file};
        if( !ifs ) throw std::runtime_error( "Failed to open the file" + file );
        replace_pattern( ifs, os, pattern, replacement );
    }
}


void
fileupdate_inplace::do_update( const std::regex& pattern,
                               const std::string& replacement )
{
    fs::ifstream ifs{filename};
    fs::ofstream ofs{temp_filename};
    if( !ifs )
        throw bad_filename_exception( "Failed to open the file " + filename.string() );
    if( !ofs )
        throw bad_filename_exception( "Failed to create a temporary file" );
    // for( string line; getline( ifs, line); ){
        // ofs << std::regex_replace( line, pattern, replacement )
        //     << endl;
        replace_pattern(ifs, ofs, pattern, replacement);
    // }
    ifs.close();
    ofs.close();
    auto temp = make_temp(filename.string());
    fs::rename(filename, temp );
    fs::rename(temp_filename, filename);
    fs::remove(temp);
    update_done = true;
}

void
fileupdate_inplace::operator()( const std::regex& pattern,
                                const std::string& replacement )
{
        std::cout << "thread[" << std::this_thread::get_id() << "] do_update"
        << endl;
    do_update( pattern, replacement );
}

void fileupdate_inplace::tidy()
{
    if( !update_done ){
        fs::remove(temp_filename);
    }
}


void
fileupdate_backup::do_update( const std::regex& pattern,
                              const std::string& replacement )
{
    fs::ifstream ifs{filename};
    fs::ofstream ofs{temp_filename};
    if( !ifs )
        throw bad_filename_exception( "Failed to open the file " + filename.string() );
    if( !ofs )
        throw bad_filename_exception( "Failed to create a temporary file" );
    // for( string line; getline( ifs, line); ){
    //     ofs << std::regex_replace( line, pattern, replacement )
    //         << endl;
    // }
    replace_pattern( ifs, ofs, pattern, replacement );
    ifs.close();
    ofs.close();
    fs::rename(filename, backup_filename);
    fs::rename(temp_filename, filename);
}

void
fileupdate_backup::operator()( const std::regex& pattern,
                               const std::string& replacement )
{
    std::cout << "thread[" << std::this_thread::get_id() << "] do_update"
        << endl;
    do_update(pattern, replacement);
}
