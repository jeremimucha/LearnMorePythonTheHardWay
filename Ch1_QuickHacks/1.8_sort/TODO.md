# sort -- research, feature list, TODO list

## Research

sort is a comand line tool which sorts given input and prints it out to stdout.
Sorting can be modified by some criteria based on command line options, e.g.
reverse order, ignore case, etc.

syntax:
sort [OPTION]... [FILE]...


## Ideas
* Most options should be rather easily implemented using STL algorithms
* It should be possible to handle the most basic input using istream_iterators
* Again, for multiple files - concurrency could be used. Could get cleaver with
sorting each file separately, than splicing the results, maybe (leave for 2nd run).
* Most options can be divided into those affecting the predicate and those
affecting either input or output. Try handle the predicates somewhat genericly?

## Features

1. Sort input read from stdin, print to stdout
2. Sort input specified by [FILE]... print to -o --output=FILE
3. -f, --ignore-case
4. -r, --reverse
5. -b, --ignore-leading-blanks
6. -d, --dictionary-order
8. -R, --random-sort
9. -c, --check - check for sorted input, do not sort, report first non-sorted line
10. 
