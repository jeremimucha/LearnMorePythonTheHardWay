 #include <iostream>
#include <string>

#include "clara/clara.hpp"

#include "Translator.h"
#include "Translating_stream.h"
#include "Filtering_istream.h"

using clara::Opt; using clara::Arg; using clara::Help;

struct cmdline_args {
    std::string set1;
    std::string set2;
    bool complement;
    bool del;
    bool squeeze;
    bool truncate;
    bool help_flag;
};

int main(int argc, char* argv[])
{
    auto cli_args = cmdline_args{};
    auto cli
        = Opt( cli_args.complement )
            ["-c"]["-C"]["--complement"]("use the complement of SET1")
        | Opt( cli_args.del )
            ["-d"]["--delete"]("delete characters in SET1, do not translate")
        | Opt( cli_args.squeeze )
            ["-s"]["--squeeze-repeats"]
            ("repleace each sequence of repeated character that is listed in the last SET"\
             " with a single occurence of that character")
        | Opt( cli_args.truncate )
            ["-t"]["--truncate_set1"]("first truncate SET1 to length of SET2")
        | Arg( cli_args.set1, "SET1").required()
        | Arg( cli_args.set2, "SET2")
        | Help( cli_args.help_flag );

    auto cli_result = cli.parse(clara::Args(argc,argv));
    if( argc < 2 || !cli_result || cli_args.help_flag 
        || (cli_args.del && !cli_args.set1.empty()) ){
        std::cerr << cli << std::endl;
        return 1;
    }

    if( cli_args.complement ){
        std::cerr << "Complement not yet implemented";
        return 2;
    }

    auto tos = Translating_stream{std::cout, {cli_args.set1, cli_args.set2}};
    auto fis = Filtering_istream{std::cin};
    if(cli_args.del && cli_args.squeeze){
        fis.del(true);
        fis.del_set(cli_args.set1);
        fis.squeeze(true);
        fis.squeeze_set(cli_args.set2);
    }
    else if(cli_args.del){
        fis.del(true);
        fis.del_set(cli_args.set1);
    }
    else if(cli_args.squeeze){
        fis.squeeze(true);
        fis.squeeze_set(cli_args.set1);
    }

    if( cli_args.truncate )
        tos.truncate();
    for( char ch; fis.get(ch); ){
        tos << ch;
    }
}
