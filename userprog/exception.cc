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
#include "nuestraSyscallImpl.h"
#include <time.h>
#include <limits.h>

void syscallExceptionHandler();
void pageFaultExceptionHandler();
void readStringFromMem(char *buf, int reg);
void readCharsFromMem(char *buf, int size, int reg);
bool writeCharsToMem(char *str, int size, int addr);
bool nuestraReadMem (int addr, int size, int *value);
bool nuestraWriteMem (int addr, int size, int value);
void incrementarPC();

void printException(int);

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

void
ExceptionHandler(ExceptionType which)
{
	switch(which) {
    	case SyscallException:
    		syscallExceptionHandler();
    		break;
    	case PageFaultException:
    		pageFaultExceptionHandler();
    		break;
    	default:
    	// TODO Manejar el resto de las excepciones.
    	// Al menos las que puedan ser disparadas por el método Translate.
    	printf("Unexpected user mode exception -> ");
    	printException(which);
    	ASSERT(false);
    }
}

void syscallExceptionHandler() {

	int type = machine->ReadRegister(2);

	char *chars = new char[100];
	int bufferAddr;
	int openFileId, size, status;
	int spaceId;

	static NuestroFilesys *nuestroFilesys = NULL;
	if(nuestroFilesys == NULL) {
		nuestroFilesys = new NuestroFilesys;
	}

	switch (type) {
		case SC_Halt :
			DEBUG('a', "Shutdown, initiated by user program.\n");
			interrupt->Halt();
			break;
		case SC_Create :
			/* para depuración */ printf("Se ejecuto CREATE\n");
			readStringFromMem(chars, 4);
			nuestroFilesys->nuestraCreate(chars);
			incrementarPC();
			break;
		case SC_Open :
			/* para depuración */ printf("Se ejecuto OPEN\n");
			readStringFromMem(chars, 4);
			openFileId = nuestroFilesys->nuestraOpen(chars);
			machine->WriteRegister(2, openFileId);
			incrementarPC();
			break;
		case SC_Read :
			/* para depuración */ printf("Se ejecuto READ\n");
			bufferAddr = machine->ReadRegister(4);
			size = machine->ReadRegister(5);
			openFileId = machine->ReadRegister(6);
			nuestroFilesys->nuestraRead(chars, size, openFileId); // TODO size no puede ser > 100
			if(!writeCharsToMem(chars, size, bufferAddr)){
				printf("ERROR al intentar escribir en memoria.\n");
			}
			incrementarPC();
			break;
		case SC_Write :
			/* para depuración */ printf("Se ejecuto WRITE\n");
			size = machine->ReadRegister(5);
			readCharsFromMem(chars, size, 4); // TODO size no puede ser > 100
			openFileId = machine->ReadRegister(6);
			nuestroFilesys->nuestraWrite(chars, size, openFileId);
			incrementarPC();
			break;
		case SC_Close :
			/* para depuración */ printf("Se ejecuto CLOSE\n");
			openFileId = machine->ReadRegister(4);
			nuestroFilesys->nuestraClose(openFileId);
			incrementarPC();
			break;
		case SC_Exec :
			/* para depuración */ printf("Se ejecuto EXEC\n");
			readStringFromMem(chars, 4);
			spaceId = nuestraExec(chars);
			machine->WriteRegister(2, spaceId);
			incrementarPC();
			break;
		case SC_Exit:
			/* para depuración */ printf("Se ejecuto EXIT\n");
            #ifdef USE_TLB
                stats->Print();
            #endif 
			status = machine->ReadRegister(4);
			nuestraExit(status);
			incrementarPC();
            
			break;
		case SC_Join:
			/* para depuración */ printf("Se ejecuto JOIN\n");
			spaceId = machine->ReadRegister(4);
			status = nuestraJoin(spaceId);
			machine->WriteRegister(2,status);
			incrementarPC();
			break;
		default :
			ASSERT(false);
	}
	delete [] chars;
}


long seed = 1;

void pageFaultExceptionHandler() {

	int vAddrs; // Dirección virtual que debemos traducir.
	int vPage; // Nº de página en la que se encuentra la dirección virtual que causo la falla.
	TranslationEntry *entry; // La entrada que vamos a escribir en la tlb.
	TranslationEntry *oldEntry; // La entrada que vamos sacar de la tlb.

	srand(seed++);
	if (seed == LONG_MAX - 1) {
		seed = 0;
	}

	// Sacamos de la tlb una entrada de manera aleatoria.
	int oldEntryIndex = (rand() % TLBSize);
	oldEntry = &machine->tlb[oldEntryIndex];
	if (oldEntry->valid) {
		currentThread->space->UpdateEntryAt(oldEntry->virtualPage, oldEntry);
	}


	// Ponemos en la posición que sacamos anteriormente la nueva entrada.
	vAddrs = machine->ReadRegister(BadVAddrReg);

	DEBUG('z',"PageFault Trap. Dirección virtual %d. TLB Index %d.\n", vAddrs, oldEntryIndex);

	vPage = divRoundDown(vAddrs, PageSize);
	entry = currentThread->space->EntryAt(vPage);
	machine->tlb[oldEntryIndex].virtualPage = entry->virtualPage;
	machine->tlb[oldEntryIndex].physicalPage = entry->physicalPage;
	machine->tlb[oldEntryIndex].dirty = entry->dirty;
	machine->tlb[oldEntryIndex].readOnly = entry->readOnly;
	machine->tlb[oldEntryIndex].valid = entry->valid;
	machine->tlb[oldEntryIndex].use = entry->use;

    stats->numErrorTLB++;
}

void readStringFromMem(char *str, int reg) {
	int cont = 0;
	int buf;
	while(true){
		if(!nuestraReadMem(machine->ReadRegister(reg) + cont,1, &buf)){
			printf("Error nuestraReadMem\n");
			ASSERT(false);
		}
		str[cont] = (char) buf;
		if (str[cont] == '\0')
		  break;
		cont++;
	}
}

void readCharsFromMem(char *chars, int size, int reg) {
	int cont = 0;
	int buf;
	while(cont < size){
		nuestraReadMem(machine->ReadRegister(reg) + cont,1, &buf);
		chars[cont] = (char) buf;
		cont++;
	}
}

bool writeCharsToMem(char *str, int size, int addr) {
	int count = 0;
	while(count < size) {
		if(!nuestraWriteMem(addr + count, 1, (int)str[count]))
			return false;
		count++;
	}
	return true;
}

void incrementarPC() {
    machine->WriteRegister(PCReg,machine->ReadRegister(NextPCReg));
}

bool nuestraReadMem (int addr, int size, int *value) {
	if (!machine->ReadMem(addr, size, value)) {
		// Fallo la lectura de la tlb pero el sistema operativo se encargo del problema y escribió la dirección que faltaba.
		// Reintentamos, no debería fallar.
		return machine->ReadMem(addr, size, value);
	}
	return true;
}

bool nuestraWriteMem (int addr, int size, int value) {
	if (!machine->WriteMem(addr, size, value)) {
		// Fallo la lectura de la tlb pero el sistema operativo se encargo del problema y escribió la dirección que faltaba.
		// Reintentamos, no debería fallar.
		return machine->WriteMem(addr, size, value);
	}
	return true;
}

void printException(int which) {
	switch(which){
		case NoException:
			printf("Everything ok!\n");
			break;
		case SyscallException:
			printf("A program executed a system call\n");
			break;
		case PageFaultException:
			printf("No valid translation found\n");
			break;
		case ReadOnlyException:
			printf("Write attempted to page marked \"read-only\"\n");
			break;
		case BusErrorException:
			printf("Translation resulted in an invalid physical address\n");
			break;
		case AddressErrorException:
			printf("Unaligned reference or one that was beyond the end of the address space\n");
			break;
		case OverflowException:
			printf("Integer overflow in add or sub\n");
			break;
		case IllegalInstrException:
			printf("Unimplemented or reserved instr\n");
			break;
		case  NumExceptionTypes:
			printf("NumExceptionTypes");
			break;
		default:
			printf("Excepcion desconocida\n");
			break;
	};
}
