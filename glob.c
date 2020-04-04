#include <glob.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  glob_t  paths;
  int     csource;
  char    **p;
		 
  printf("argv[0] = %s\n", argv[0]);
  printf("argv[1] = %s\n", argv[1]);  /* as pathname */
			      
  /* Find all ".h" files in a given directory */
  csource = glob("*.h", 0, NULL, &paths);
					   
  if (csource == 0) {
    for (p = paths.gl_pathv; *p != NULL; ++p)
      puts(*p);

    globfree(&paths);
  }
}