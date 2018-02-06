#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <regex>
#include <utility>
#include <clara/clara.hpp>
#include <boost/filesystem.hpp>

#include <future>

namespace fs = boost::filesystem;

using std::vector; using std::string;
using std::cout; using std::endl;

using clara::Opt; using clara::Help;
using clara::Arg;

namespace
{
bool g_help_flag{false};
string g_start_path;
string g_name{};
} // namespace


vector<string> parallel_find_name( const fs::path& p, const std::regex& re );


int main( int argc, char* argv[] )
{
    auto clip
            = Arg( g_start_path, "Path where the search should begin" )
            | Opt( g_name, "Something" )["--name"]
                 ("File names to find")
            | Help( g_help_flag );
    auto clip_result = clip.parse( clara::Args(argc, argv) );
    if( !clip_result || g_help_flag ){
        cout << clip;
        return 0;
    }

    fs::path start_path(g_start_path);
    if( !is_directory(start_path) ){
        cout << clip;
        return 1;
    }

    if( !g_name.empty() ){
        std::regex re(g_name);
        auto paths = parallel_find_name( start_path, re );
        for( const auto& p : paths ){
            cout << p << endl;
        }
        // for( const auto& de : fs::recursive_directory_iterator(start_path) )
        // {
        //     auto path = de.path();
        //     if( is_regular_file(path)
        //         && std::regex_search(path.filename().string(), re) )
        //         std::cout << path.string() << endl;
        // }
    }
}


vector<string> parallel_find_name( const fs::path& p, const std::regex& re )
{
    std::vector<std::future<std::vector<std::string>>> future_results;
    std::vector<std::string> results;
    for( const auto& de : fs::directory_iterator(p) )
    {
        auto path = de.path();
        if(is_directory(path)){
            future_results.push_back(std::async(parallel_find_name, path, re));
        }
        else if( is_regular_file(path)
                    && std::regex_search(path.filename().string(), re) ){
            results.push_back(path.string());
        }
    }
    for( auto& f : future_results ){
        auto res = f.get();
        for( auto&& s : res )
            results.push_back(std::move(s));
    }
    return results;
}
