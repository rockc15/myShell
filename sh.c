
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
    char * prev = getcwd(NULL, PATH_MAX+1);

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
        printf("cashelll>");

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
        args[argsIndex] = NULL;
        
    

        /* check for each builiuft in command and implement */
        if(strcmp("exit",args[0]) == 0){
            printf("exiting....\n");
            go = 0;
        }else if(strcmp("which",args[0]) == 0){
            which(args[1], pathlist);
        }else if(strcmp("where", args[0]) == 0){
            where(args[1], pathlist);
        } else if(strcmp("ls", args[0]) == 0){
            list(args);
        } else if(strcmp("pwd", args[0]) == 0){
            printWorkingDir();
        } else if(strcmp("pid", args[0]) == 0){
            printPid();
        }else if(strcmp("cd", args[0]) == 0){ 
            changeDir(args, prev);
        }
    

// prev = getcwd(NULL, PATH_MAX+1);
        
    

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


/* loop through pathlist until finding command and return it.  Return
NULL when not found. */
char *which(char *command, struct pathelement *pathlist ) {
    char cmd[64];
    if(command == NULL){
        printf("Too few arguments \n");
    }else{
        while(pathlist != NULL){
           sprintf(cmd, "%s/%s", pathlist->element, command);
            if(access(cmd, X_OK)){
                printf("[%s]\n", cmd);
                break;
            }
             pathlist= pathlist->next;
        }
    }

}


/* loop through pathlist until finding all commands and return it.  Return
NULL when not found. */
char *where(char *command, struct pathelement *pathlist ) {
    char cmd[64];

    if(command == 0){
        printf("Too few arguments \n");
    } else {
        while(pathlist){
            sprintf(cmd, "%s/%s", pathlist->element, command);
            if(access(cmd, X_OK)){
                printf("[%s]\n", cmd);
            }
             pathlist= pathlist->next;
        }
    }
} 


/* fiegure out how to list specifc files names */
void list ( char **dir ) {
    DIR *directory;
    struct dirent * dirInfo;

    if(dir[1] == NULL){
        directory = opendir(".");
    } else {


        
        /* possible iterate and only print the file name */ 
       if((directory = opendir(dir[1])) == NULL){
          printf("can't open %s \n", dir[1]);
          return;
       }
    }

    while((dirInfo = readdir(directory)) != NULL){
        printf("%s \n", dirInfo -> d_name);
    }

    closedir(directory);
} 

/* Print the current working directory */ 
void printWorkingDir() {
    char * pwd = getcwd(NULL, PATH_MAX+1);
    printf("%s \n", pwd);
}

/* Prints the process id of the shell */
void printPid(){
    printf("%d \n", getpid());
}

void changeDir(char **args, char * prev){   
    char * tmpPrev;

    if(args[1] == NULL){
        strcpy(prev, getcwd(NULL, PATH_MAX+1));
        chdir(getenv("HOME"));
        return;
    }else if(strcmp(args[1], "-") == 0) {
    
        strcpy(tmpPrev, prev);
        strcpy(prev, getcwd(NULL, PATH_MAX+1));

        if(chdir(tmpPrev) != 0)
            printf("dosnt not work \n");
          
    }else{
        strcpy(prev, getcwd(NULL, PATH_MAX+1));
        chdir(args[1]);   
    }
}



