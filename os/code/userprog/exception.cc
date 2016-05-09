// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include <stdlib.h>
#include "addrspace.h"
#include "synch.h"
#include <stdio.h>

void
ExecProcess(int filenames)
{
  char* filename = (char*) filenames;
  filename[7] = '/';
  //  printf("ExecProcess for %s\n", filename);                                              
  OpenFile *executable = fileSystem->Open(filename);
  AddrSpace *space;

  if (executable == NULL) {
    printf("Unable to open file %s\n", filename);
        return;
  }
  space = new AddrSpace(executable);
  currentThread->space = space;
  
  delete executable;                  // close file                                       
  
  space->InitRegisters();             // set the initial register values                  
  space->RestoreState();              // load page table register                         
  
  machine->Run();                     // jump to the user progam                          
  ASSERT(FALSE);                      // machine->Run never returns;                      
  // the address space exits                                                              
  // by doing the syscall "exit"                                                          
}



//#include "progtest.cc"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

int 
checkNulAhead(int name, int* physAddr) {
  //physAddr = (int*) malloc(1*sizeof(int));
  machine->Translate(name, physAddr, 1, true);
  //printf("in check nul\n");
  if (machine->mainMemory[*physAddr] != '\0') {
    
    return 1;
  }
  else { 
    
    return 0;
  }

}

  
void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if ((which == SyscallException) && (type == SC_Halt)) {
	DEBUG('a', "Shutdown, initiated by user program.\n");
   	interrupt->Halt();
    } 



    else if ((which == SyscallException) && (type == SC_Exit)) {
      int arg1 = machine->ReadRegister(4);
      processTermID = currentThread->processID;
      if (ioList->IsEmpty()) {}
      else {
      Thread *t = static_cast<Thread*>(ioList->Remove());
      t->space->RestoreState();
      t->space->InitRegisters();
      scheduler->ReadyToRun(t);
      }
      currentThread->space->DealocateMem();
      currentThread->Finish();
     } 
    
    else if ((which == SyscallException) && (type == SC_Join)) {
      int arg1 = machine->ReadRegister(4);
      interrupt->SetLevel(IntOff);
      currentThread->space->SaveState();
      ioList->Append(currentThread);
      currentThread->Sleep();
      
      
     }

    else if ((which == SyscallException) && (type == SC_Write)) {
      
      int size = machine->ReadRegister(5);  //SIZE OF STRING
      int bufferAdr = machine->ReadRegister(4); //location of string  
      int* physAddr = (int*) malloc(1*sizeof(int));
 
      for (int i = 0; i < size; i++) {
	machine->Translate(bufferAdr+i, physAddr, 1, true);
	printf("%c", machine->mainMemory[*physAddr]);
      }
      printf("\n");
    }



    else if ((which == SyscallException) && (type == SC_Exec)) {
      int name = machine->ReadRegister(4);
      int* physAddr = (int*) malloc(1*sizeof(int));
      machine->Translate(name, physAddr, 1, true);
      char buffer[60]; // = (char) malloc(sizeof(char)*60);
      
      int i = 0;
      while(i<60 && checkNulAhead(name+i, physAddr)) {
	machine->Translate(name+i, physAddr, 1, true);
	//	printf("%c\n", machine->mainMemory[*physAddr]);
	buffer[i] = machine->mainMemory[*physAddr];
	i++;
      } 
      
      //printf("in exec %s\n", buffer); 
      
      Thread *t = new Thread(buffer);
      currentProcess += 1;   
      t->processID = currentProcess;                                               
      char* stri = (char*) malloc(sizeof(buffer)); 
      stri = (char*) &buffer;
      currentThread->space->SaveState();
      machine->WriteRegister(4, currentProcess); 
      t->Fork(ExecProcess, (int) stri);
      
    }

    else if ((which == SyscallException) && (type == SC_Read)) {
      int bufferOffset = machine->ReadRegister(4);
      int bufferNum = machine->ReadRegister(5);
      char ch;
      for( int i = 0; i < bufferNum; i++){
	
	if( scanf("%c",&ch) != EOF )
	  if( machine->WriteMem(bufferOffset + i , 1, (int)ch ) == false )
	    return;
	  else
	    break;
      }




    }



    

    else {
	printf("Unexpected user mode exception %d %d\n", which, type);
	ASSERT(FALSE);
    }

    
    int currentPC = machine->ReadRegister(PCReg);
    int nextPC = machine->ReadRegister(NextPCReg);
    machine->WriteRegister(PrevPCReg, currentPC);
    machine->WriteRegister(PCReg, nextPC);
    machine->WriteRegister(NextPCReg, nextPC+4);
    			   
}




