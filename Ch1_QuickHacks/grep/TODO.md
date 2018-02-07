# grep -- research + TODO

The grep utility searches the given input files for lines containing the pattern (regex).
syntax:
    grep [OPTIONS] PATTERN [FILE...]

We're trying to read through the listed files line by line and find all the lines that match the given regex.


## Ideas

* I should probably write a class to encapsulate the result of searching through the file. It should contain the file path and a container of line-number/line pairs that match the given pattern.
* Once that's done it's just a matter of opening each file and reading it line-by-line with getline().
* I could try using std::async to process the files in parallel. This could be done quite simply in one function returning a container of the results.


## TODO

1. Get the command line arguments to work
    * Don't bother with options for now
    * One positional string argument that contains the regex pattern
    * One positional vector\<string\> pattern that contains all the files to search through.
2. Use stdlib for handling the files.
3. Define the result-class that will contain a file name string and a container of line-number/line pairs.
4. Define a function that reads a file line-by-line and returns the resulting structure
5. Define a function that goes through the file names container and returns a container of all the results.
6. Define a function that prints the results to stdout.


## Done
First try took about an hour. Pretty happy with the result actually.
The basic process of doing a little research, writing down general ideas and making a todo list works really well.

## The bad
* I'm still getting very distracted by the internet and just random things popping into my head.
    -> Turn off everything I don't need. Have a separate browser just for code-related stuff.
* Having trouble sticking to the TODO list, I'm jumping between the tasks.
    -> Try writing down more detailed todo lists, maybe?
* HORRIBLE sitting position. I need to discipline myself somehow.
    -> ???

## The good
* VSCode 'zen mode' (fullscreen) helps with avoiding distractions.
* Music with no vocals or no music at all, in a quiet environment, works well.
* Hack/spike project stub works really well, no adjustments needed this time.
