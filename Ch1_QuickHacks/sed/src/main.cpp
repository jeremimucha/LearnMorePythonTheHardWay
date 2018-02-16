#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <iterator>
#include <stdexcept>
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
        : fileupdate_base{fname}, ifs{fname}, ofs{ make_temp(fname) }
        {
            if( !ifs )
                throw bad_filename_exception( "Failed to open the file " + fname );
            if( !ofs )
                throw bad_filename_exception( "Failed to create a temporary file" );
        }
    void operator()( const std::regex& pattern, const std::string& replacement );
protected:
    void do_update( const std::regex& pattern,
                    const std::string& replacement ) override;
private:
    fs::ifstream ifs;
    fs::ofstream ofs;
};

class fileupdate_backup : public fileupdate_base
{
public:
    fileupdate_backup( const std::string& fname, const std::string& sfx )
        : fileupdate_base{fname}
        , ifs{filename}
        , ofs{make_temp(fname)}
        , backup_filename{fname+"."+sfx}
        {
            if( !ifs )
                throw bad_filename_exception( "Failed to open the file " + fname );
            if( !ofs )
                throw bad_filename_exception( "Failed to create a temporary file" );
            if( fs::exists(backup_filename) )
                throw bad_filename_exception( "Invalid SUFFIX. File "
                        + backup_filename.string() + " already exists." );
        }
private:
    fs::ifstream ifs;
    fs::ofstream ofs;
    fs::path backup_filename;
};

auto replace_pattern( std::istream&, std::ostream&, const std::regex& re ) -> void;
template<typename Container>
void replace_in_files( const Container& files, const std::regex& pattern,
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
        | Arg( cli_args.filenames, "FILES..." )
        | Help( cli_args.help_flag );
    auto cli_result = cli.parse( clara::Args(argc, argv) );
    if( !cli_result || cli_args.help_flag ){
        cout << cli << endl;
        return 1;
    }
    auto pattern = std::regex( cli_args.pattern );
    replace_in_files( cli_args.filenames, cout, pattern, cli_args.replacement );
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
        os << std::regex_replace( line, pattern, replacement )
         << endl;
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

