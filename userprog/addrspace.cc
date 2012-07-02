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
    size = numPages * PageSize;

	//--{ smb 26/04/2012
	mainVirtDirArgv = size;//en esta direccion pondremos los argumentos en caso de necesitarse
	
	unsigned int NumPhysPagesLibres =  machine->bitMapPagMemAdmin->NumClear();
//ya no son todas las paginas para un solo proceso
// ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory
	DEBUG('a',"num pages %d, NumPhysPagesLibres %d\n",numPages,NumPhysPagesLibres);
	ASSERT(numPages <= NumPhysPagesLibres);		// check we're not trying
	//--} smb 26/04/2012
    DEBUG('a', "Initializing address space, num pages %d, size %d\n", 
					numPages, size);
// first, set up the translation 
    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
		pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
		//--{ smb 26/04/2012
		int numNextPagFisicaLibre = machine->bitMapPagMemAdmin->Find(); 
		//pageTable[i].physicalPage = i;
		pageTable[i].physicalPage = numNextPagFisicaLibre;
		machine->bitMapPagMemAdmin->Mark(numNextPagFisicaLibre);
		//--} smb 26/04/2012	
		pageTable[i].valid = true;
		pageTable[i].use = false;
		pageTable[i].dirty = false;
		pageTable[i].readOnly = false;  // if the code segment was entirely on 
						// a separate page, we could set its 
						// pages to be read-only
    }
    
// zero out the entire address space, to zero the unitialized data segment 
// and the stack segment
//--{ smb 26/04/2012
    //bzero(machine->mainMemory, size);
     for (i = 0; i < numPages; i++) {
		 bzero(&(machine->mainMemory[pageTable[i].physicalPage * PageSize]), PageSize);	
	 }
//--} smb 26/04/2012	


// then, copy in the code and data segments into memory
    if (noffH.code.size > 0) {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
			noffH.code.virtualAddr, noffH.code.size);
//--{ smb 26/04/2012			
		//~ executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]),
			//~ noffH.code.size, noffH.code.inFileAddr);
		for (i = 0; i < (int)noffH.code.size; i++) {
			 int virtAddr = noffH.code.virtualAddr+i;
			 unsigned int vpn = (unsigned) virtAddr / PageSize;
			 unsigned int offset = (unsigned) virtAddr % PageSize;
			 int pageFrame = pageTable[vpn].physicalPage;
			 int physAddr = pageFrame * PageSize + offset;
			 executable->ReadAt(&(machine->mainMemory[physAddr]),1,noffH.code.inFileAddr+i);
			 //machine->WriteMem(noffH.code.virtualAddr + i, 1,c);//esta ya accede a Translate para traducir virtual a real
		}
//--} smb 26/04/2012	        
    }
    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
			noffH.initData.virtualAddr, noffH.initData.size);
//--{ smb 26/04/2012						
        //~ executable->ReadAt(&(machine->mainMemory[noffH.initData.virtualAddr]),
			//~ noffH.initData.size, noffH.initData.inFileAddr);
		for (i = 0; i < (int)noffH.initData.size; i++) {
			 int virtAddr = noffH.initData.virtualAddr+i;
			 unsigned int vpn = (unsigned) virtAddr / PageSize;
			 unsigned int offset = (unsigned) virtAddr % PageSize;
			 int pageFrame = pageTable[vpn].physicalPage;
			 int physAddr = pageFrame * PageSize + offset;
			 executable->ReadAt(&(machine->mainMemory[physAddr]),1,noffH.initData.inFileAddr+i);
			 //machine->WriteMem(noffH.initData.virtualAddr + i, 1,c);			
		}
//--} smb 26/04/2012	        			                    
    }
}


bool AddrSpace::ApilarArgumentos(int argc,char**argv) {
	int tamanioTotalArgs=0;
	for(int i= 0;i<argc;i++) {
		tamanioTotalArgs+= strlen(argv[i]) + 1;
	}
	
	aumentoStackArgsExec = tamanioTotalArgs;

	DEBUG('a', "Extra size argumentos Exec igual a %d bytes, para %d cant de argumentos \n", tamanioTotalArgs,argc);
	
	if (!AumentarEspacio(tamanioTotalArgs))
		return false;
	
	mainArgc = argc;
	mainVirtDirArgv =  numPages * PageSize - aumentoStackArgsExec - 16;
	int contOffSet = 0;
	int indStr = 0;
	for (int nroArg = 0; nroArg < argc; nroArg++) {
			 indStr = 0;		 
			  do {
				int virtAddr = mainVirtDirArgv + contOffSet;
				machine->mainMemory[ traducirVirDir2PhisDir(virtAddr)] = argv[nroArg][indStr];
				contOffSet++;	
				indStr++;
			 } while (argv[nroArg][indStr] !='\0');
			 
	}
	DEBUG('a', "Expandemos el address space, num pages %d\n", 
					numPages);
	return true;
}

int AddrSpace::traducirVirDir2PhisDir (int virtAddr) {
	 unsigned int vpn = (unsigned) virtAddr / PageSize;
	 unsigned int offset = (unsigned) virtAddr % PageSize;
	 int pageFrame = pageTable[vpn].physicalPage;
	 int physAddr = pageFrame * PageSize + offset;
	 return physAddr;
}

bool AddrSpace::AumentarEspacio(int tamanio) {
	int i;
	int aumPages = divRoundUp(tamanio, PageSize);
	TranslationEntry *newPageTable;

	//No hay suficiente espacio en memoria
	if(aumPages > machine->bitMapPagMemAdmin->NumClear())
		return false;

	ASSERT(numPages + aumPages  <= NumPhysPages);

	newPageTable = new TranslationEntry[numPages + aumPages];

	for (i = 0; i < numPages; i++) {
		newPageTable[i].virtualPage = pageTable[i].virtualPage;
		newPageTable[i].physicalPage = pageTable[i].physicalPage;
		newPageTable[i].valid = pageTable[i].valid;
		newPageTable[i].use = pageTable[i].use;
		newPageTable[i].dirty = pageTable[i].dirty;
		newPageTable[i].readOnly = pageTable[i].readOnly;			
	}
	for (; i < numPages + aumPages; i++) {
		newPageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
		//--{ smb 26/04/2012
		int numNextPagFisicaLibre = machine->bitMapPagMemAdmin->Find(); 
		newPageTable[i].physicalPage = numNextPagFisicaLibre;
		machine->bitMapPagMemAdmin->Mark(numNextPagFisicaLibre);
		//--} smb 26/04/2012	
		newPageTable[i].valid = true;
		newPageTable[i].use = false;
		newPageTable[i].dirty = false;
		newPageTable[i].readOnly = false;  // if the code segment was entirely on 
						// a separate page, we could set its 
						// pages to be read-only
    }
	
	// Inicializo la totalidad de la memoria física asignada al proceso, para 
	// que quede en cero la parte de la data no inicializada y el stack.
    for (i = numPages; i < aumPages; i++) 
		bzero(&(machine->mainMemory[((newPageTable[i].physicalPage) * PageSize)]), PageSize);

	delete []pageTable;
	pageTable = newPageTable;
	numPages += aumPages;
	return true;
}

int AddrSpace::ObtenerArgc() {
	return mainArgc;
}

int AddrSpace::ObtenerVirtDirArgv() {
	return mainVirtDirArgv;
}
    
//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
   //--{ smb 26/04/2012
   //limpiamos los marcos que uso y los seteamos en libres
   for (unsigned int i = 0; i < numPages; i++) {
		machine->bitMapPagMemAdmin->Clear(pageTable[i].physicalPage );
   } 	
   //--} smb 26/04/2012
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

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    //~ machine->WriteRegister(StackReg, numPages * PageSize - 16);
    int restaStack = divRoundUp(aumentoStackArgsExec,4);
    machine->WriteRegister(StackReg, numPages * PageSize - restaStack*4 -16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - restaStack*4  - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{}

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
