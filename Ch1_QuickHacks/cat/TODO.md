# cat TODO list

1. Create a project:
    - single main.cpp source file
    - manage build with cmake -> write CMakeLists.txt files
    - third-party->clara subdirectories

2. Include all the necessary headers
3. use the clara library to handle cmd-line args.
4. Start with just -h, --help and positional arguments - files to cat
5. Implement a function that takes an open ifstream and writes it out to stdout
6. Implement a function that takes a vector of file names, opens each in turn and calls the output function
7. see if it all works
8. Implement the -n, --number option - number all lines
9. Implement the -b, --number-nonblank option - number non-blank lines (overrides -n)
10. -E, --show-ends - print '$' at end of each line
11. -s, --squeeze-blank - suppress repeated empty output lines
12. -T, --show-tabs - display \t as ^I
13. -v, --show-nonprinting - ^M for \t, not sure about other chars? - go with '?' for now.
14. -A, --show-all == -vET
