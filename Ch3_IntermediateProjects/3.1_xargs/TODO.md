# Xargs

## 'research'
xargs - reads items from stdin and executed a command (by default echo) with
any initial-arguments followed by the items read from stdin.
* Items are delimited by blanks(?) or newlines
* Blank lines are ignored

syntax:
xargs [options] [command [initial-arguments]]

## Ideas

* use boost::process
* read stdin line-by line
* spawn a child process and feed it the arguments we just read

## Features

1. Initial-arguments make sense only if the command is given?
2. -a, --arg-file -> read items from a file
3. -t, --verbose  -> print the command line on stderr before executing it
