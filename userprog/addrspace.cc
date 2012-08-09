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

#ifdef USE_TLB
AddrSpace::AddrSpace(OpenFile *exec, int spaceId)
{
	printf(">>>>>>>>>usamos TLB \n");
	executable = exec;
	executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
	unsigned int i, size;

	if ((noffH.noffMagic != NOFFMAGIC) &&
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
		SwapHeader(&noffH);
	ASSERT(noffH.noffMagic == NOFFMAGIC);

	// how big is address space?
	size = noffH.code.size + noffH.initData.size + noffH.uninitData.size
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
	numPages = divRoundUp(size, PageSize);

	//ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory

	pageTable = new TranslationEntry[numPages];

	for (i = 0; i < numPages; i++) {
		pageTable[i].virtualPage  = i;
		pageTable[i].physicalPage = 0;
		pageTable[i].valid        = false;
		pageTable[i].use          = false;
		pageTable[i].dirty        = false;
		pageTable[i].readOnly     = false;
		pageTable[i].swapPage     = NULL_PAGE;
		DEBUG('y', "Page %d con swapPage %d \n", i,pageTable[i].swapPage);
	}
	swapFileName = new char[10];
	sprintf(swapFileName, "SWAP.%d", spaceId);
	DEBUG('y', "SwapFile %s \n", swapFileName);
	ASSERT(fileSystem->Create(swapFileName, 0));
	swapPagesCounter = 0;
}
#else
AddrSpace::AddrSpace(OpenFile *exec)
{
	printf(">>>>>>>>>NO usamos TLB \n");
    NoffHeader noffH;
    unsigned int i, size;

    exec->ReadAt((char *)&noffH, sizeof(noffH), 0);
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
/* TODO No lo estamos necesitando para el swapping
	unsigned int NumPhysPagesLibres =  coreMap->NumClear();
//ya no son todas las paginas para un solo proceso
// ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory
	DEBUG('a',"num pages %d, NumPhysPagesLibres %d\n",numPages,NumPhysPagesLibres);
	ASSERT(numPages <= NumPhysPagesLibres);		// check we're not trying
	//--} smb 26/04/2012
*/
    DEBUG('a', "Initializing address space, num pages %d, size %d\n",
					numPages, size);
// first, set up the translation
    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
		pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
		//--{ smb 26/04/2012
		int numNextPagFisicaLibre = coreMap->Find(i);
		//pageTable[i].physicalPage = i;
		pageTable[i].physicalPage = numNextPagFisicaLibre;
		//coreMap->Mark(numNextPagFisicaLibre);
		//--} smb 26/04/2012
		pageTable[i].valid = true;
		pageTable[i].use = false;
		pageTable[i].dirty = false;
		pageTable[i].readOnly = false;  // if the code segment was entirely on
						// a separate page, we could set its
						// pages to be read-only
		pageTable[i].swapPage     = NULL_PAGE;						
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
		//~ exec->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]),
			//~ noffH.code.size, noffH.code.inFileAddr);
		for (i = 0; i < (int)noffH.code.size; i++) {
			 int virtAddr = noffH.code.virtualAddr+i;
			 unsigned int vpn = (unsigned) virtAddr / PageSize;
			 unsigned int offset = (unsigned) virtAddr % PageSize;
			 int pageFrame = pageTable[vpn].physicalPage;
			 int physAddr = pageFrame * PageSize + offset;
			 exec->ReadAt(&(machine->mainMemory[physAddr]),1,noffH.code.inFileAddr+i);
			 //machine->WriteMem(noffH.code.virtualAddr + i, 1,c);//esta ya accede a Translate para traducir virtual a real
		}
//--} smb 26/04/2012
    }
    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n",
			noffH.initData.virtualAddr, noffH.initData.size);
//--{ smb 26/04/2012
        //~ exec->ReadAt(&(machine->mainMemory[noffH.initData.virtualAddr]),
			//~ noffH.initData.size, noffH.initData.inFileAddr);
		for (i = 0; i < (int)noffH.initData.size; i++) {
			 int virtAddr = noffH.initData.virtualAddr+i;
			 unsigned int vpn = (unsigned) virtAddr / PageSize;
			 unsigned int offset = (unsigned) virtAddr % PageSize;
			 int pageFrame = pageTable[vpn].physicalPage;
			 int physAddr = pageFrame * PageSize + offset;
			 exec->ReadAt(&(machine->mainMemory[physAddr]),1,noffH.initData.inFileAddr+i);
			 //machine->WriteMem(noffH.initData.virtualAddr + i, 1,c);
		}
//--} smb 26/04/2012
    }
    DEBUG('a', "SwapFile %s \n", swapFileName);
}
#endif
//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
   //--{ smb 26/04/2012
   //limpiamos los marcos que uso y los seteamos en libres
   for (unsigned int i = 0; i < numPages; i++) {
		coreMap->Clear(pageTable[i].physicalPage );
   } 	
   //--} smb 26/04/2012
   delete pageTable;
   delete swapFile;
   delete [] swapFileName;
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
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
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
    #ifndef USE_TLB
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
    #endif
}

TranslationEntry *AddrSpace::EntryAt(int page)
{
	unsigned int i;
	if (page >  numPages) {
		printf("El número de página %d es mayor al tamaño de la tabla de paginas %d.\n",page, NumPhysPages);
		ASSERT(false);
	}

	if (!pageTable[page].valid) {
		int numNextPagFisicaLibre = coreMap->Find(page);
		pageTable[page].physicalPage = numNextPagFisicaLibre;
		//coreMap->Mark(numNextPagFisicaLibre);
		pageTable[page].valid = true;
		pageTable[page].use = false;
		pageTable[page].dirty = true; // Porque el coremap copia una entrada de memoria a disco solo si esta fue modicada.
// Entonces cuando la leemos del archivo ejecutable solo la cargamos en ram y el seteando
// el bit dirty nos aseguramos que si el coremap la saca la copie en disco (independientemente
// de si el micro la modifico o no).
		pageTable[page].readOnly = false;
		pageTable[page].inMemory = true;
		pageTable[page].swapPage = NULL_PAGE;

		bzero(&(machine->mainMemory[pageTable[page].physicalPage * PageSize]), PageSize);

		if (noffH.code.size > 0) {
			DEBUG('a', "Initializing code segment, at 0x%x, size %d\n",
				noffH.code.virtualAddr, noffH.code.size);

			for (i = 0; i < (int)noffH.code.size; i++) {
				 int virtAddr = noffH.code.virtualAddr+i;
				 unsigned int vpn = (unsigned) virtAddr / PageSize;
				 unsigned int offset = (unsigned) virtAddr % PageSize;
				 if (vpn == page) {
					int pageFrame = pageTable[vpn].physicalPage;
					int physAddr = pageFrame * PageSize + offset;
					executable->ReadAt(&(machine->mainMemory[physAddr]),1,noffH.code.inFileAddr+i);
				}
			}
		}
		if (noffH.initData.size > 0) {
			DEBUG('a', "Initializing data segment, at 0x%x, size %d\n",
				noffH.initData.virtualAddr, noffH.initData.size);
			for (i = 0; i < (int)noffH.initData.size; i++) {
				 int virtAddr = noffH.initData.virtualAddr+i;
				 unsigned int vpn = (unsigned) virtAddr / PageSize;
				 unsigned int offset = (unsigned) virtAddr % PageSize;
				 if (vpn == (unsigned) page) {
					 int pageFrame = pageTable[vpn].physicalPage;
					 int physAddr = pageFrame * PageSize + offset;
					 executable->ReadAt(&(machine->mainMemory[physAddr]),1,noffH.initData.inFileAddr+i);
				}
			}
		}
	} else if (!pageTable[page].inMemory) {//no esta en memoria
	
			DEBUG('y', "Pide leer de disco la page %d con orden de disco en %d \n", page,pageTable[page].swapPage);
			ASSERT(pageTable[page].swapPage != NULL_PAGE); //debe estar si o si en disco (es valida pero no esta en memoria)
		
		
			// TODO extraer la pagina de disco y cargarla en la ram
			int numNextPagFisicaLibre = coreMap->Find(page);
			pageTable[page].physicalPage = numNextPagFisicaLibre;
			//~ pageTable[page].valid = true;
			//~ pageTable[page].use = false;
			//~ pageTable[page].dirty = false;
			//~ pageTable[page].readOnly = false;
			pageTable[page].inMemory = true;
			
			//reseteo la RAM
			bzero(&(machine->mainMemory[pageTable[page].physicalPage * PageSize]), PageSize);
			//escribo en la RAM
			
			ASSERT(readFromSwap(&(machine->mainMemory[pageTable[page].physicalPage * PageSize]), pageTable[page].swapPage));
		
	}

	return &pageTable[page];
}

void AddrSpace::UpdateEntryAt(int page, TranslationEntry *entry)
{
	if (page > numPages) {
		printf("El número de página es mayor al tamaño de la tabla de paginas.\n");
		ASSERT(false);
	}

	pageTable[page].virtualPage = entry->virtualPage;
	pageTable[page].physicalPage = entry->physicalPage;
	pageTable[page].dirty = entry->dirty;
	pageTable[page].readOnly = entry->readOnly;
	pageTable[page].valid = entry->valid;
	pageTable[page].use = entry->use;
	pageTable[page].swapPage = entry->swapPage;
	pageTable[page].inMemory = entry->inMemory;
}

bool AddrSpace::writeToSwap(char*buf,int page) {
	int nroFrame;
	
	if (pageTable[page].swapPage != NULL_PAGE){//esta en disco
		if (!pageTable[page].dirty) {//no fue modificada, la copia de disco es actual
			return true;
		}
		nroFrame = pageTable[page].swapPage;
	} else {
		//~ DEBUG('y', "Pide escribir en disco page %d con orden de disco en %d \n", page,pageTable[page].swapPage);
		nroFrame = swapPagesCounter;		
		swapPagesCounter=swapPagesCounter+1;
	}
	ASSERT(numPages > swapPagesCounter);

	if (swapFile == NULL) {
		swapFile = fileSystem->Open(swapFileName);
	}
	
	pageTable[page].swapPage = nroFrame;
	pageTable[page].inMemory = false;
	DEBUG('y', "Pide escribir en disco page %d con orden de disco en %d \n", page,pageTable[page].swapPage);
	//~ DEBUG('y', "swapPagesCounter %d \n", swapPagesCounter);
	return (swapFile->WriteAt(buf,PageSize,nroFrame*PageSize) == PageSize);
}

bool AddrSpace::readFromSwap(char*buf,int ordenEnDisco) {
	if (swapFile == NULL) {
		swapFile = fileSystem->Open(swapFileName);
	}
	
	return (swapFile->ReadAt(buf,PageSize,ordenEnDisco*PageSize) == PageSize);
}
 

