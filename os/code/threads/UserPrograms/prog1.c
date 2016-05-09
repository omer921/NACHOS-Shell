#include "syscall.h"

int main ()
  {
  char buffer[6];

//  while (1) {
  Write(buffer, 5, ConsoleOutput);
// }

  Exit(0);
  }


