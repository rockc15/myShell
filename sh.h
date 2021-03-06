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
#include <glob.h>
#include "get_path.h"
#include <utmpx.h>
#include <pthread.h>
#include <fcntl.h>



int pid;
int sh( int argc, char **argv, char **envp);
void which(char *command, struct pathelement *pathlist);
void where(char *command, struct pathelement *pathlist);
void list ( char **dir );
void printWorkingDir();
void printenv(char **envp);
void printPid();
void changeDir(char ** args, char * preve);
void killProcess(char ** args);
void changePrompt(char * pro, char * args1);
void printEnv(char **args);
void setEnv(char **args);
void watchUser(char ** args);
int redirection(char ** args);
int backGround(char ** args);
void printWatchedUsers();


#define PROMPTMAX 32
#define MAXARGS 10