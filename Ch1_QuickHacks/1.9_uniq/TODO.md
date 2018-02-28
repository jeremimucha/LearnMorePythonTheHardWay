# uniq -- research, feature list, TODO list

## Research

uniq is a command line tool which takes sorted lines as input and outputs
only uniqe lines.

syntax:
uniq [OPTION]... [INPUT [OUTPUT]]


## Ideas
* Seems rather similiar to sort in how code could be structured
* Represent the lines as pairs of (line,count) - will make the rest of the processing
easier. Push back next line only if it's different, otherwise increment the count.
* This will require determining the predicate before reading the lines -- predicate
std::equal<pair<string,size_t>> or same with ignore_case applied



## Features

1. Take input from stdin print to stdout
2. Sort input specified by [INPUT] print to [OUTPUT]
3. -c, --count -- prefix lines by the number of occurences
4. -d, --repeated -- only print duplicate lines, one for each group
5. -D -- print ALL repeated lines
6. -i, --ignore-case -- ignore case difference when comparing
7. -u, --unique -- only print unique lines


## TODO
