/* shell.c

This is a simple shell to run on the NACHOS operating system.  Its 
abilities include:

1) Accepting command line arguments.  The command line is passed as one 
string to the kernel which parses it into the respective tokens (with a 
few exceptions.

2) Ability to run a program in the background.  In this instance the 
shell does not issue a Join syscall.  Rather it just presents the user 
with a new prompt.  This feature is utilized by ending the command line 
with an '&' character.

3) As a side note, the command line is checked for any trailing spaces 
which are then removed.

*/

#include "syscall.h"

int main()
{
  SpaceId newProc;
  OpenFileId input = ConsoleInput;
  OpenFileId output = ConsoleOutput;
  char prompt[7], ch, buffer[60];
  int i;

  prompt[0] = 'N';
  prompt[1] = 'A';
  prompt[2] = 'C';
  prompt[3] = 'H';
  prompt[4] = 'O';
  prompt[5] = 'S';
  prompt[6] = '>';

  while( 1 )
    {
    Write(prompt, 7, output);

    i = 0;
    
    do
      {
      Read(&buffer[i], 1, input); 

      if (buffer[i] == '\b')
        i--;
      } 
    while (buffer[i++] != '\n');

    buffer[--i] = '\0';

    while (buffer[--i] == ' ')
      buffer[i] = '\0';

    if( i > 0 )
      {
      if (buffer[i] == '&')
        {
        buffer[i] = '\0';
        while (buffer[--i] == ' ')
          buffer[i] = '\0';
        Write("Executing program in the background.\n", 37, ConsoleOutput);
        newProc = Exec(buffer);
        if (newProc == -1)
          Write("Couldn't execute program.\n", 26, ConsoleOutput);
        }
      else
        {
        newProc = Exec(buffer);
        if (newProc == -1)
          Write("Couldn't execute program.\n", 26, ConsoleOutput);
        else
          {
          i = Join(newProc);
          if (i == 0)
            Write("Program exited with status 0.\n", 30, ConsoleOutput);
          else
            Write("Program exited with an error.\n", 30, ConsoleOutput);
          }
        }
      }
    }
}

