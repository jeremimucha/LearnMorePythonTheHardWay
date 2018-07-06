#include <iostream>
#include "Token.h"
#include "Scanner.h"
#include "Parser.h"


int main()
{
    auto parser = Parser{std::cin};
    try{
    for(auto p = parser.root(); ; p = parser.root()){
        std::cout << p.rep() << std::endl;
        if(!parser)
            break;
    }
    }
    catch(const std::exception& err){
        std::cout << "\nParsing error: " << err.what();
    }
    return 0;
}
