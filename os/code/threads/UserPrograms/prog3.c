#include "syscall.h"

int main ()
  {
  char buffer[6];

  if (Unix_Fork() == 0)
	Write(buffer, 5, ConsoleInput);
	if (Unix_Fork() == 0)
		Write(buffer, 5, ConsoleInput);
  }


