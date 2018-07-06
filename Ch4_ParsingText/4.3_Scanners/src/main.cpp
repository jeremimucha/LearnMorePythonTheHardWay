#include <iostream>
#include "Token.h"
#include "Scanner.h"


int main()
{
    auto scanner = Scanner{std::cin};
    for(Token t = scanner.get(); scanner; t = scanner.get())
        std::cout << t << "\n";
    return 0;
}
