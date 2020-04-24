#include "sh.h"
#define MAXLINE 128
extern char **environ;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 
struct utmpx * up;

/**
 * start a shell.
 *
 * @param   argc    number of command line arguments
 * @param   argv    command line arguments
 * @param   envp    environmental variables
 *
 * @return  status of shell. 0 is fine. all others are errors.
 */
int sh( int argc, char **argv, char **envp ) {
    char *prompt = calloc(PROMPTMAX, sizeof(char));
    char **args = calloc(MAXARGS, sizeof(char*));
    char * token, *pwd, *owd;
    char * prev = getcwd(NULL, PATH_MAX+1);
    int uid, status, go =1;
    struct passwd *password_entry;
    char *homedir;
    struct pathelement *pathlist;
    pid_t pid; 
    glob_t paths;
    char ** globArray = calloc(MAXARGS, sizeof(args)+sizeof(paths.gl_pathv));
    int background = 0;

   


    uid = getuid();
    password_entry = getpwuid(uid);    

  
    homedir = password_entry->pw_dir;		
        
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
        char *commandline = calloc(MAX_CANON, sizeof(char));
        /* print your prompt */
        char * workingDir = getcwd(NULL, PATH_MAX+1);
        printf("%s[%s]> ", prompt, workingDir);        

        free(workingDir);
        /* get command line and process */
        fgets(commandline, MAXLINE, stdin);
        
        if( strcmp(commandline, "\0") == 0 ){
            printf("Use exit to leave the shell \n");
        }else if(strcmp(commandline, "\n") != 0 ){

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
                printf("Executing built-in %s \n", args[0]);
                which(args[1], pathlist);
            }else if(strcmp("where", args[0]) == 0){
                printf("Executing built-in %s \n", args[0]);
                where(args[1], pathlist);
            }else if(strcmp("list", args[0]) == 0){
                printf("Executing built-in %s \n", args[0]);
                list(args);
            }else if(strcmp("pwd", args[0]) == 0){
                printf("Executing built-in %s \n", args[0]);
                printWorkingDir();
            }else if(strcmp("pid", args[0]) == 0){
                printf("Executing built-in %s \n", args[0]);
                printPid();
            }else if(strcmp("cd", args[0]) == 0){ 
                printf("Executing built-in %s \n", args[0]);
                changeDir(args, prev);
            }else if(strcmp("kill", args[0]) == 0){
                printf("Executing built-in %s \n", args[0]);
                killProcess(args);
            }else if(strcmp("prompt", args[0]) == 0){
                printf("Executing built-in %s \n", args[0]);
                changePrompt(prompt, args[1]);
            }else if(strcmp("printenv", args[0]) == 0){
                printf("Executing built-in %s \n", args[0]);
                printEnv(args);
            }else if(strcmp("setenv", args[0]) == 0){
                printf("Executing built-in %s \n", args[0]);
                setEnv(args);
            }else if (strcmp("watchuser", args[0]) == 0){
                 printf("Executing built-in %s \n", args[0]);
                 watchUser(args);
            } else{

                //determines if there is background process
                if(strcmp("&", args[argsIndex-1]) == 0){
                    printf("adding to the backgorund \n");
                    args[argsIndex-1] = NULL;
                    background = 1;
                }else{
                    background = 0;
                }

                if ((pid = fork()) < 0) {
                    perror("Fork Error: ");
                    exit(1);
                }else if(pid == 0){
                    //check for absolute path
                    if(commandline[0] == '/' || commandline[0] =='.'){
                        if (access(commandline, X_OK ) == 0){
                            execve(commandline, args, envp);
                            perror("Could Not Execute: ");
                            exit(127);
                        }else {
                            perror("Could Not Execute: ");
                            exit(127);
                        }
                    } else {
                        char cmd[128];
                        while( pathlist ){
                            sprintf(cmd, "%s/%s", pathlist->element, commandline);
                            if(access(cmd, X_OK) == 0){
                                /* Handles Wildcards */
                                int globStatus = -1;
                                for(int i = 1; args[i] != NULL; i++){
                                    globStatus = glob(args[i], 0 , NULL, &paths);
                                
                                   if(globStatus == 0){
                                        int globI = 0;
                                        for(int j = 0; args[j] != NULL; j++){
                                            globArray[j] = args[j];
                                            globI++;
                                        }
                                        globI--;
                                        for( int i = 0 ; paths.gl_pathv[i] != NULL; i++){
                                            globArray[globI] = paths.gl_pathv[i];
                                            globI++;
                                        }
                                        globArray[globI] = NULL;
                                        
                                        execve(cmd, globArray, envp);
                                        perror("Could Not Execute: ");
                                        exit(127);
                                    }

                                }

                                printf("Executing %s \n", cmd);
                                execve(cmd, args, envp);
                                perror("Could Not Execute: ");
                                exit(127);
                                break;
                            }
                            pathlist = pathlist->next;
                        }
                        exit(127);
                    }
                }

                if(background){ //if background is true the parent won't wait for the child 
                    printf("+[%d]\n", pid);
                }else if ((pid = waitpid(pid, &status, 0)) < 0){
                    perror("Waitpid Error: ");
                }


            }        
        }
        free(commandline);
    }

    for(int j = 1; args[j] != NULL; j++)
        free(args[j]);

    for(int i = 0; globArray[i] != NULL; i++){
        free(globArray[i]);
    }

    free(globArray);

    while(pathlist){
        struct pathelement * temp = pathlist;
        pathlist = pathlist->next;
        free(temp);
    }

    free(pathlist);
    
    globfree(&paths);

    free(args);
    
    free(prompt);

    free(pwd);

    free(owd);

    free(prev);

  return 0;
} 


/** loop through pathlist until finding command and return it.
 *
 * @param   command     what command is being searched for.
 * @param   pathlist    used to find environmental path.
 *
 * @return  NULL when not found.
 */
void which(char *command, struct pathelement *pathlist ) {
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


/**
 * loop through pathlist until finding all commands and return it.
 *
 * @param   command     what command is being searched for.
 * @param   pathlist    used to find environmental path.
 *
 * @return  NULL when not found.
 */
void where(char *command, struct pathelement *pathlist ) {
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


/**
 * list all the files in a given directory.
 *
 * @param   dir     directory being listed.
 */
void list ( char **dir ) {
    DIR *directory;
    struct dirent * dirInfo;

    if(dir[1] == NULL){
        directory = opendir(".");
    } else {

    
       if((directory = opendir(dir[1])) == NULL){
            perror("Could Not Open: ");
          return;
       }
    }

    while((dirInfo = readdir(directory)) != NULL){
        printf("%s \n", dirInfo -> d_name);
    }

    closedir(directory);
} 

/**
 * print the current working directory.
 */ 
void printWorkingDir() {
    char * pwd = getcwd(NULL, PATH_MAX+1);
    printf("%s \n", pwd);
    free(pwd);
}

/**
 * prints the process id of the shell.
 */
void printPid(){
    printf("%d \n", getpid());
}

/**
 * change into a directory.
 *
 * @param   args    full command line (ex: `cd ...`).
 * @param   prev    last directory was in. 
 */
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
            perror("Could Not change Dir: ");
          
    }else{
        strcpy(prev, cwd);
        chdir(args[1]);   
    }
    
    free(cwd);
}

/**
 * kills a processes.
 *
 * @param   args    full command line (ex: `kill ...`).
 */
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
        int i = 1;
        while(args[1][i]){
            strncat(sig, &args[1][i], 1);
            i++;
        }

        if(!sig){
            if(!kill(atoi(args[2]), atoi(sig)))
                perror("Could Not Kill Process: ");
        }else{
            printf("Add a process to kill \n");
        }
        return;
    }
    
    //killing with just pid
    if(!kill(atoi(args[1]), SIGTERM))
        perror("Process not killed: ");
}

/**
 * changes the command line prompt
 *
 * @param   pro     new prompt to be in front of default prompt.
 * @param   args    full command line (ex: `prompt ...`). 
 */
void changePrompt(char * pro, char * args1){
    char promptBuf[MAXLINE];

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

/**
 * prints the environment.
 *
 * @param   args    full command line (ex: `printenv ...`).
 */
void printEnv(char ** args){
    if(!args[1]){
        for(int i = 0; environ[i] != NULL; i++)
            printf("%s\n",environ[i]);
    }else{
            if(!args[2])
                printf("%s \n", getenv(args[1]));
            else
                printf("printenv: to many arguments \n");
    }   
}

/**
 * set a update/new environmental variable
 *
 * @param   args    full command line (ex: `setnev ...`).
 */
void setEnv(char **args){
    char * enbuf = malloc(20);
    if(!args[1]){
        for(int i = 0; environ[i] != NULL; i++)
            printf("%s\n",environ[i]);
    }else if(args[3]){
        printf("You have to many args");
    }else if(args[1] && args[2]){
        strcpy(enbuf, args[1]);
        setenv(enbuf, args[2], 1);

        for(int i = 0; environ[i] != NULL; i++)
            printf("%s\n",environ[i]);
    }else {
        strcpy(enbuf, args[1]);
        setenv(enbuf, args[2], 1);
    }
    free(enbuf);
}


void watchUser(char ** args){
  setutxent();			/* start at beginning */
  while (up = getutxent()){	/* get an entry */
    if ( up->ut_type == USER_PROCESS )	/* only care about users */
    {
      printf("%s has logged on %s from %s\n", up->ut_user, up->ut_line, up ->ut_host);
    }
  }

}

