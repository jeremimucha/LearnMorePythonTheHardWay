#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <regex>
#include <algorithm>
#include <iterator>
#include <utility>
#include <stdexcept>
#include <future>
#include <thread>
#include "clara/clara.hpp"

using std::cout;
using std::wcout; using std::endl;
using std::string; using std::vector; using std::list; using std::pair;
using std::wstring;
using clara::Opt; using clara::Arg; using clara::Help;


namespace
{

string g_delimiter("\t");
string g_fields_temp;
pair<int,int> g_fields;
vector<string> g_files;
bool g_help_flag{false};

} // namespace


auto parse_fields( const std::string& ) -> pair<int,int>;

using FieldRange = pair<int,int>;
using LineFields = std::vector<std::wstring>;
using ListLineFields = std::list<LineFields>;


auto process_line( const std::wstring&, const std::wregex&, FieldRange )
     -> LineFields;
auto process_file( const std::string&, const std::wregex&, FieldRange )
     -> ListLineFields;
auto process_files( const vector<string>&, const std::wregex&, FieldRange )
     -> ListLineFields;

auto operator<<( std::wostream& wos, const LineFields& lf ) -> std::wostream&;
auto operator<<( std::wostream& wos, const ListLineFields& llf ) -> std::wostream&;


int main( int argc, char* argv[] )
try{
    auto cli
        = Opt( g_delimiter, "DELIM" )["-d"]["--delimiter"]
             ("Use DELIM instead of the tab character as the field delimiter")
             .required()
        | Opt( g_fields_temp, "N-M")["-f"]["--fields"]
             ("Select only the fields in range N-M, begining with 1.")
             .required()
        | Arg( g_files, "Files to process" ).required()
        | Help( g_help_flag );
    auto cli_result = cli.parse( clara::Args(argc, argv) );
    if( !cli_result || g_help_flag ){
        std::cerr << cli << endl;
        return 1;
    }
    g_fields = parse_fields(g_fields_temp);

    std::wstring wdelim( g_delimiter.cbegin(), g_delimiter.cend() );
    std::wregex delim_re( wdelim );
    auto res = process_files( g_files, delim_re, g_fields );
    wcout << res;
}
catch( const std::exception& ex ){
    std::cerr << ex.what() << endl;
}

auto parse_fields( const std::string& raw_fields ) -> pair<int,int>
{
    int n{0}, m{0};
    char ch;
    std::istringstream iss(raw_fields);
    iss >> n >> ch >> m;
    return std::make_pair(n,m);
}


auto process_line( const std::wstring& line, const std::wregex& re, FieldRange fr ) -> LineFields
{
    LineFields result;
    std::wsregex_token_iterator p(line.cbegin(), line.cend(), re, -1);
    std::wsregex_token_iterator end;
    std::copy( p, end, std::back_inserter(result) );
    auto b = result.size() > fr.first -1 ? result.begin()+(fr.first-1) : result.end();
    auto e = result.size() > fr.second -1 ? result.begin()+(fr.second-1) : result.end();
    return LineFields(std::move(b),std::move(e));
}

auto process_file( const std::string& fname, const std::wregex& re, FieldRange fr )
     -> ListLineFields
{
    std::wifstream ifs{fname};
    if( !ifs )
        throw std::runtime_error( "Failed to open the file " + fname );

    ListLineFields result;
    for( wstring line; getline( ifs, line ); ){
        result.push_back( process_line(line, re, fr) );
    }
    return result;
}

auto process_files( const vector<string>& files, const std::wregex& re, FieldRange fr )
     -> ListLineFields
{
    ListLineFields result;
    std::list<std::future<ListLineFields>> intermediate_results;

    for( const auto& f : files ){
        intermediate_results.push_back( std::async(process_file, f, re, fr) );
    }

    for( auto&& ir : intermediate_results ){
        result.splice( result.cend(), std::move(ir.get()) );
    }
    return result;
}

auto operator<<( std::wostream& wos, const LineFields& lfs ) -> std::wostream&
{
    for( const auto& f : lfs ){
        wcout << f << ", ";
    }
    return wos;
}

auto operator<<( std::wostream& wos, const ListLineFields& llfs ) -> std::wostream&
{
    for( const auto& lf : llfs ){
        wos << lf << endl;
    }
    return wos;
}
