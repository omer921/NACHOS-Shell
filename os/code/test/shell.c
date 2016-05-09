#include "syscall.h"

int
main()
{
    SpaceId newProc;
    OpenFileId input = ConsoleInput;
    OpenFileId output = ConsoleOutput;
    char prompt[2], ch, buffer[60];
    int i;

    prompt[0] = 'S';
    prompt[1] = 'H';
    prompt[2] = 'E';
    prompt[3] = 'L';
    prompt[4] = 'L';
    prompt[5] = '>';
    
   while( 1 )
    {
	Write(prompt, 6, output);

	i = 0;
	//int z = 1; 
	do {
	
	    Read(&buffer[i], 1, input); 

	   } while( buffer[i++] != '\n' );
	
	// Write(buffer[0], 4, input);
	buffer[--i] = '\0';

	if( i > 0 ) {
		newProc = Exec(buffer);
		Join(newProc);
	}
   }
}

