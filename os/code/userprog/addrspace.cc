// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"
#include <stdlib.h>
#include <stdio.h>
//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

static int iMarker = 0;
static int marker = 0;
static int programLoaded = 0;

AddrSpace::AddrSpace(OpenFile *executable)
{
    NoffHeader noffH;
    unsigned int i, size;
    
    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size 
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
    numPages = divRoundUp(size, PageSize);
     if (programLoaded > 0) {
       //numPages = marker;
       marker = iMarker;
       iMarker += numPages;
       //marker += numPages+1;
       // printf("if: current marker is %i, now is %i \n", marker,iMarker);
       programLoaded++;
    } else {
       programLoaded++; 
      iMarker = numPages;
      //      printf("else: current marker is %i, now is %i \n", marker,iMarker);
      }
    size = numPages * PageSize;
    //printf("num pages: %i, iMarker: %i\n", marker, iMarker);
    ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", 
					numPages, size);
// first, set up the translation 
    pageTable = new TranslationEntry[numPages];
    //    printf("translating page table\n");
    for (i = 0; i < numPages; i++) {
        pageTable[i].virtualPage = i;// + marker;	// for now, virtual page # = phys page #
        pageTable[i].physicalPage = i + marker;
	pageTable[i].valid = TRUE;
	pageTable[i].use = FALSE;
	pageTable[i].dirty = FALSE;
	pageTable[i].readOnly = FALSE;  // if the code segment was entirely on 
					// a separate page, we could set its 
					// pages to be read-only
	//memset(machine->mainMemory, 0, size);
	memset(&machine->mainMemory[pageTable[i].physicalPage*PageSize], 0, PageSize);
    }    
// zero out the entire address space, to zero the unitialized data segment 
// and the stack segment
//    bzero(machine->mainMemory, size); rm for Solaris
    //memset(machine->mainMemory, 0, size);

// then, copy in the code and data segments into memory
    int offset = noffH.code.virtualAddr % PageSize;
    int offsetI = noffH.initData.virtualAddr % PageSize;
    //memset(&(machine->mainMemory[pageTable[0].physicalPage]), 0, PageSize);
    // memset(&(machine->mainMemory[pageTable[noffH.code.virtualAddr/PageSize].physicalPage*PageSize+offset]), 0,  noffH.code.size);
    if (noffH.code.size > 0) {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
			noffH.code.virtualAddr, noffH.code.size);
	
	
	executable->ReadAt(&(machine->mainMemory[pageTable[noffH.code.virtualAddr/PageSize].physicalPage*PageSize+offset]),
			   noffH.code.size, noffH.code.inFileAddr);
	//executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]),
	//		noffH.code.size, noffH.code.inFileAddr);
    }
    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
			noffH.initData.virtualAddr, noffH.initData.size);
        

	executable->ReadAt(&(machine->mainMemory[pageTable[noffH.initData.virtualAddr/PageSize].physicalPage*PageSize+offsetI]),
			  noffH.initData.size, noffH.initData.inFileAddr);
	//executable->ReadAt(&(machine->mainMemory[noffH.initData.virtualAddr]),
	//		noffH.initData.size, noffH.initData.inFileAddr);
    }
    
}

void
AddrSpace::DealocateMem()
{
  marker -= numPages;
  iMarker -= numPages;
}
//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
   delete pageTable;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;
    //    printf("cleaing up registers: %i\n", NumTotalRegs);
    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);
    //    int* translated = (int*) malloc(1*sizeof(int));

    //machine->ReadMem((PageSize*pageTable[0].virtualPage),4, translated); 
    // Initial program counter -- must be location of "Start"
    //printf("current PC location %s\n", (pageTable[0].virtualPage));
    machine->WriteRegister(PCReg,0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    //printf("read program counter\n");
    machine->WriteRegister(NextPCReg, 4);
    //printf("read next command\n");
   
   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    //printf("setting stack register\n");
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
    //printf("made it to post debug stage\n");
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{
   pageTable = machine->pageTable;
   numPages = machine->pageTableSize; 
}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}
