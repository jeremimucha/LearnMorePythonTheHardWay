# Hexdump

## 'research'
tr - translate od delete characters. Given two sets (or a single set if the '-d' option is given) it translates all characters from SET1 to characters in SET2

Syntax: tr [OPTION]... SET1 [SET2]


## Ideas, observations

* Seems like the characters are mapped 1-to-1 by index, i.e. if SET1={A,B,C} and SET2={x,y,z}, 'ABSENCE' would become 'xySENzE'.
* If SET1 is longer than SET2 all the characters in SET1 with indexes higher than the lest index of SET2 map to the last character of SET2
* Options that REQUIRE a single set: -d
* Can be used with single or two sets: -c, -s
* Requires two sets: -t
* Could regex be used to make this easy?
* Read input line by line? If I do I'd need to consider the new-line character
* It might actually be easier to read char-by-char and filter the characters appropriately? Could maybe be done with istream_iterator, ostream_iterator, copy with an appropriate predicate? 
* istreambuf_iterator maybe?


## Feature list
    -c, -C, --complement - use the complement of SET1 // should be simple "if not"

    -d, --delete delete characters in SET1, do not translate // should also be simple

    -s, --squeeze-repeats
            replace  each sequence of a repeated character that is listed in the last specified SET, with a single occurrence of that character

    -t, --truncate-set1
            first truncate SET1 to length of SET2


    SETs are specified as strings of characters.  Most represent themselves.  Interpreted sequences are:

    \NNN   character with octal value NNN (1 to 3 octal digits)

    \\     backslash

    \n     new line

    \r     return

    \t     horizontal tab

    CHAR1-CHAR2
            all characters from CHAR1 to CHAR2 in ascending order

    [CHAR*]
            in SET2, copies of CHAR until length of SET1

    [CHAR*REPEAT]
            REPEAT copies of CHAR, REPEAT octal if starting with 0

    [:alnum:]
            all letters and digits

    [:alpha:]
            all letters

    [:blank:]
            all horizontal whitespace

    [:cntrl:]
            all control characters


## TODO:
* Don't bother with the interpreted set sequences - just go with simple 1:1 mapping in the beginning.
* Try different approaches.
    * Read char-by-char.
    * Try the iterator approach
    * Try the line-by-line approach and just assume the new line char (probably bad)?
* Implement the options
* Do some of the interpreted sequences only if the first part goes quick.
