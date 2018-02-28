#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <iterator>
#include <utility>
#include <stdexcept>
#include <gsl/gsl>
#include "clara/clara.hpp"

using std::cout; using std::endl; using std::cin;
using std::string; using std::vector; using std::list;
using clara::Opt; using clara::Help; using clara::Arg;



struct commandline_args
{
    bool count{false};
    bool repeated{false};
    bool repeated_all{false};
    bool ignore_case{false};
    bool unique{false};
    std::string infile;
    std::string outfile;
    bool help_flag{false};
};

struct Line
{
    /* explicit */ Line( const std::string& line )
        : count{1}, data{line} { }
    /* explicit */ Line( std::string&& line )
        : count{1}, data{std::move(line)} { }
    Line& operator++() noexcept { ++count; return *this; }
    Line  operator++(int)
    {
        auto rv = *this;
        ++*this;
        return rv;
    }
    operator gsl::cstring_span<>() const noexcept { return data; }
    std::size_t count;
    std::string data;

    static bool print_count;
};
bool Line::print_count{false};

std::ostream& operator<<( std::ostream& os, const Line& line )
{
    if( Line::print_count )
        os << "\t" << line.count << " : ";
    return os << line.data;
}

bool operator == ( const Line& lhs, const Line& rhs )
{ return lhs.data == rhs.data; }
bool operator != ( const Line& lhs, const Line& rhs )
{ return !(lhs.data == rhs.data); }

inline
bool icase_compare(unsigned char lhs, unsigned char rhs) noexcept
{
    return ::toupper(lhs) == ::toupper(rhs);
}
inline
bool line_case_compare( gsl::cstring_span<> lhs, gsl::cstring_span<> rhs ) noexcept
{
    if(lhs.size() != rhs.size())
        return false;
    return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
}
inline
bool line_icase_compare( gsl::cstring_span<> lhs, gsl::cstring_span<> rhs ) noexcept
{
    if(lhs.size() != rhs.size())
        return false;
    return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), icase_compare);
}


class Lines
{
public:
    using Predicate = bool (*)(gsl::cstring_span<>, gsl::cstring_span<>) noexcept;
    using value_type = vector<Line>::value_type;
    using iterator = vector<Line>::iterator;
    using const_iterator = vector<Line>::const_iterator;
    explicit Lines( const commandline_args& args)
        : predicate_{ args.ignore_case ?
                       line_icase_compare
                      :line_case_compare
                    }
        , repeated{args.repeated}
        , repeated_all{args.repeated_all}
        , unique{args.unique}
        {}
    void push_back( string&& line ) noexcept
    {
        if( !data_.empty() && predicate_(data_.back(), line) )
            ++data_.back();
        else
            data_.emplace_back( std::move(line) );
    }
    void push_back( const string& line )
    {
        if( !data_.empty() && predicate_(data_.back(), line) )
            ++data_.back();
        else
            data_.emplace_back( line );
    }
    auto begin() noexcept { return data_.begin(); }
    auto end() noexcept { return data_.end(); }
    auto begin() const noexcept { return data_.cbegin(); }
    auto end() const noexcept { return data_.cend(); }
    auto cbegin() const noexcept { return data_.cbegin(); }
    auto cend() const noexcept { return data_.cend(); }

    friend std::ostream& operator<<( std::ostream&, const Lines& );
private:
    vector<Line> data_;
    Predicate predicate_;
    bool repeated{false};
    bool repeated_all{false};
    bool unique{false};
};

std::ostream& operator<<( std::ostream& os, const Lines& lines )
{
    for( const auto& line : lines ){
        if( lines.unique && line.count == 1 )
            os << line << endl;
        else if( lines.repeated && 1 < line.count )
            os << line << endl;
        else if( lines.repeated_all && 1 < line.count ){
            for(auto i = line.count; i != 0; --i ){
                os << line << endl;
            }
        }
        else if( !lines.unique && !lines.repeated && !lines.repeated_all )
            os << line << endl;
    }
    return os;
}

auto get_lines( const commandline_args& args ) -> Lines
{
    auto lines = Lines{args};
    auto ifs = std::ifstream{args.infile};
    auto& is = ifs.is_open() ? ifs : std::cin;
    for( string line; getline(is, line); ){
        lines.push_back(std::move(line));
    }
    return lines;
}

int main( int argc, char* argv[] )
try{
    auto cli_args = commandline_args{};
    auto cli
        = Opt( cli_args.count )
             ["-c"]["--count"] ("Prefix lines by the number of occurences")
        | Opt( cli_args.repeated )
             ["-d"]["--repeated"]("Print only duplicate lines, one for each group")
        | Opt( cli_args.repeated_all  )
             ["-D"]("Print ALL repeated lines")
        | Opt( cli_args.ignore_case )
             ["-i"]["--ignore-case"]("Ignore case difference when comparing")
        | Opt( cli_args.unique )
             ["-u"]["--unique"]("Print only unique lines")
        | Arg( cli_args.infile, "Input file path" )
        | Arg( cli_args.outfile, "Output file path" )
        | Help( cli_args.help_flag );

    auto cli_result = cli.parse( clara::Args(argc, argv) );
    if( !cli_result || cli_args.help_flag ){
        cout << cli << endl;
        return 1;
    }
    if( cli_args.count && cli_args.repeated_all ){
        cout << "Printing all duplicated lines and their counts is meaningless" 
                ". Try -? for more information" << endl;
        return 2;
    }
    Line::print_count = cli_args.count;
    auto res =  get_lines( cli_args );
    cout << res;
}
catch( const std::exception& e ){
    std::cerr << e.what() << endl;
}
