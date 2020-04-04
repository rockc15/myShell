/*
 * sample program showing how wildcard is processed
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

main(int argc, char **argv)
{
  pid_t pid;
  int i = 0;
//   char **arg;
  
//   arg = (char **) NULL;

  printf("** argc [%d]\n", argc);
  while (argv[i]) {
    printf("(%d) [%s]\n", i, argv[i]);
    i++;
  }
}