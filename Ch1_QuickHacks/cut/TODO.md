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


## Ideas

* Use the regex library - regex_token_iterator to get the resulting tokens from
each line.
* Copy (move) the tokens to a container to process them later.
This should make it easy to implement all the possible ranges (N, N-, N-M, -M)
* Should probably define a class to encapsulate the result.
* Implement a class to encapsulate the processing? Or is a function enough?
* Template would probably make it easy to implement all of the 
-b, -c and -f options.
