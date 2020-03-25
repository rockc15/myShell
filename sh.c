
#include "sh.h"
#define MAXLINE 128



int sh( int argc, char **argv, char **envp ) {
    char *prompt = calloc(PROMPTMAX, sizeof(char));
    char *commandline = calloc(MAX_CANON, sizeof(char));
    char **args = calloc(MAXARGS, sizeof(char*));
    char * token, *pwd, *owd;
    char * prev = getcwd(NULL, PATH_MAX+1);
    int uid, status, go =1;
    struct passwd *password_entry;
    char *homedir;
    struct pathelement *pathlist;
    pid_t pid; 
    

    // char *command, *arg, *commandpath, *p, *pwd, *owd;
    // int uid, i, status, argsct, go = 1;


    uid = getuid();
    password_entry = getpwuid(uid);         /* get passwd info */
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


    strcpy(prompt, "myShell$ ");

    
    while ( go ) {
        /* print your prompt */
        printf("%s", prompt);

        /* get command line and process */
        fgets(commandline, MAXLINE, stdin);
        
        /* removes the newline char from cmdBuffer */ 
        int len = strlen(commandline);
        commandline[len - 1] = '\0';


        /* Tokenizes the cmdBuffer*/
        token = strtok(commandline, " ");

        /* reads the token into args */
        int argsIndex = 0;
        while(token != NULL){
            args[argsIndex] = token;
            argsIndex++;
            token = strtok(NULL, " ");
        }
        args[argsIndex] = NULL;
        
    

        /* check for each built-in in command and implement */
        if(strcmp("exit",args[0]) == 0){
            printf("exiting....\n");
            go = 0;
        }else if(strcmp("which",args[0]) == 0){
            which(args[1], pathlist);
        }else if(strcmp("where", args[0]) == 0){
            where(args[1], pathlist);
        }else if(strcmp("ls", args[0]) == 0){
            list(args);
        }else if(strcmp("pwd", args[0]) == 0){
            printWorkingDir();
        }else if(strcmp("pid", args[0]) == 0){
            printPid();
        }else if(strcmp("cd", args[0]) == 0){ 
            changeDir(args, prev);
        }else if(strcmp("kill", args[0]) == 0){
            killProcess(args);
        }else if(strcmp("prompt", args[0]) == 0){
            changePrompt(prompt, args[1]);
        }else if(strcmp("printenv", args[0]) == 0){
            printEnv(envp, args);
        }else if(strcmp("setenv", args[0]) == 0){
            setEnv(envp, args);
        }else{

            if ((pid = fork()) < 0) {
			    printf("fork error\n");
			    exit(1);
            }else if(pid == 0){
                execve(commandline, args, envp);
                printf("couldn't execute: %s \n", commandline);
                exit(127);
                
            }

            if ((pid = waitpid(pid, &status, 0)) < 0)
			    printf("waitpid error\n");


        }        
    }

    //doent free if you are using 1
    for(int j = 1; args[j] != NULL; j++)
        free(args[j]);

    //need to fix
    while(pathlist->next){
        free(pathlist->element);
        pathlist = pathlist->next;
    }   
    
    free(args);
    
    free(prompt);

    free(commandline);

    free(pwd);

    free(owd);

  return 0;
} 


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
    free(pwd);
}

/* Prints the process id of the shell */
void printPid(){
    printf("%d \n", getpid());
}

void changeDir(char **args, char * prev){   
    char tmpPrev[PATH_MAX];
    char * cwd= getcwd(NULL, PATH_MAX+1);

    if(args[1] == NULL){
        strcpy(prev, cwd);
        chdir(getenv("HOME"));
        return;
    }else if(strcmp(args[1], "-") == 0) {
        strcpy(tmpPrev, prev);
        strcpy(prev, cwd);

        if(chdir(tmpPrev) != 0)
            printf("dosnt not work \n");
          
    }else{
        strcpy(prev, cwd);
        chdir(args[1]);   
    }
    free(cwd);
}

void killProcess(char ** args){
    char * dash;
    char sig[100];

    //no args
    if(!args[1]){
        printf("Add a process to kill \n");
        return;
    }

    //killing with flags and pid
    dash = strstr(args[1], "-");
    if(strcmp(args[1], dash) == 0){
        printf("ypoooooo");
        int i = 1;
        while(args[1][i]){
            strncat(sig, &args[1][i], 1);
            i++;
        }

        if(!sig){
            if(!kill(atoi(args[2]), atoi(sig)))
                printf("your process was not killed \n");
        }else{
            printf("Add a process to kill \n");
        }
        return;
    }
    
    //killing with just pid
    if(!kill(atoi(args[1]), SIGTERM))
        printf("your process was not killed \n");
}


void changePrompt(char * pro, char * args1){
    char promptBuf[MAXLINE];
    strcpy(pro, " myShell$ ");

    if(!args1){
        printf("  input new prompt> ");
        fgets(promptBuf, MAXLINE, stdin);
        int len = strlen(promptBuf);
        promptBuf[len - 1] = ' ';
        strcat(promptBuf, pro);
        strcpy(pro, promptBuf);
    }else{
        strcat(args1, pro);
        strcpy(pro, args1);
    }
}


void printEnv(char **envp, char ** args){
    if(!args[1]){
        for(int i = 0; envp[i] != NULL; i++)
            printf("%s\n",envp[i]);
    }else{
            if(!args[2])
                printf("%s \n", getenv(args[1]));
            else
                printf("printenv: to many arguments \n");
    }   
}


//fixme
void setEnv(char **envp, char **args){
    char * enbuf = malloc(20);
    if(!args[1]){
        printf("here1");
        for(int i = 0; envp[i] != NULL; i++)
            printf("%s\n",envp[i]);

    }else if(args[3]){

        printf("You have to many args");

    }else if(args[1] && args[2]){
        strcpy(enbuf, args[1]);

        setenv(enbuf, args[2], 1);

        for(int i = 0; envp[i] != NULL; i++)
            printf("%s\n",envp[i]);

        

        
    }
    
}


