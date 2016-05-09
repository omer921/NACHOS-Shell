#include "syscall.h"

int main ()
  {
  char buffer[7];
  int i;
  buffer[0] = 'H';
  buffer[1] = 'E';
  buffer[2] = 'L';
  buffer[3] = 'L';
  buffer[4] = 'O';
  buffer[5] = '\0';
  buffer[6] = '\n';

  for (i = 0; i < 10; i++) {
      Write(buffer, 7, ConsoleOutput);
  }
  Exit(0);
}


