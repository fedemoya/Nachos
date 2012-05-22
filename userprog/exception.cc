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

void readStringFromRegister(char *buf, int reg);
bool writeCharsToMem(char *str, int size, int addr);
void incrementarPC();

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
    int type = machine->ReadRegister(2);

    char *chars = new char(100);
    int bufferAddr;
    int openFileId, size;

    static NuestroFilesys *nuestroFilesys = NULL;
    if(nuestroFilesys == NULL) {
    	nuestroFilesys = new NuestroFilesys;
    }

    if (which == SyscallException) {
    	switch (type) {
    		case SC_Halt :
    			DEBUG('a', "Shutdown, initiated by user program.\n");
    			interrupt->Halt();
    			break;
    		case SC_Create :
    			/* para depuración */ printf("Se ejecuto CREATE\n");
    			readStringFromRegister(chars, 4);
    			nuestroFilesys->nuestraCreate(chars);
    			incrementarPC();
    			break;
    		case SC_Open :
    			/* para depuración */ printf("Se ejecuto OPEN\n");
    			readStringFromRegister(chars, 4);
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
				readStringFromRegister(chars, 4);
				size = machine->ReadRegister(5);
				openFileId = machine->ReadRegister(6);
				nuestroFilesys->nuestraWrite(chars, size, openFileId);
				incrementarPC();
				break;
    		default :
    			ASSERT(false);
    	}

    } else {
    	// TODO Manejar el resto de las excepciones.
    	// Al menos las que puedan ser disparadas por el método Translate.
    	printf("Unexpected user mode exception %d %d\n", which, type);
    	ASSERT(false);
    }
}

void readStringFromRegister(char *str, int reg) {
	int cont = 0;
	while(true){
		machine->ReadMem(machine->ReadRegister(reg) + cont,1, (int *)&str[cont]);
		if (str[cont] == '\0')
		  break;
		cont++;
	}
}

bool writeCharsToMem(char *str, int size, int addr) {
	int count = 0;
	while(count < size) {
		if(!machine->WriteMem(addr + count, 1, (int)str[count]))
			return false;
		count++;
	}
	return true;
}

void incrementarPC() {
    machine->WriteRegister(PCReg,machine->ReadRegister(NextPCReg));
}
