#include "sh.h"
#define MAXLINE 128
extern char **environ;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 

// watch user
pthread_t p;
int m;

// watch user: A linked list node 
struct node {
   char* key;
   struct node *next;
};

struct node *head = NULL;
struct node *current = NULL;

//display the list
void print_list() {
    struct node *ptr = head;
    printf("\n[ ");
	
    //start from the beginning
    while(ptr != NULL) {
        printf("(%s) ",ptr->key,ptr->key);
        ptr = ptr->next;
    }
	
    printf(" ]");
}

//insert link at the first location
void insert_first(char* key) {
    //create a link
    struct node *link = (struct node*) malloc(sizeof(struct node));
	
    link->key = key;
	
    //point it to old first node
    link->next = head;
	
    //point first to new first node
    head = link;
}

//delete first item
struct node* delete_first() {

    //save reference to first link
    struct node *tempLink = head;
	
    //mark next to first link as first 
    head = head->next;
	
    //return the deleted link
    return tempLink;
}

//is list empty
int is_empty() {
    return head == NULL;
}

int length() {
    int length = 0;
    struct node *current;
	
    for(current = head; current != NULL; current = current->next) {
        length++;
    }
	
   return length;
}

// find a link with given key
struct node* find(char* key) {

    // start from the first link
    struct node* current = head;

    // if list is empty
    if(head == NULL) {
        return NULL;
    }

    // navigate through list
    while(current->key != key) {
	
        // if it is last node
        if(current->next == NULL) {
            return NULL;
        } else {
            // go to next link
            current = current->next;
        }
    }      
	
    // if data found, return the current Link
    return current;
}

// delete a link with given key
struct node* delete(int key) {

    // start from the first link
    struct node* current = head;
    struct node* previous = NULL;
	
    // if list is empty
    if(head == NULL) {
        return NULL;
    }

    // navigate through list
    while(current->key != key) {

        // if it is last node
        if(current->next == NULL) {
            return NULL;
        } else {
            //store reference to current link
            previous = current;
            //move to next link
            current = current->next;
        }
    }

    //found a match, update the link
    if(current == head) {
        //change first to point to next link
        head = head->next;
    } else {
        //bypass the current link
        previous->next = current->next;
    }    
	
   return current;
}

/**
 * Watchuser: Thread function. 
 * 
 * @param   arg     first user to watch.
 */
void *user(void * arg) {
    //printf("%s \n", arg);
    struct utmpx * up;
    setutxent();
    int watchTime = 20;
    while(watchTime > 0) {
        pthread_mutex_lock(&lock); // prevent collision with watchUser function

        while ( up = getutxent() ) {	/* get an entry */
            if ( up->ut_type == USER_PROCESS ) {

                // search the linked list for it
                struct node* temp = head;
                while(temp != NULL) {
                    if(strcmp(up->ut_user, temp->key) == 0)
                        printf("%s has logged on %s from %s\n", up->ut_user, up->ut_line, up->ut_host);
                    temp = temp->next;
                }

            }
        }

        pthread_mutex_unlock(&lock); // prevent collision with watchUser function
        watchTime -= 3;
        sleep(3);
    }
}

/**
 * Start a shell.
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
            if(strcmp("exit",args[0]) == 0) {
                printf("exiting....\n");
                go = 0;
            } else if(strcmp("which",args[0]) == 0) {
                printf("Executing built-in %s \n", args[0]);
                which(args[1], pathlist);
            } else if(strcmp("where", args[0]) == 0) {
                printf("Executing built-in %s \n", args[0]);
                where(args[1], pathlist);
            } else if(strcmp("list", args[0]) == 0) {
                printf("Executing built-in %s \n", args[0]);
                list(args);
            } else if(strcmp("pwd", args[0]) == 0) {
                printf("Executing built-in %s \n", args[0]);
                printWorkingDir();
            } else if(strcmp("pid", args[0]) == 0) {
                printf("Executing built-in %s \n", args[0]);
                printPid();
            } else if(strcmp("cd", args[0]) == 0) { 
                printf("Executing built-in %s \n", args[0]);
                changeDir(args, prev);
            } else if(strcmp("kill", args[0]) == 0) {
                printf("Executing built-in %s \n", args[0]);
                killProcess(args);
            } else if(strcmp("prompt", args[0]) == 0) {
                printf("Executing built-in %s \n", args[0]);
                changePrompt(prompt, args[1]);
            } else if(strcmp("printenv", args[0]) == 0) {
                printf("Executing built-in %s \n", args[0]);
                printEnv(args);
            } else if(strcmp("setenv", args[0]) == 0) {
                printf("Executing built-in %s \n", args[0]);
                setEnv(args);
            } else if (strcmp("watchuser", args[0]) == 0) {
                 printf("Executing built-in %s \n", args[0]);
                 watchUser(args);
            } else {
                // determines if there is background process
                if(backGround(args)){
                    background = 1;
                }else{
                    background = 0;
                }

                if ((pid = fork()) < 0) {
                    perror("Fork Error: ");
                    exit(1);
                }else if(pid == 0){
                    int fd;
                    // determmines if there was a redirection 
                    int re = redirection(args);
                    if (0 <= re ){
                        
                        
                        if(re == 0) {
                            close(STDOUT_FILENO);
                            fd = open(args[argsIndex - 1], O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
                        } else if(re == 1){ 
                            close(STDOUT_FILENO);
                            fd = open(args[argsIndex - 1], O_CREAT|O_WRONLY|O_APPEND, S_IRWXU);
                        } else if(re == 2) {
                            close(STDIN_FILENO);
                            stdin = fopen(args[argsIndex - 1], "r");
                        }
                        
                    }

                    //check for absolute path
                    if (commandline[0] == '/' || commandline[0] == '.') {
                        if (access(commandline, X_OK ) == 0) {
                            execve(commandline, args, envp);
                            perror("Could Not Execute: ");
                            exit(127);
                        } else {
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

    // close watch user stuff
    pthread_cancel(p);
    pthread_join(p, (void** ) &m);
    if(!is_empty()) {
        struct node* prev = head;
        struct node* temp = head->next;

        while(temp != NULL) {
            free(prev);
            prev = temp;
            temp = temp->next;
        }
    }

    // free everything else
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


/** 
 * Loop through pathlist until finding command and return it.
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
 * Loop through pathlist until finding all commands and return it.
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
 * List all the files in a given directory.
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
 * Print the current working directory.
 */ 
void printWorkingDir() {
    char * pwd = getcwd(NULL, PATH_MAX+1);
    printf("%s \n", pwd);
    free(pwd);
}

/**
 * Prints the process id of the shell.
 */
void printPid(){
    printf("%d \n", getpid());
}

/**
 * Change into a directory.
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
 * Kills a process(es).
 *
 * @param   args    full command line (ex: `kill ...`).
 */
void killProcess(char ** args){
    char * dash;
    char sig[100];

    //no args
    if(!args[1]) {
        printf("Add a process to kill \n");
        return;
    }

    //killing with flags and pid
    dash = strstr(args[1], "-");
    if(strcmp(args[1], dash) == 0) {
        int i = 1;
        while(args[1][i]) {
            strncat(sig, &args[1][i], 1);
            i++;
        }

        if(!sig) {
            if(!kill(atoi(args[2]), atoi(sig)))
                perror("Could Not Kill Process: ");
        }else{
            printf("Add a process to kill \n");
        }
        return;
    }

    // killing with just pid
    if(!kill(atoi(args[1]), SIGTERM))
        perror("Process not killed: ");
}

/**
 * Changes the command line prompt.
 *
 * @param   pro     new prompt to be in front of default prompt.
 * @param   args    full command line (ex: `prompt ...`). 
 */
void changePrompt(char * pro, char * args1) {
    char promptBuf[MAXLINE];

    if(!args1) {
        printf("  input new prompt> ");
        fgets(promptBuf, MAXLINE, stdin);
        int len = strlen(promptBuf);
        promptBuf[len - 1] = ' ';
        strcat(promptBuf, pro);
        strcpy(pro, promptBuf);
    } else {
        strcat(args1, pro);
        strcpy(pro, args1);
    }
}

/**
 * Prints the environment.
 *
 * @param   args    full command line (ex: `printenv ...`).
 */
void printEnv(char ** args) {
    if(!args[1]) {
        for(int i = 0; environ[i] != NULL; i++)
            printf("%s\n",environ[i]);
    } else {
        if(!args[2])
            printf("%s \n", getenv(args[1]));
        else
            printf("printenv: to many arguments \n");
    }   
}

/**
 * Set a update/new environmental variable.
 *
 * @param   args    full command line (ex: `setnev ...`).
 */
void setEnv(char **args){
    char * enbuf = malloc(20);
    if(!args[1]) {
        for(int i = 0; environ[i] != NULL; i++)
            printf("%s\n",environ[i]);
    } else if(args[3]) {
        printf("You have to many args");
    } else if(args[1] && args[2]) {
        strcpy(enbuf, args[1]);
        setenv(enbuf, args[2], 1);

        for(int i = 0; environ[i] != NULL; i++)
            printf("%s\n",environ[i]);
    } else {
        strcpy(enbuf, args[1]);
        setenv(enbuf, args[2], 1);
    }
    free(enbuf);
}

/**
 *  Deterimes if there is a redirection symbol in the command line.
 * 
 *  @param   args    full command line (ex: `setnev ...`).
 */
int redirection(char ** args){
    for(int i = 0; args[i] != NULL; i++){
        //append output 
        if(strstr(args[i], ">>")){
            args[i] = NULL;
            return 1;
        }
        //override output 
        if(strstr(args[i], ">")){
            args[i] = NULL;
            return 0;
        }
        if(strstr(args[i], "<")){
            printf("yooo thsu su what tim tedt ign ");
            args[i] = NULL;
            return 2;
        }
    }

    return -1;
}

/**
 * Determine if a thread is going to the background or being redirected.
 * 
 * @param   args    command line args
 */
int backGround(char ** args){
    for(int i = 0; args[i] != NULL; i++){
        if(strstr(args[i], "&") && (strstr(args[i], "<") || strstr(args[i], ">"))){
            return 1;
        }
        if(strstr(args[i], "&")){
            args[i] = NULL;
            return 1;
        }
    }

    return 0;
}

/**
 * Watch a user logged in to the terminal.
 * 
 * @param   args    [0]: "watchuser"
 *                  [1]: user to be watched
 *                  [2]: "off"; meaning to stop watching a user
 */
void watchUser(char ** args){
    if(is_empty()) {
        insert_first("");
    }

    print_list();

    if(p == NULL) {
        pthread_create(&p, NULL, user, (void*) args[1]);
        printf("created watchuser pthread\n");
    }

    pthread_mutex_lock(&lock); // prevent collision with user function

    if(strcmp("off", args[2]) == 0) { // watchuser <username> off
        printf("removing user %s from watchlist", args[1]);
        // stop watching user
        struct node* temp = find(args[2]);

        if(temp != NULL) { // if the user exists in the linked list, remove them
            delete(temp->key);
        }
    } else {
        printf("start watching user\n");
        // start watching user
        insert_first(args[1]);
    }

    print_list();

    pthread_mutex_unlock(&lock); // prevent collision with user function
}
