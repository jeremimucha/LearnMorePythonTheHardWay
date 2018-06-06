#include <iostream>
#include <fstream>
// #include <iomanip>
#include <memory>
#include <utility>

#include "clara/clara.hpp"

#include "HexChunk.h"
#include "HexChunkStream.h"
#include "Printer.h"

using clara::Opt; using clara::Arg; using clara::Help;

struct commandline_args {
    std::string input_file{};
    bool octal;
    bool character;
    bool canonical;
    bool help_flag;
};

namespace
{

Printer_hex       oshex{std::cout};
Printer_octal     osoct{std::cout};
Printer_char      oschar{std::cout};
Printer_canonical oscanon{std::cout};

}


int main(int argc, char* argv[])
{
    auto cli_args = commandline_args{};
    auto cli
        = Opt( cli_args.octal )
            ["-b"]["--one-byte-octal"]("One byte octal display")
        | Opt( cli_args.character )
            ["-c"]["--one-byte-char"]("One byte character display")
        | Opt( cli_args.canonical )
            ["-C"]["--canonical"]("Canonical hex+ascii display")
        | Arg( cli_args.input_file, "Input file" )
        | Help( cli_args.help_flag );

    auto cli_result = cli.parse(clara::Args(argc,argv));
    if( !cli_result || cli_args.help_flag ){
        std::cerr << cli << std::endl;
        return 1;
    }
    auto infile = std::ifstream{cli_args.input_file};
    if( !cli_args.input_file.empty() && !infile.is_open()){
        std::cerr << "Failed to open the file" << cli_args.input_file;
        return 1;
    }
    auto chunk_stream = [fileopen=infile.is_open(),&infile=infile](){
        if(fileopen)
            return Hex_chunk_stream{infile};
        else
            return Hex_chunk_stream{std::cin};
    }();

    if(cli_args.canonical){
        for(Hex_chunk chunk{{0},0,0}; chunk_stream >> chunk;)
            oscanon << chunk << std::endl;;
    }
    else if(cli_args.octal){
        for(Hex_chunk chunk{{0},0,0}; chunk_stream >> chunk;)
            osoct << chunk << std::endl;;
    }
    else if(cli_args.character){
        for(Hex_chunk chunk{{0},0,0}; chunk_stream >> chunk;)
            oschar << chunk << std::endl;;
    }
    else{
        for(Hex_chunk chunk{{0},0,0}; chunk_stream >> chunk;)
            oshex << chunk << std::endl;;
    }
}
