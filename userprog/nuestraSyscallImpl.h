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
#include "console.h"
#include "synch.h"
#include "list.h"

/* when an address space starts up, it has two open files, representing
 * keyboard input and display output (in UNIX terms, stdin and stdout).
 * Read and Write can be used directly on these, without first opening
 * the console device.
 */
#define ConsoleInput	0
#define ConsoleOutput	1

class SynchConsole {
  public:
    SynchConsole(const char *readFile, const char *writeFile);
    ~SynchConsole();

    void PutChar(char ch);
    char GetChar();

  private:
    Console *console;
    Lock *lock;
};

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
		void nuestraClose(char *name);
		int nuestraRead(char *buffer, int size, OpenFileId id);
		void nuestraWrite(char *buffer, int size, OpenFileId id);

	private:
		List<OpenFileData*>* openFiles;
		SynchConsole *console;
};

#endif /* NUESTRASYSCALLIMPL_H_ */
