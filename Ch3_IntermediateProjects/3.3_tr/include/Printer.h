#pragma once

#include <ostream>
#include <iomanip>
#include "HexChunk.h"

class Printer_base
{
public:
    explicit Printer_base(std::ostream& strm) noexcept
        : os_{strm.rdbuf()}, offset_os_{strm.rdbuf()}
        {
            offset_os_.setf(std::ios_base::hex, std::ios_base::basefield);
        }

friend Printer_base& operator<<(Printer_base& printer, std::ostream&(*op)(std::ostream&))
{
    printer.os_ << op;
    return printer;
}

friend Printer_base& operator<<(Printer_base& printer, const char* str) noexcept
{
    printer.os_ << str;
    return printer;
}

friend Printer_base& operator<<(Printer_base& printer, const offset_t offset) noexcept
{
    printer.offset_os_ << std::setw(8) << std::setfill('0') << offset;
    return printer;
}

friend Printer_base& operator<<(Printer_base& printer, const Hex_chunk& chunk) noexcept
{
    printer << chunk.offset;
    printer.os_ << "  ";
    for(int i = 0; i < chunk.n; ++i){
        printer.os_ << std::setfill('0') << std::setw(2)
            << static_cast<unsigned int>(chunk.buffer[i]) << " ";
    }
    return printer;
}
protected:
    std::ostream os_;
    std::ostream offset_os_;
};

class Printer_hex : public Printer_base
{
public:
    explicit Printer_hex(std::ostream& strm) noexcept
        : Printer_base{strm}
        {
            os_.setf(std::ios::hex, std::ios::basefield);
        }
};

class Printer_octal final : public Printer_base
{
public:
    explicit Printer_octal(std::ostream& strm) noexcept
        : Printer_base{strm}
        {
            os_.setf(std::ios::oct, std::ios::basefield);
        }

friend Printer_octal& operator<<(Printer_octal& printer, const Hex_chunk& chunk) noexcept
{
    printer << chunk.offset;
    printer.os_ << "  ";
    for(int i = 0; i < chunk.n; ++i){
        printer.os_ << std::setfill('0') << std::setw(3)
            << static_cast<unsigned int>(chunk.buffer[i]) << " ";
    }
    return printer;
}

};

class Printer_char final : public Printer_base
{
public:
    explicit Printer_char(std::ostream& strm) noexcept
        : Printer_base{strm}
        {
        }

friend Printer_char& operator<<(Printer_char& printer, const Hex_chunk& chunk) noexcept
{
    printer << chunk.offset;
    printer.os_ << "  ";
    for(int i = 0; i < chunk.n; ++i){
        printer.os_ << std::setw(2)
            << (std::isprint(chunk.buffer[i]) ? chunk.buffer[i] : '.') << " ";
    }
    return printer;
}

};

class Printer_canonical final : public Printer_hex
{
public:
    explicit Printer_canonical(std::ostream& strm) noexcept
        : Printer_hex{strm}, ascii_os_{strm} { }

friend Printer_canonical& operator<<(Printer_canonical& printer, const Hex_chunk& chunk) noexcept
{
    printer << chunk.offset;
    if(chunk.n)
    {
        printer.os_ << "  ";
        int i = 0;
        for( ; i != chunk.n && i != Hex_chunk::MaxChunk/2; ++i)
            printer.os_ << std::setfill('0') << std::setw(2)
               << static_cast<unsigned int>(chunk.buffer[i]) << " ";
        printer.os_ << " ";
        for( ; i != chunk.n; ++i )
            printer.os_ << std::setfill('0') << std::setw(2)
               << static_cast<unsigned int>(chunk.buffer[i]) << " ";
        for( ; i != Hex_chunk::MaxChunk; ++i )
            printer.os_ << "   ";
        printer.os_ << " ";
        printer.os_ << "|";
        for(int i = 0; i != chunk.n; ++i)
            printer.os_ << (std::isprint(chunk.buffer[i]) ? chunk.buffer[i] : '.');
        printer.os_ << "|";
    }
    return printer;
}
private:
    std::ostream& ascii_os_;
};
