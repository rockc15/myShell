#include <stdio.h>
#include <unistd.h>

int main()
{
  int i;
  for (i = 1; i <= 10; i++) {  // sleep 20 seconds
    sleep(2);
    printf("child has slept for %d seconds...\n", 2*i);
  }
  printf("child has exited...\n");
  return 19;
}
