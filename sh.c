#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <linux/limits.h>
#include "sh.h"
#define MAXLINE 128

int sh( int argc, char **argv, char **envp ) {
    char *prompt = calloc(PROMPTMAX, sizeof(char));
    char *commandline = calloc(MAX_CANON, sizeof(char));
    char *command, *arg, *commandpath, *p, *pwd, *owd;
    char **args = calloc(MAXARGS, sizeof(char*));
    int uid, i, status, argsct, go = 1;
    struct passwd *password_entry;
    char *homedir;
    struct pathelement *pathlist;
    char * token;
   

    char cmdBuffer[MAXLINE];

    uid = getuid();
    password_entry = getpwuid(uid);               /* get passwd info */
    homedir = password_entry->pw_dir;		/* Home directory to start
                            out with*/
        
    if ( (pwd = getcwd(NULL, PATH_MAX+1)) == NULL ){
        perror("getcwd");
        exit(2);
    }
    owd = calloc(strlen(pwd) + 1, sizeof(char));
    memcpy(owd, pwd, strlen(pwd));
    prompt[0] = ' '; prompt[1] = '\0';

    /* Put PATH into a linked list */
    pathlist = get_path();

    
    while ( go ) {
        /* print your prompt */
        printf("%s> ", pwd);

        /* get command line and process */
        fgets(cmdBuffer, MAXLINE, stdin);
        
        /* removes the newline char from cmdBuffer */ 
        int len = strlen(cmdBuffer);
        cmdBuffer[len - 1] = '\0';


        /* Tokenizes the cmdBuffer*/
        token = strtok(cmdBuffer, " ");

        /* reads the token into args */
        int argsIndex = 0;
        while(token != NULL){
            args[argsIndex] = token;
            argsIndex++;
            token = strtok(NULL, " ");
        }

        /* check for each builiuft in command and implement */
        if(strcmp("exit",args[0]) == 0){
            printf("exiting....\n");
            go = 0;
        }else if(strcmp("which",args[0]) == 0){
            which(args[1], pathlist);
        }else if(strcmp("where", args[0]) == 0){
            where(args[1], pathlist);
        } else if(strcmp("ls", args[0]) == 0){
            list(args[1]);
        }
    


        /*  else  program to exec */
        //{
        /* find it */
        /* do fork(), execve() and waitpid() */
        // if(1){

        // } else {
        //     fprintf(stderr, "%s: Command not found.\n", args[0]);
        // }
    }
  return 0;
} /* sh() */

char *which(char *command, struct pathelement *pathlist ) {
    if(command == NULL){
        printf("add a command to which \n");
    }else{
        /* loop through pathlist until finding command and return it.  Return
        NULL when not found. */
        printf("you are in which: %s \n",command);
    }

} /* which() */

char *where(char *command, struct pathelement *pathlist ) {
    if(command == 0){
        printf("Add a comment to where \n");
    } else {
        /* similarly loop through finding all locations of command */
        /* where() */
         printf("you are in where: %s \n",command);
    }
} 

void list ( char *dir ) {
  /* see man page for opendir() and readdir() and print out filenames for
  the directory passed */
} /* list() */