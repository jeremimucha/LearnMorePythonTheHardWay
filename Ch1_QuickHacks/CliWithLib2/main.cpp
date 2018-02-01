#include <iostream>
#include <string>
#include <vector>
#include "clara/clara.hpp"

using clara::Opt;
using clara::Arg;
using clara::Help;

int int_option{11};
double double_option{3.1415};
std::string string_option{"default_value"};
bool a_flag{false};
bool b_flag{false};
bool help_flag{false};
std::vector<std::string> remaining;

int main( int argc, char* argv[] )
{
    auto cliparser
            = Opt( int_option, "int vlaue" )
                 ["-i"]["--int"]
                 ("Integer value description")
                 .required()
            | Opt( double_option, "double value" )
                 ["-d"]["--double"]
                 ("Double value description")
            | Opt( string_option, "string value" )
                 ["-s"]["--string"]
                 ("String value description")
            | Opt( a_flag )
                 ["-a"]["--a_flag"]
                 ("Set the a_flag")
            | Opt( b_flag )
                 ["-b"]["--b_flag"]
                 ("set the b_flag")
            | Arg( remaining, "Remaining arguments as strings" )
            | Help( help_flag );
    auto result = cliparser.parse( clara::Args(argc, argv) );

    if( !result ){
        std::cout << "parse failed\n" << cliparser << std::endl;
        exit(1);
    }

    if( help_flag ){
        std::cout << cliparser;
        exit(0);
    }
    std::cout << "int_option = " << int_option << std::endl;
    std::cout << "double_option = " << double_option << std::endl;
    std::cout << "string_option = " << string_option << std::endl;
    std::cout << std::boolalpha << "a_flag set? = " << a_flag << std::endl;
    std::cout << "b_flag set? = " << b_flag << std::endl;
    std::cout << "remaining:\n";
    for( const auto& s : remaining ){
        std::cout << s << ", ";
    }
    std::cout << std::endl;
}
