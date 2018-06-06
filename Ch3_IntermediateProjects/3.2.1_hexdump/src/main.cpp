#include <iostream>
#include <fstream>
#include <cstdio>
#include <iomanip>
#include <memory>
#include <utility>

#include "clara/clara.hpp"

using clara::Opt; using clara::Arg; using clara::Help;

using byte = char;
using offset_t = std::streamsize;

class File_handle
{
public:
    explicit File_handle(const char* fname)
        : fp_{ fopen(fname, "rb") } {}

    ~File_handle() noexcept { fclose(fp_); }

    bool is_open() const noexcept { return fp_; }

    template<typename T, std::size_t N>
    auto read(T(&buffer)[N])
    {
        return fread(buffer, sizeof(T), N, fp_);
    }

    explicit operator bool() const noexcept { return fp_; }

private:
    FILE* fp_;
};

struct HexChunk
{
    static constexpr int MaxChunk{16};
    byte buffer[MaxChunk];
    int n;
    offset_t offset;
};

class IPrinter
{
public:
    // IPrinter() noexcept = default;
    virtual ~IPrinter() noexcept = default;
    virtual void print(std::ostream& os, const HexChunk&) const = 0;
};

class Printer_base : public IPrinter
{
protected:
    // Printer_base() noexcept = default;
    // ~Printer_base() noexcept = default;
    void do_print_offset(std::ostream& os, offset_t offset) const
    {
        os << std::setw(8) << std::setfill('0') << std::hex << offset;
    }
};

class PrinterHex : public Printer_base
{
public:
    // PrinterHex() noexcept = default;
    // ~PrinterHex() noexcept = default;
    void print(std::ostream& os, const HexChunk& chunk) const override
    {
        do_print_offset(os, chunk.offset);
        os << "  ";
        {
        const auto del = [flags=os.setf(std::ios::hex, std::ios::basefield)]
                (std::ostream* pos){pos->setf(flags);};
        const auto finally = std::unique_ptr<
            std::ostream,decltype(del)>{&os,del};
        for(int i=0; i != chunk.n; ++i){
            os << std::setfill('0') << std::setw(2)
               << static_cast<unsigned int>(chunk.buffer[i]) << " ";
        }
        }
    }
};

class PrinterCanonical : public Printer_base
{
public:
    // PrinterCanonical() noexcept = default;
    // ~PrinterCanonical() noexcept = default;
    void print(std::ostream& os, const HexChunk& chunk) const override
    {
        do_print_offset(os, chunk.offset);
        if(chunk.n){
        os << "  ";
        {
        const auto del = [flags=os.setf(std::ios::hex, std::ios::basefield)]
                (std::ostream* pos){pos->setf(flags);};
        const auto finally = std::unique_ptr<
            std::ostream,decltype(del)>{&os,del};
        int i = 0;
        for( ; i != chunk.n && i != HexChunk::MaxChunk/2; ++i)
            os << std::setfill('0') << std::setw(2)
               << static_cast<unsigned int>(chunk.buffer[i]) << " ";
        os << " ";
        for( ; i != chunk.n; ++i )
            os << std::setfill('0') << std::setw(2)
               << static_cast<unsigned int>(chunk.buffer[i]) << " ";
        for( ; i != HexChunk::MaxChunk; ++i )
            os << "   ";
        }
        os << " ";
        os << "|";
        for(int i = 0; i != chunk.n; ++i)
            os << (std::isprint(chunk.buffer[i]) ? chunk.buffer[i] : '.');
        os << "|";
        }
    }
};

class PrinterOctal : public Printer_base
{
public:
    // PrinterOctal() noexcept = default;
    // ~PrinterOctal() noexcept = default;
    void print(std::ostream& os, const HexChunk& chunk) const override
    {
        do_print_offset(os, chunk.offset);
        os << " ";
        {
            const auto del = [flags=os.setf(std::ios::oct,std::ios::basefield)]
                (std::ostream* pos){pos->setf(flags);};
            const auto finally = std::unique_ptr<
                std::ostream,decltype(del)>{&os,del};
            for(int i=0; i != chunk.n; ++i){
            os << std::setfill('0') << std::setw(2)
               << static_cast<unsigned int>(chunk.buffer[i]) << " ";
            }
        }
    }
};

class PrinterChar : public Printer_base
{
public:
    // PrinterChar() noexcept = default;
    // ~PrinterChar() noexcept = default;
    void print(std::ostream& os, const HexChunk& chunk) const override
    {
        do_print_offset(os, chunk.offset);
        os << "  ";
        for(int i=0; i != chunk.n; ++i){
            os << (std::isprint(chunk.buffer[i]) ? chunk.buffer[i] : '.');
        }
    }
};

class Hexargs
{
public:
    explicit Hexargs(const char* fname, IPrinter* pr) noexcept
        :fh{fname}, printer{pr}, isgood{fh} { }

    auto read() const
    {
        const auto res = fh.read(chunk.buffer);
        chunk.n = res;
        chunk.offset += res;
        return res;
    }

    template<typename charT, typename traits> friend
    std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT,traits>& os, const Hexargs& fh)
    {
        offset_t n = fh.read();
        fh.printer->print(std::cout, fh.chunk);
        if(!n)
            fh.isgood = false;
        return os << "\n";
    }

    explicit operator bool() const noexcept { return isgood; }

private:
    mutable HexChunk chunk{{0},0,0};
    mutable File_handle fh;
    IPrinter* printer;
    mutable bool isgood{true};
};

struct commandline_args {
    std::string input_file{};
    bool octal;
    bool character;
    bool canonical;
    bool help_flag;
};

int main(int argc, char* argv[])
{
    auto cli_args = commandline_args{};
    auto cli
        = Opt( cli_args.octal, "Format")
            ["-b"]["--one-byte-octal"]("One byte octal display")
        | Opt( cli_args.character, "Format")
            ["-c"]["--one-byte-char"]("One byte character display")
        | Opt( cli_args.canonical, "Format")
            ["-C"]["--canonical"]("Canonical hex+ascii display")
        | Arg( cli_args.input_file, "Input file" )
        | Help( cli_args.help_flag );

    auto cli_result = cli.parse(clara::Args(argc,argv));
    if( !cli_result || cli_args.help_flag ){
        std::cerr << cli << std::endl;
        return 1;
    }

    auto printer = [&cli_args](){
        auto p = std::unique_ptr<IPrinter>{};
        if(cli_args.canonical)
            p.reset(new PrinterCanonical);
        else if(cli_args.octal)
            p.reset(new PrinterOctal);
        else if(cli_args.character)
            p.reset(new PrinterChar);
        else 
            p.reset(new PrinterHex);
        return p;
    }();

    auto fh = Hexargs{cli_args.input_file.c_str(), printer.get()};
    while(fh){
        std::cout << fh;
    }
    std::cout << std::endl;
}
