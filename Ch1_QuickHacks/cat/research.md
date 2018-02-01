# cat command 'research'

The cat command takes a list of files (passed via command line) and writes
each files contents to stdout. Combined with stream redirection it can
concatenate all the files in to one:
$ cat file_one file_two file_three > outfile

man cat
       -A, --show-all
              equivalent to -vET

       -b, --number-nonblank
              number nonempty output lines, overrides -n

       -e     equivalent to -vE

       -E, --show-ends
              display $ at end of each line

       -n, --number
              number all output lines

       -s, --squeeze-blank
              suppress repeated empty output lines

       -t     equivalent to -vT

       -T, --show-tabs
              display TAB characters as ^I

       -u     (ignored)

       -v, --show-nonprinting
              use ^ and M- notation, except for LFD and TAB

       --help display this help and exit

       --version
              output version information and exit

I'll need to handle command-line arguments - use the Clara library.
Reading and printing the contents of a file, line by line, can be handled
by the stdlib iostreams.

Simplest case - it'd be enough to read the entire file in one go and also print
it in one go.

Any of the following options: -b, --number-nonblank; -E, --show-ends;
-n, --number; -s, --squeze-blank would require reading line-by-line

Any of the following options: -A, --show-all; -e; -t; -T, --show-tabs
-v, --show-nonprinting would require reading individual characters.

Go with reading char-by-char for generality.

Not sure about carriage return. Need to open in binary? For now stick with
text input.

Try to implement options in the following order:
0. -h, --help
1. -n, --number
2. -b, --number-nonblank
3. -E, --show-ends
4. -s, --squeeze-blank
5. -T, --show-tabs
6. -v, --show-nonprinting
6. -A, --show-all
