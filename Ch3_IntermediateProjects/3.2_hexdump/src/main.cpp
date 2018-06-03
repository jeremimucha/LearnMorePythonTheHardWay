#include <iostream>
#include <fstream>
#include <cstdio>
#include <iomanip>
#include <memory>

#include "clara/clara.hpp"

using byte = char;


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


class Reader
{
public:
    explicit Reader(const char* fname) noexcept
        :fh_{fname}, isgood{fh_} { }

    template<typename charT, typename traits>
    void do_print_ascii(std::basic_ostream<charT,traits>& os, std::streamsize n) const
    {
        os << "|";
        for(std::streamsize i=0; i<n; ++i)
            os << (std::isprint(buffer[i]) ? buffer[i] : '.');
        os << "|";
    }

    template<typename charT, typename traits>
    void do_print_hex(std::basic_ostream<charT,traits>& os, std::streamsize n) const
    {
        auto del = [flags=os.setf(std::ios::hex, std::ios::basefield)](std::basic_ostream<charT,traits>* pos){
            pos->setf(flags);
        };
        auto finally = std::unique_ptr<std::basic_ostream<charT,traits>,decltype(del)>{&os,del};
        // os.unsetf(std::ios::dec);

        std::streamsize i = 0;
        for( ; i < n && i < Chunk/2; ++i)
            os << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(buffer[i]) << " ";
        os << "  ";
        for( ; i < n; ++i)
            os << static_cast<unsigned int>(buffer[i]) << " ";
        for( ; i < Chunk; ++i)
            os << "   ";
    }

    template<typename charT, typename traits>
    void do_print_offset(std::basic_ostream<charT,traits>& os) const
    {
        os << std::setw(8) << std::setfill('0') << std::hex << count;
    }

    auto read() const
    {
        return fh_.read(buffer);
    }

    template<typename charT, typename traits> friend
    std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT,traits>& os, const Reader& fh)
    {
        fh.do_print_offset(os);
        std::streamsize n = fh.read();
        if(n) {
            count += n;
            os << "  ";
            fh.do_print_hex(os, n);
            os << "  ";
            fh.do_print_ascii(os, n);
        }
        else
            fh.isgood = false;
        return os << "\n";
    }

    explicit operator bool() const noexcept { return isgood; }

private:
    static std::streamsize count;
    static constexpr std::streamsize Chunk{16};

    mutable byte buffer[Chunk];
    mutable File_handle fh_;
    mutable bool isgood{true};
};
std::streamsize Reader::count{0};


int main()
{
    {
    auto inbuf = std::filebuf{};
    inbuf.open("test.dat", std::ios::in | std::ios::binary);

    std::streamsize n{0};
    byte outbuf[16];
    while( (n = inbuf.sgetn(outbuf, 16)) == 16 ) {
        std::cout.write(outbuf, n);
    }
    std::cout.write(outbuf, n);
    std::cout << std::endl;
    }

    {
    auto fh = File_handle{"test.dat"};
    std::streamsize n{0};
    byte outbuf[16];
    while( (n = fh.read(outbuf)) == 16 ){
        std::cout.write(outbuf, n);
    }
    std::cout.write(outbuf,n);
    std::cout << std::endl;
    }

    {
        auto fh = Reader{"test.dat"};
        while(fh){
            std::cout << fh;
        }
        std::cout << std::endl;
    }
}
