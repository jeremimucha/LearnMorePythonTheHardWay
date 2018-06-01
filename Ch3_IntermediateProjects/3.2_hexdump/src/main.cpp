#include <iostream>
#include <fstream>
#include <cstdio>
#include <string_view>

using byte = char;

class file_handle
{
public:
    explicit file_handle(const char* fname) noexcept
        :fh_{std::fopen(fname, "rb")} { }
    ~file_handle() { fclose(fh_); }

    size_t read(void* buffer, size_t size, size_t count)
    {
        return fread(buffer, size, count, fh_);
    }

private:
    FILE* fh_;
};

int main()
{
    // auto inbuf = std::filebuf{};
    // inbuf.open("test.dat", std::ios::in | std::ios::binary);

    // std::streamsize n{0};
    // byte outbuf[16];
    // while( (n = inbuf.sgetn(outbuf, 16)) == 16 ) {
    //     std::cout.write(outbuf, n);
    // }
    // std::cout.write(outbuf, n);
    // std::cout << std::endl;

    auto fh = file_handle{"test.dat"};
    std::streamsize n{0};
    byte outbuf[16];
    while( (n = fh.read(outbuf, sizeof(byte), sizeof(outbuf))) == 16 ){
        std::cout.write(outbuf, n);
    }
    std::cout.write(outbuf,n);
    std::cout << std::endl;
}
