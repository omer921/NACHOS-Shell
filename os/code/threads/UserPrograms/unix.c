#include "syscall.h"

int main ()
  {
  while (1) {
  Unix_Fork();
  }

  Exit(0);
  }


