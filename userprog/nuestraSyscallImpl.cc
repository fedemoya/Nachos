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
	Iterator<OpenFileData*>* iter = openFiles->GetIterator();
	OpenFileData* openFileData;
	while(iter->HasNext()){
		openFileData = iter->Next();
		if(openFileData->id == id){
			openFileData->openFile->Write(buffer, size);
		}
	}
	printf("El identificador de archivo %d no corresponde a un archivo abierto.\n", id);
	ASSERT(false);
}
