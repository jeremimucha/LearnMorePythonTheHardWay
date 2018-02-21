#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <stdexcept>
#include <clara/clara.hpp>

using clara::Opt;
using clara::Arg;
using clara::Help;
using std::vector;
using std::string;
using std::cout; using std::endl;
using std::istringstream;

namespace
{

bool gHelpFlag{false};
bool gNumberLines{false};
bool gNumberNonBlank{false};
vector<string> gFiles;

} // namespace


class FileException : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};


void write( const string&, size_t& );
void concat( const vector<string>& files );


int main( int argc, char* argv[] )
try{
    auto cliparser
            = Help( gHelpFlag )
            | Opt( gNumberLines )
                 ["-n"]["--number"]
                 ("Number all lines.")
            | Opt( [](bool flag)
                   {
                       if( gNumberLines ) gNumberLines = false;
                       gNumberNonBlank = true;
                   } )
                 ["-b"]["--number-nonblank"]
                 ("Number non-blank lines. Overrides --number")
            | Arg( gFiles, "Files to concatenate" );
    auto result = cliparser.parse( clara::Args(argc, argv) );
    if( !result || gHelpFlag ){
        std::cout << cliparser;
        return 0;
    }

    concat( gFiles );
}
catch( const FileException& e ){
    std::cerr << e.what() << std::endl;
}


void write( const string& file, size_t& line_number )
{
    bool line_begin{true};
    std::ifstream ifs{file};
    if( !ifs ){
        throw FileException( "Failed to open the file " + file );
    }

    for( char ch; ifs.get(ch); ){
        if( gNumberLines && line_begin ){
            cout << line_number << ".\t";
            ++line_number;
            line_begin = false;
        }
        if( gNumberNonBlank && line_begin && ch != '\n' ){
            cout << line_number << ".\t";
            ++line_number;
            line_begin = false;
        }

        cout << ch;
        
        if( ch == '\n' ){
            
            line_begin = true;
        }
    }
}

void concat( const vector<string>& files )
{
    size_t line_number{1};
    for( const auto& file : files ){
        write( file, line_number );
    }
}
