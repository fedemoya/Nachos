/*
 * nustraSyscallImpl.cc
 *
 *  Created on: 05/05/2012
 */

//#include "../threads/system.h"
#include "system.h"
#include "nuestraSyscallImpl.h"


void nuestraCreate(char *name) {
	fileSystem->Create(name, 0);
}
