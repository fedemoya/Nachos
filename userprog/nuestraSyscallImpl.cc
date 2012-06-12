/*
 * nustraSyscallImpl.cc
 *
 *  Created on: 05/05/2012
 */

#include "system.h"
#include "nuestraSyscallImpl.h"

static Semaphore *readAvail;
static Semaphore *writeDone;

// Callbacks para que la consola nos avise cuando
// hay caracteres para leer (con GetChar) o cuando pudo escribir
// el caracter que le pasamos (con PutChar).
static void ReadAvailHandler(void* arg) { readAvail->V(); };
static void WriteDoneHandler(void* arg) { writeDone->V(); };

SynchConsole::SynchConsole(const char *readFile, const char *writeFile) {
	console = new Console (readFile, writeFile, ReadAvailHandler, WriteDoneHandler, NULL);
	readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);
	lock = new Lock("SynchConsole Lock");
}

SynchConsole::~SynchConsole() {
	delete console;
	delete lock;
}

char SynchConsole::GetChar() {
	lock->Acquire();
	readAvail->P();
	char ch = console->GetChar();
	lock->Release();
	return ch;
}

void SynchConsole::PutChar(char ch) {
	lock->Acquire();
	console->PutChar(ch);
	writeDone->P();
	lock->Release();
}

NuestroFilesys::NuestroFilesys() {
	openFiles = new List<OpenFileData*>;
	console = new SynchConsole(NULL,NULL); // al pasarle (NULL, NULL) lee y escribe en la consola unix
}

NuestroFilesys::~NuestroFilesys() {
	delete openFiles;
}

void NuestroFilesys::nuestraCreate(char *name) {
	fileSystem->Create(name, 0);
}


OpenFileId NuestroFilesys::nuestraOpen(char *name) {
	int ultimoId = 3;
	Iterator<OpenFileData*>* iter = openFiles->GetIterator();
	while(iter->HasNext()){
		OpenFileData* openFileData = iter->Next();
		if(strcmp(name, openFileData->name) == 0){
			return openFileData->id;
		}
		ultimoId = openFileData->id;
	}
	OpenFile *openFile = fileSystem->Open(name);
	OpenFileData *openFileData = new OpenFileData;
	openFileData->name = new char(100);
	strcpy(openFileData->name, name);
	openFileData->id = ++ultimoId;
	openFileData->openFile = openFile;
	openFiles->Append(openFileData);
	return openFileData->id;
}

void NuestroFilesys::nuestraClose(OpenFileId id) {
	Iterator<OpenFileData*>* iter = openFiles->GetIterator();

	OpenFileData* openFileData = iter->Next();
	//si es el primer elemento de la lista lo borro con el metodo remove de lista
	if(iter->HasNext() && openFileData->id == id){
		openFiles->Remove();
	}
	else {
		//sino es el primer elementos, busco cual es el elemento con el iterador y lo borro
		while(iter->HasNext()){
			openFileData = iter->Next();
			if(openFileData->id == id){
				iter->Remove();
			}
		}
	}
}

int NuestroFilesys::nuestraRead(char *buffer, int size, OpenFileId id) {
	if(id == ConsoleInput) {
		int i = 0;
		while(i<size){
			buffer[i] = console->GetChar();
			i++;
		}
		return size;
	}
	Iterator<OpenFileData*>* iter = openFiles->GetIterator();
	OpenFileData* openFileData;
	while(iter->HasNext()){
		openFileData = iter->Next();
		if(openFileData->id == id){
			return openFileData->openFile->Read(buffer, size);
		}
	}
	printf("El identificador de archivo %d no corresponde a un archivo abierto.\n", id);
	ASSERT(false);
}

void NuestroFilesys::nuestraWrite(char *buffer, int size, OpenFileId id) {
	if(id == ConsoleOutput) {
		int i = 0;
		while(i<size){
			console->PutChar(buffer[i]);
			i++;
		}
		return;
	}
	Iterator<OpenFileData*>* iter = openFiles->GetIterator();
	OpenFileData* openFileData;
	while(iter->HasNext()){
		openFileData = iter->Next();
		if(openFileData->id == id){
			openFileData->openFile->Write(buffer, size);
			return;
		}
	}
	printf("El identificador de archivo %d no corresponde a un archivo abierto.\n", id);
	ASSERT(false);
}

typedef struct {
	SpaceId key;
	AddrSpace *value;
} SpaceData;

List<SpaceData*>* spaceList;
int nextSpaceId=1;

void runInChildThread(void*);

SpaceId nuestraExec(char *filename) {

	static Thread* newThread;
	static AddrSpace* space;
	SpaceData* spaceData;

	OpenFile *executable = fileSystem->Open(filename);

	if (executable == NULL) {
		printf("Unable to open file %s\n", filename);
		return 0;
	}

	space = new AddrSpace(executable);
	delete executable;			// close file

	if (spaceList == NULL) {
    	spaceList = new List<SpaceData*>;
    }

    spaceData = new SpaceData;

    newThread = new Thread (filename);

    spaceData->key = newThread->getId();
	spaceData->value = space;
	spaceList->Append(spaceData);

	newThread->Fork(runInChildThread, (void *)space);

    return spaceData->key;
}

void runInChildThread(void* space) {

	printf("Aca deberia aparecer el nombre del arhivo a ejecutar: %s\n",currentThread->getName());

    currentThread->space = (AddrSpace *) space;

    currentThread->space->InitRegisters();		// set the initial register values
    currentThread->space->RestoreState();		// load page table register

    machine->Run();			// jump to the user progam
    printf("La funcion startProcess llego al assert\n");
    ASSERT(false);
}


void nuestraExit(int status) {

	Iterator<SpaceData*>* iter = spaceList->GetIterator();

	SpaceData* spaceData = iter->Next();
	//si es el primer elemento de la lista lo borro con el metodo remove de lista
	if(iter->HasNext() && spaceData->key == currentThread->getId()){
		spaceList->Remove();
	}
	else {
		//sino es el primer elementos, busco cual es el elemento con el iterador y lo borro
		while(iter->HasNext()){
			spaceData = iter->Next();
			if(spaceData->key == currentThread->getId()){
				iter->Remove();
			}
		}
	}

	currentThread->Finish();
}
