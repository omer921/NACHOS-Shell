#include "syscall.h"

int main ()
  {
  int id;
  char buffer[6];

  if ((id = Unix_Fork()) > 0)
	{/* this is parent part */
	Write(buffer, 5, ConsoleOutput); 
		/* we use ConsoleOutput to indicate it's parent */
	Exit(0);
	}
  else {
		/* we use ConsoleInput to indicate it's parent */
       Write(buffer, 5, ConsoleInput);
	Exit(0);
  }
  }


