#include <iostream>
#include "Sh.h"

#include "clara/clara.hpp"

using clara::Opt; using clara::Arg; using clara::Help;


int main(/* int argc, char* argv[] */)
{
    auto shell = Sh{std::cin};
    shell.run();
}
