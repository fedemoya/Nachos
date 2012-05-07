/*
 * nuestraSyscallImpl.h
 *
 *  Created on: 05/05/2012
 *
 *  Interfaz de nuestras implementaciones de las llamadas al sistema.
 *
 */

#ifndef NUESTRASYSCALLIMPL_H_
#define NUESTRASYSCALLIMPL_H_

#include "filesys.h"
#include "list.h"

typedef int OpenFileId;

typedef struct {
	char* name;
	OpenFileId id;
	OpenFile *openFile;
} OpenFileData;

class NuestroFilesys {
	public:
		NuestroFilesys();
		~NuestroFilesys();

		void nuestraCreate(char *name);
		OpenFileId nuestraOpen(char *name);
		int nuestraRead(char *buffer, int size, OpenFileId id);

	private:
		List<OpenFileData*>* openFiles;
};

#endif /* NUESTRASYSCALLIMPL_H_ */
