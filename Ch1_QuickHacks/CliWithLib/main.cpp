#include <iostream>
#include <string>
#include <vector>
#include "cxxopts/cxxopts.hpp"



int main( int argc, char* argv[] )
try{
    cxxopts::Options options(argv[0], "Testing out the cxxopts library");
    options.add_options()
        ("h,help", "Display help for this program")
        ("a,alpha", "A option - string", cxxopts::value<std::string>()
            ->implicit_value("Default A option value"), "BLA")
        ("b,bravo", "B option - int", cxxopts::value<int>())
        ("c,charlie", "C option - double", cxxopts::value<double>()
            ->default_value("3.141518"))
        ("d,d_flag", "D flag")
        ("e,e_flag","E flag")
        ("f,f_flag","F flag")
        ("positional",
            "Positional arguments - these are args entered without an option",
            cxxopts::value<std::vector<std::string>>())
        ;
    options.parse_positional({"e_flag", "f_flag", "positional"});

    auto result = options.parse(argc, argv);

    if( result.count("h") ){
        std::cout << options.help() << std::endl;
        exit(0);
    }

    if( result.count("b") ){
        std::cout << "Saw option 'b' " << result.count("b") << " times." << std::endl;
    }

    if( result.count("alpha") ){
        std::cout << "alpha = " << result["alpha"].as<std::string>()
            << std::endl;
    }
    if( result.count("bravo") ){
        std::cout << "bravo = " << result["bravo"].as<int>()
            << std::endl;
    }
    if( result.count("charlie") ){
        std::cout << "charlie = " << result["charlie"].as<double>()
            << std::endl;
    }

    if( result.count("d_flag") ){
        std::cout << "d_flag = " << result["d_flag"].as<bool>()
            << std::endl;
    }
    if( result.count("e_flag") ){
        std::cout << "e_flag = " << result["e_flag"].as<bool>()
            << std::endl;
    }
    if( result.count("f_flag") ){
        std::cout << "f_flag = " << result["f_flag"].as<bool>()
            << std::endl;
    }

    if( result.count("positional") ){
        std::cout << "Positional = {";
        auto& v = result["positional"].as<std::vector<std::string>>();
        for( const auto& s : v ){
            std::cout << s << ", ";
        }
        std::cout << "}" << std::endl;
    }

    return 0;
}
catch( const cxxopts::OptionException& e ){
    std::cout << "error parsing options: " << e.what() << std::endl;
    return 1;
}
