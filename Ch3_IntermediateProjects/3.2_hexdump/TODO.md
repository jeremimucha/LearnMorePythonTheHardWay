# Hexdump

## 'research'
hexdump displays file contents in hexadecimal, decimal, octal or ascii
* displays the specified file or the stdin if no files are specified

syntax:
hexdump [options] file...

## Ideas, observations

* The options are genarally different formattings, it shouldn't be too difficoult to implement most of them. Start with just straight hex output.
* How to read input? Some options are std::ifstream, std::filebuf or just good old fread... Since I need to consider stdin it might be cleanest to try to use a std::filebuf to open the file (if any) and assign an appropriate buffer to a new istream object.
* To minimize ram usage it'd be best to output each chunk of data right after reading it. I could define a class with an overloaded output operator that actually reads the binary data and then outputs it to stdout formatted appropriately. Could maybe do a template (tag dispatch somehow?) to accomodate all of the different formatting options.

## Feature list
* Read data from a file and output it in hex
* Read data from stdin and output it in hex
* Options:
-b, --one-byte-octal
    One-byte octal display.  Display the input offset in hexadecimal, followed by sixteen space-separated, three-column, zero-filled bytes of input data, in octal, per line.

-c, --one-byte-char
    One-byte  character  display.   Display the input offset in hexadecimal, followed by sixteen space-separated, three-column, space-filled characters of input data per line.

-C, --canonical
    Canonical hex+ASCII display.  Display the input offset in hexadecimal, followed by sixteen space-separated, two-column, hexadecimal bytes, followed by the same sixteen bytes in %_p format enclosed in '|' characters.

-d, --two-bytes-decimal
    Two-byte  decimal display.  Display the input offset in hexadecimal, followed by eight space-separated, five-column, zero-filled, two-byte units of input data, in unsigned decimal, per line.

-n, --length length
        Interpret only length bytes of input.

-o, --two-bytes-octal
        Two-byte  octal  display.   Display  the input offset in hexadecimal, followed by eight space-separated, six-column, zero-filled, two-byte quantities of input data, in octal, per line.

-s, --skip offset
        Skip offset bytes from the beginning of the input.

-x, --two-bytes-hex
        Two-byte hexadecimal display.  Display the input offset in hexadecimal, followed by eight space-separated, four-col‐
umn, zero-filled, two-byte quantities of input data, in hexadecimal, per line.

-h, --help
        Display help text and exit.

## TODO:
* Start with just the file input
* Figure out the basic formatting, don't bother encapsulating in a class for now
* Try defining a class with an appropriate output operator - once that's done it's just a matter of figuring out all of the appropriate formattings.
