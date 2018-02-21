#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <utility>
#include <thread>
#include <future>
#include <stdexcept>
#include "clara/clara.hpp"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::regex;
using std::smatch;
using std::ifstream;
using clara::Arg;
using clara::Help;

namespace
{
bool g_help_flag;
string g_pattern;
vector<string> g_file_names;
} // namespace


using LineMatch = std::pair<size_t,string>;

namespace std
{
    std::ostream& operator<<( std::ostream& os, const LineMatch& lm );
}

class FileMatches
{
public:
    explicit FileMatches( string file_name )
        : fname(file_name) { }
    
    void push_back( LineMatch match )
        { matches.push_back(std::move(match)); }

    auto empty() const noexcept -> bool
    { return matches.empty(); }
    
    friend std::ostream& operator<<( std::ostream& os, const FileMatches& fm );
private:
    string fname;
    vector<LineMatch> matches;
};


auto grep_file( const string& fname, const regex& re ) -> FileMatches;
auto grep_files( const vector<string>, const regex& re) -> vector<FileMatches>;


int main( int argc, char* argv[] )
try{
    auto clip
        = Arg( g_pattern, "The pattern to look for" ).required()
        | Arg( g_file_names, "File(s) to search through" )
        | Help( g_help_flag );
    auto clip_result = clip.parse( clara::Args(argc, argv) );
    if( !clip_result || g_help_flag ){
        cout << clip << endl;
        return !clip_result;
    }
    
    auto re = regex( g_pattern );
    auto results = grep_files( g_file_names, re );
    for( const auto& file_match : results )
        cout << file_match;

    return 0;
}
catch( const std::regex_error& e )
{
    std::cerr << "    Invalid regular expression: " << e.what() << std::endl;
    return 2;
}
catch( const std::exception& e )
{
    std::cerr << e.what() << endl;
    return 3;
}


auto grep_file( const string& fname, const regex& re ) -> FileMatches
{
    cout << "grep_file on thread[" << std::this_thread::get_id()
        << "]" << endl;
    auto ifs = ifstream{fname};
    if( !ifs )
        throw std::runtime_error( "Unable to open the file " + fname );
    
    auto result = FileMatches{fname};
    auto n = size_t{1};
    for( string line; getline(ifs, line); ++n){
        if( std::regex_search(line,  re) ){
            result.push_back( {n, std::move(line)} );
        }
    }
    return result;
}

auto grep_files( const vector<string> files, const regex& re) -> vector<FileMatches>
{
    auto intermediate_result = vector<std::future<FileMatches>>();
    for( const string& file : files ){
        intermediate_result.push_back( std::async( grep_file, file, re) );
    }
    auto result = vector<FileMatches>();
    for( auto&& f : intermediate_result ){
        try{
            result.push_back( std::move( f.get() ) );
        }
        catch( const std::runtime_error& e ){
            std::cerr << e.what() << endl;
        }
    }
    return result;
}

std::ostream& operator<<( std::ostream& os, const FileMatches& fm )
{
    os << fm.fname << endl;
    for( const auto& lm : fm.matches )
        os << lm << endl;
    return os;
}

namespace std
{
    std::ostream& operator<<( std::ostream& os, const LineMatch& lm )
    {
        return os << lm.first << ":    " << lm.second;
    }
}
