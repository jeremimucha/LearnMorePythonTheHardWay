# sh

## 'research'
sh is a command language interpreter that executes commands read from stdin or from a file.


## Ideas, observations
* use boost::process to launch other programs
* use STL to process input/output
* There should be no need for launching subprocesses asynchronously.
* I'll need to redirect stdin into the launched program and redirect stdout of the launched program into the 'Sh' process.
* 'Parsing' the input should be rather simple, in the most basic case - the first 'word' is the program to launch, everything else, up to a new line are parameters.
* Next steps would be to consider special characters like ; and escapes like \
* going further I could split up params into separate strings. This would require additionally handling '' and ""
* Could also include some basic build-in commands like 'exit'


## TODO:

* implement a very basic parser, tokenize input
* write a simple wrapper class for the boost::process::child class, to make sure the process always properly closes and reports any potential errors on scope exit
