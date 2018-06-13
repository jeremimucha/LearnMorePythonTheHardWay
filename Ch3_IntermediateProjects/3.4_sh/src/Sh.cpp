#include <iostream>
#include <system_error>
#include <boost/process.hpp>

#include "Sh.h"

namespace bp = boost::process;

void Sh::run()
{
    while(ts_)
    try{
        std::cout << Prompt;
        auto t = ts_.get();
        while(t.kind() == Kind::Terminator)
            t = ts_.get();
        switch(t.kind()){
            case Kind::Quit:
                std::cerr << "bye!" << std::endl;
                return;
            case Kind::Cmd:{
                // auto term = ts_.get();
                    run_command(t.value());
                // if(term.kind() == Kind::Pipe)
                //     throw std::runtime_error("Piping not yet implemented");
                // else if(term.kind() != Kind::Terminator)
                //     throw std::runtime_error("Terminator or pipe expected after a command");
                break;
            }
            case Kind::Terminator:
                // throw std::runtime_error("Sh: syntax error, unexpected token " + t.value());
                break;
            case Kind::Pipe:
                throw std::runtime_error("Sh: pipe not yet implemented");
            default:
                throw std::runtime_error("Sh: unknown error");
        }
    }
    catch(std::exception& e){
        std::cerr << e.what() << std::endl;
        ts_.ignore(Kind::Terminator);
    }
}

void Sh::run_command(const std::string& cmd)
{
    auto ec = std::error_code{};
    auto proc = bp::child{cmd, /* bp::std_out > stdout,
                                bp::std_err > stderr,
                                bp::std_in < stdin, */
                                ec};
    proc.wait();
    if(ec) {
        std::cerr << "Sh: " << ec.message() << std::endl;
    }
}
