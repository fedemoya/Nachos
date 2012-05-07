/*
 * nustraSyscallImpl.cc
 *
 *  Created on: 05/05/2012
 */

#include "system.h"
#include "nuestraSyscallImpl.h"

NuestroFilesys::NuestroFilesys() {
	openFiles = new List<OpenFileData*>;
}

NuestroFilesys::~NuestroFilesys() {
	delete openFiles;
}

void NuestroFilesys::nuestraCreate(char *name) {
	fileSystem->Create(name, 0);
}


OpenFileId NuestroFilesys::nuestraOpen(char *name) {
	int ultimoId = 0;
	Iterator<OpenFileData*>* iter = openFiles->GetIterator();
	OpenFileData* openFileData;
	while(iter->HasNext()){
		openFileData = iter->Next();
		if(strcmp(name, openFileData->name) == 0){
			return openFileData->id;
		}
		ultimoId = openFileData->id;
	}
	OpenFile *openFile = fileSystem->Open(name);
	openFileData = new OpenFileData;
	openFileData->name = new char(100);
	strcpy(openFileData->name, name);
	openFileData->id = ultimoId++;
	openFileData->openFile = openFile;
	openFiles->Append(openFileData);
	return openFileData->id;
}

int NuestroFilesys::nuestraRead(char *buffer, int size, OpenFileId id) {
	Iterator<OpenFileData*>* iter = openFiles->GetIterator();
	OpenFileData* openFileData;
	while(iter->HasNext()){
		openFileData = iter->Next();
		if(openFileData->id == id){
			return openFileData->openFile->Read(buffer, size);
		}
	}
	// Error: el archivo no fue abierto.
}
