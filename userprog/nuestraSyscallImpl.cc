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

void NuestroFilesys::nuestraClose(char *name) {
//	Iterator<OpenFileData*>* iter = openFiles->GetIterator();
//	while(iter->HasNext()){
//		OpenFileData* openFileData = iter->Next();
//		if(strcmp(name, openFileData->name) == 0){
//			//Borrar el archivo de la lista de archivos abiertos.
//		}
//		ultimoId = openFileData->id;
//	}
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
