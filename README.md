# My Shell
### Devs
Casey Rock - rockc15
James Villemarette - jvillemare
---

## Implemented Built-in Functions

 - `exit`: Leave the shell.
 - `which [filename]...`: Find the first instance of a command.
 - `where [filename]...`: Find all instances of a command.
 - `cd [directory]`: Change into a directory
 - `cd -`: Change back into the last directory
 - `pwd`: Print the current working directory.
 - `list`: List all the files in the current working directory.
 - `list [directory]`: List all the files in the directory specified.
 - `pid`: Print the process id of the shell.
 - `kill [integer]`: Send a SIGTERM to process with id of [integer].
 - `prompt [string]`: Change the beginning of the prompt of the shell.
 - `printenv`: Prints all environmental variables.
 - `printenv [string]`: Print the specified environmental variable.
 - `setenv [string] [string]`: Update/set a new enviornmental variable.

## Howto Glob

Run a while loop through the arguments array. While the glob does not
give an exit zero status, then concatenate both the arguments array and
the results from the glob function into one array. That array is then
passed into execvc. 

## System Calls 

`fgets(char * s, int size, FILE * stream)`
reads in at most one less than size characters from stream and stores 
them into the buffer pointed to by s. Reading stops after an
EOF or a newline.  If a newline is read, it is stored into the buffer.
A terminating null byte ('\0') is stored after the  last  character  in
the buffer.

`strtok(char * str, const *delim)`
function breaks a string into a sequence of zero or more
nonempty tokens.  On the first call  to  strtok(),  the  string  to  be
parsed should be specified in str.  In each subsequent call that should
parse the same string, str must be NULL.

`getcwd(char * buf, size_t size)`
function copies an absolute pathname of the current
working directory to the array pointed to by buf, which is of length
size.

`sprintf(char *str, const char *string,...); `
“String print”. Instead of printing on console, it store output on char buffer which are specified in sprintf

`DIR *opendir(const char *name);`
function opens a directory stream corresponding to the
directory name, and returns a pointer to the directory stream.  The
stream is positioned at the first entry in the directory.

`struct dirent *readdir(DIR *dirp);`
function returns a pointer to a dirent structure
representing the next directory entry in the directory stream pointed
to by dirp.  It returns NULL on reaching the end of the directory
stream or if an error occurred.

```
struct dirent {
   ino_t          d_ino;       /* Inode number */
   off_t          d_off;       /* Not an offset; see below */
   unsigned short d_reclen;    /* Length of this record */
   unsigned char  d_type;      /* Type of file; not supported
                                  by all filesystem types */
   char           d_name[256]; /* Null-terminated filename */
};
```

`pid_t getpid(void);`
returns the process ID (PID) of the calling process.  (This
is often used by routines that generate unique temporary filenames.)

`int closedir(DIR *dirp);`
function closes the directory stream associated with dirp. A successful call to closedir() also closes the underlying file descriptor associated with dirp. The directory stream descriptor dirp is not available after this call.

- [x] Exiting
- [x] Built in functions
- [x] External Functions
- [x] Capturing siginals 
- [x] Wildcards
- [x] flags
