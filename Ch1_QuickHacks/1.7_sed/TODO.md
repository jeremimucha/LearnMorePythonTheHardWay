# sed -- research, feature list, TODO list

## Research

Sed is a command line stream editor for filtering and transforming text. It uses regular expressions to find, replace, edit, filter text input - read from a file. The most useful command is search and replace based on a regex pattern - s/regexp/replacement.

The most basic syntax is
sed -e 's/regexp/replacement/ FILES...

More complex usage allows for writing sed scripts and reading them from a file with the -f option, instead of specifying patterns on the command line.


## Ideas

1. Libraries to use:
    * clara - for command line options
    * regex - pattern replacement
    * filesystem (try experimental/filesystem instead of boost?),
    * ?
2. The most basic implementation, which writes to stdout shouldn't be too difficult - read a file line by line, use std::regex_replace to find the pattern and change it then write it to stdout.
3. Actually updating the files will be more challenging. Create a new file, write the mutated lines to it then rename the temp file and the original.


## Features

1. Take command line arguments -e --expression s/// FILES...
2. Find and replace word A with word B
3. Write output to stdout
4. Find and replace regex patterns RE with given word W
5. Make changes in place -i[SUFFIX] --in-place=[SUFFIX] with optional backup file if SUFFIX is given.


## TODO
1. Implement command line arguments
    * -e --expression s/pattern/replacement/
    * FILES... (positional)
    * Help
2. Try using istream/ostream iterators with regex_replace to do the find and replace.
3. Write a function which takes an istream, ostream and a regex pattern, reads the istream and writes the string after replacement to ostream
4. Write a function which processes all the files
5. If there's still time implement the in-place option:
    * -i[SUFFIX] --in-place[SUFFIX]
If set we should process all the files in parallel, making sure the temporary files are unique.
If suffix is given do the following
    * Open a temporary file for writing,
    * Read the inputfile, write the changed lines to the temp file
    * rename the inputfile to have filenameSUFFIX
    * rename the temp file to filename
