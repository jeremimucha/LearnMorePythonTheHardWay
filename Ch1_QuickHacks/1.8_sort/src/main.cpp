#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <iterator>
#include <utility>
#include <functional>
#include <future>
#include <clara/clara.hpp>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::list;
using clara::Opt;
using clara::Arg;
using clara::Help;


struct commandline_args
{
    bool ignore_case;
    bool reverse_order;
    bool ignore_leading_blanks;
    bool dictionary_order;
    bool random_sort;
    bool check;
    vector<string> infiles;
    string outfile;
    bool help_flag;
};

using Lines = vector<string>;
using Line_list = list<string>;
template<typename Container>
auto get_lines( std::istream& is ) -> Container;

// template<typename Container>
// auto operator<<( std::ostream& os, const Container& lines ) -> std::ostream&;
auto operator<<( std::ostream& os, const vector<string>& lines ) -> std::ostream&;

template<typename Container, typename Predicate>
auto sort_lines( Container& lines, Predicate pred ) -> Container&;

template<typename Container, typename Predicate>
auto process_file( const string& fname, Predicate pred ) -> Container;

template<typename OutContainer, typename InContainer, typename Predicate>
auto process_files( const InContainer& files, Predicate pred ) -> OutContainer;

template<typename Container>
auto write_lines( const string& fname, const Container& container ) -> void;


int main( int argc, char* argv[] )
{
    auto cli_args = commandline_args{};
    auto cli
        = Opt( cli_args.ignore_case, "Treat all lowercase as uppercase" )
             ["-f"]["--ignore-case"]
        | Opt( cli_args.reverse_order, "Reverse the comparison" )
             ["-r"]["--reverse"]
        | Opt( cli_args.outfile, "Output file" )
             ["-o"]["--output-file"].required()
        | Arg( cli_args.infiles, "[FILE]..." )
        | Help( cli_args.help_flag );

    auto cli_result = cli.parse( clara::Args(argc, argv) );
    if( !cli_result || cli_args.help_flag ){
        cout << cli << endl;
        return 1;
    }

    if( !cli_args.infiles.empty() ){
        auto lines = process_files<Line_list>( cli_args.infiles, std::less<string>() );
        write_lines(cli_args.outfile, lines);
    }
    else{
        auto lines = get_lines<Lines>(std::cin);
        std::sort(lines.begin(), lines.end(), std::less<string>());
        write_lines(cli_args.outfile, lines);
    }
}

template<typename Container>
auto get_lines( std::istream& is ) -> Container
    // requires .push_back()
{
    auto lines = Container();
    for( string line; getline(is, line); ){
        lines.push_back(std::move(line));
    }
    return lines;
}

// template<typename Container>
// std::ostream& operator<<( std::ostream& os, const Container& lines )
// {
//     for( auto it = lines.cbegin(); it != lines.cend(); ){
//         os << *it++ << "\n";
//     }
//     // for( const auto& line : lines ){
//     //     os << line << "\n";
//     // }
//     return os;
// }
auto operator<<( std::ostream& os, const vector<string>& lines ) -> std::ostream&
{
    for( auto it = lines.cbegin(); it != lines.cend(); ++it){
        os << *it << "\n";
    }
    return os;
}

template<typename Container, typename Predicate>
auto sort_lines( Container& lines, Predicate pred ) -> Container&
{
    lines.sort(pred);
    return lines;
}

template<typename Container, typename Predicate>
auto process_file( const string& fname, Predicate pred ) -> Container
{
    auto ifs = std::ifstream{fname};
    auto lines = get_lines<Container>(ifs);
    sort_lines(lines, pred);
    return lines;
}

template<typename OutContainer, typename InContainer, typename Predicate>
auto process_files( const InContainer& files, Predicate pred ) -> OutContainer
{
    auto intermediate_results = vector<std::future<OutContainer>>();
    for( const auto& file : files ){
        intermediate_results.push_back( std::async(
            process_file<OutContainer, Predicate>, file, pred) );
    }
    auto results = Line_list();
    for( auto&& fresult : intermediate_results ){
        auto result = std::move(fresult.get());
        results.splice( std::upper_bound(results.cbegin(), results.cend(), result.front()),
                        std::move(result) );
    }
    return results;
}

template<typename Container>
auto write_lines( const string& fname, const Container& lines ) -> void
{
    auto ofs = std::ofstream{fname};
    std::ostream& os = ofs ? ofs : cout;
    for( const auto& line : lines ){
        os << line << "\n";
    }
}
