#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern char **environ;

int main(int argc, char **argv)
{
  int i;
  for (i = 0; environ[i] != NULL; i++){
    printf("-- %d [%s]\n", i, environ[i]);
  }
  char *c = malloc(20);
  strcpy(c, "HELLO");
  setenv(c, "kitty", 1);

  for (i = 0; environ[i] != NULL; i++){
    printf("++ %d [%s]\n", i, environ[i]);
  }

  strncpy(c, "WHOIS", 5);
  setenv(c, "cshen", 1);

  for (i = 0; environ[i] != NULL; i++){
    printf("%d [%s]\n", i, environ[i]);
  }
}