/* cat.c
prints out the content of a Nachos file 
*/

#include "syscall.h"
#define TransferSize 8

int main(int argc, char **argv)
{
OpenFileId output = ConsoleOutput;
OpenFileId openFile;
int i, amountRead;
char buffer[20];
i = 0;

if ((openFile = Open(argv[1])) == 0) {
   Write("No such a file\n", 15, ConsoleOutput);
   Exit(1);
}

Write ("Opening file: ", 14, ConsoleOutput);
while (argv[1][i] != '\0') {
	Write (&argv[1][i], 1, ConsoleOutput);
	i++;
}
Write ("\n", 1, ConsoleOutput);

while ((amountRead = Read(buffer, TransferSize, openFile)) > 0)
        for (i = 0; i < amountRead; i++)
            Write(&buffer[i],1, ConsoleOutput);
}

