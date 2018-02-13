# cut -- research + TODO

## research

cut OPTION... [FILE]...

cut - prints selected parts of a line from each FILE to stdout.
Options to consider:
    -d, --delimiter  - specify a delimiter to use, default = \t
    -f, --fields     - specify which fields, delimited by -d, to output
    -b, --bytes      - select only these bytes
    -c, --characters - select only these characters
-d and -f is enough to start with.
-b, -c, -f take a 'LIST' which is
    N       Nth byte/char/field, counting from 1
    N-      starting from Nth byte/char/field
    N-M     from Nth to Mth (including) byte/char/field
    -M      from first to Mth (including)  byte/char/field

Usage:

>$ ls -l
>total 5
>drwxr-xr-x 1 Jeremi None   0 Feb 12 21:23 Ch1_QuickHacks
>drwxr-xr-x 1 Jeremi None   0 Jan 31 21:07 external
>-rw-r--r-- 1 Jeremi None 156 Jan 29 20:54 README.md


>$ ls -l | **cut -d ' ' -f 3-7**
>
>Jeremi None   0
>Jeremi None   0
>Jeremi None 156 Jan 29

>$ ls -l | **cut -d ' ' -f 3-**
>
>Jeremi None   0 Feb 12 21:23 Ch1_QuickHacks
>Jeremi None   0 Jan 31 21:07 external
>Jeremi None 156 Jan 29 20:54 README.md

>$ ls -l | **cut -d ' ' -f -7**
>total 5
>drwxr-xr-x 1 Jeremi None   0
>drwxr-xr-x 1 Jeremi None   0
>-rw-r--r-- 1 Jeremi None 156 Jan 29

>$ ls -l | **cut -d ' ' -f 8**
>
>Feb
>Jan
>20:54


Given multiple FILES to process there's no indication in the output where one
file ends and another begins.


## Ideas

* Use the regex library - regex_token_iterator to get the resulting tokens from
each line.
* Copy (move) the tokens to a container to process them later.
This should make it easy to implement all the possible ranges (N, N-, N-M, -M)
* Should probably define a class to encapsulate the result.
* Implement a class to encapsulate the processing? Or is a function enough?
* Template would probably make it easy to implement all of the 
-b, -c and -f options.


## TODO

* implement the necessary options:
    -d, --delimiter
    -f, --fields N-M range - this will need to be parsed and turned into
    a pair of two ints.
    Can use a regex here aswell and std::stoi to convert to int
* Define a class to hold the result, can just be a typedef for now for vector\<string\>
* Define a aclass to hold the complete result set. This can also be just a define
for now - list\<vector\<string\>\>;
* Define a template to process a single line and return the resulting fields
* Define a template that opens a file and processes all the lines, it should return
a list\<vector\<string\>\>
* Define a template that processes all of the given files (concurrently?)


## Done
Not too happy this time. I've had a lot of trouble with the regex library and
attempts to parse the fields range arguments properly before falling back
on what it should be - an ugly hack. I also unnecessarily tried using wide strings
right away, which also gave me some trouble.
Once again - I need to stick to the simplest possible solutions the first time.
Then later I should come back and improve on it.

Caught myself a few times in very bad sitting positions. Ideally I'd like a
standing setup, but for now I need to force myself to sit straight and relaxed
somehow.

Had trouble focusing this time. Probably had a hard because I hadn't written
anything in few days.

Didn't get distracter by the internet at least.
