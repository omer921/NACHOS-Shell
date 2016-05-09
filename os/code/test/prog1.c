#include "syscall.h"

int
main()
{
  OpenFileId output = ConsoleOutput;
  //  printf("running user prog1\n");
  char* str = "current user program: prog1 end \n";
  char* h = "hello\n";
  char new = '\n';
  int i,j;
  for (i = 0; i < 5; i++)
  {
    Write(str, 27, output);
    // Write(new, 1, output);  
    for (j = 0; j < 10000; j++);
  }
  
  Exit(0);
}
