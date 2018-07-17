#include <iostream>
#include "Parser.h"
#include "PunyPyWorld.h"
#include "Analyzer.h"


int main()
{
    auto parser = Parser{std::cin};
    auto prods = std::vector<Production>();
    for(auto p = parser.root(); ; p = parser.root())
    try{
            std::cout << p.rep() << std::endl;
            prods.push_back(std::move(p));
            if(!parser)
                break;
    }
    catch(std::exception& err){
        std::cerr << "\nParsing error: " << err.what();
    }

    auto world = PunyPyWorld{};
    auto analyzer = Analyzer{world, prods};

    try{
        analyzer.analyze();
    }
    catch(std::exception& err){
        std::cerr << "\nAnalysis error: " << err.what();
    }

    return 0;
}
