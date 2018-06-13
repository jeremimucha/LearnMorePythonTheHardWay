#pragma once

#include <iosfwd>
#include <string>

#include "Keywords.h"
#include "Token.h"
#include "TokenStream.h"

class Sh
{
public:
    explicit Sh(std::istream& is)
        : ts_{is} { }

    void run();
private:
    static void run_command(const std::string&);
    TokenStream ts_;
};
