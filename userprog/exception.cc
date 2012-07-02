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


void syscallExceptionHandler();

bool readStringFromReg(char *buf, int reg);
bool readStringFromDirMem(char *str, int dir);
bool readCharsFromMem(char *buf, int size, int reg);
bool writeCharsToMem(char *str, int size, int addr);
void incrementarPC();
bool readArgsFromMem(int dir, char ***argv, int numArgs);

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
	int vAddrs; // Dirección virtual que debemos traducir.
    switch(which) {
    	case SyscallException:
    		syscallExceptionHandler();
    		break;
    	case PageFaultException:
    		DEBUG('z', "PageFault Trap\n");
    		vAddrs = machine->ReadRegister(BadVAddrReg);
    		// hay que traducir la vAddrs a una pAddrs
    		// hay que escribir en la tlb la pAddrs
    		// Importante! Hay que limpiar la tlb en cada
    		// cambio de contexto.
    		//
    		// A la hora de leer, osea machine->ReadMem,
    		// si la lectura dio falle, osea ReadMem retorno falso,
    		// hay que reintentar(?)
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
	
	//Exec
	int argc;
	int dirArgv;
	char**argv;
	
	//Exec
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
			readStringFromReg(chars, 4);
			nuestroFilesys->nuestraCreate(chars);
			incrementarPC();
			break;
		case SC_Open :
			/* para depuración */ printf("Se ejecuto OPEN\n");
			readStringFromReg(chars, 4);
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
			if (!readCharsFromMem(chars, size, 4)) { // TODO size no puede ser > 100
				printf("ERROR al intentar leer argumentos de Write.\n");
				ASSERT(false);
			}
			
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
		//~ case SC_Exec :
			//~ /* para depuración */ printf("Se ejecuto EXEC\n");
			//~ readStringFromReg(chars, 4);
			//~ spaceId = nuestraExec(chars);
			//~ machine->WriteRegister(2, spaceId);
			//~ incrementarPC();
			//~ break;
		case SC_Exec:
			/* para depuración */ printf("Se ejecuto EXEC\n");
			readStringFromReg(chars, 4);
			argc = machine->ReadRegister(5);
			argv = new char*[argc + 1];
			for (int i = 0; i < argc; i++){
				argv[i] = new char[100];
			}
			dirArgv = machine->ReadRegister(6);
			if (!readArgsFromMem(dirArgv,&argv,argc)) {
				printf("ERROR al intentar leer argumentos de Exec.\n");
				ASSERT(false);
			}				
			spaceId = nuestraExecWithArgs(chars,argc,argv);
			machine->WriteRegister(2, spaceId);
			delete [] argv;
			incrementarPC();
			break;
		case SC_Exit:
			/* para depuración */ printf("Se ejecuto EXIT\n");
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

bool readStringFromReg(char *str, int reg) {
	return readStringFromDirMem(str,machine->ReadRegister(reg));
}

//se le debe pasar una virtual y ReadMem hace la traduccion
bool readStringFromDirMem(char *str, int dir)  {
	int cont = 0;
	int buf;
	while(true){
		if (!machine->ReadMem(dir + cont,1, &buf)) 
			return false;
		str[cont] = (char) buf;
		if (str[cont] == '\0')
		  break;
		cont++;
	}
	return true;
}

bool readCharsFromMem(char *chars, int size, int reg) {
	int cont = 0;
	int buf;
	while(cont < size){
		if(!machine->ReadMem(machine->ReadRegister(reg) + cont,1, &buf)) 
			return false;
		chars[cont] = (char) buf;
		cont++;
	}
	return true;
}

bool writeCharsToMem(char *str, int size, int addr) {
	int count = 0;
	while(count < size) {
		if(!machine->WriteMem(addr + count, 1, (int)str[count])) // Hay que ver que onda acá!!!
			return false;
		count++;
	}
	return true;
}

void incrementarPC() {
    machine->WriteRegister(PCReg,machine->ReadRegister(NextPCReg));
}

bool readArgsFromMem(int dir, char ***pargv, int numArgs)  {
	int cont = 0;
	int strLenCont = 0;
	while(cont < numArgs) {
		if (!readStringFromDirMem( (*pargv)[cont] ,dir + strLenCont))
			return false;
		strLenCont+= strlen((*pargv)[cont])+1;
		printf("%s \n",(*pargv)[cont]);
		cont++;
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
