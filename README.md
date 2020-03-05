### My Shell
---

## Syntax for the shell 

`fgets(char * s, int size, FILE * stream)`

    reads in at most one less than size characters from stream and stores them into the buffer pointed to by s.  Reading  stops  after  an
    EOF  or a newline.  If a newline is read, it is stored into the buffer.
    A terminating null byte ('\0') is stored after the  last  character  in
    the buffer.

`strtok(char * str , const *delim)`

    function breaks a string into a sequence of zero or more
    nonempty tokens.  On the first call  to  strtok(),  the  string  to  be
    parsed should be specified in str.  In each subsequent call that should
    parse the same string, str must be NULL.
