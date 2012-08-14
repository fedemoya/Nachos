#include "syscall.h"

#define NULL_32 '0'
#define NULL_64 0

void cat(OpenFileId id);

int main() {
	OpenFileId id;
	id = Open("pep");
	cat(id);
	Exit(0);
}

void cat(OpenFileId id) {
	//~ char *c;
	//~ Read(c, 1, id);
	//~ while (*c != NULL_32 && *c != NULL_64) {
		//~ Write(c, 1, ConsoleOutput);
		//~ Read(c, 1, id);
	//~ }
	char *c;
	int resultado;
	while (resultado = Read(c, 1 , id )){	
		if(resultado == 0) {
			Write(c, 1 , ConsoleOutput );
			break;
		}
		if(resultado == 0) {
			Write(c, 1 , ConsoleOutput );
			break;
		}
		if(resultado == 0) {
			Write(c, 1 , ConsoleOutput );
			break;
		}
		Write(c, 1 , ConsoleOutput );		
	}
}


int leerArchivo(OpenFileId id, char*buf)  {
	char *c;
	int cont=0;
	while (1){	
		if(Read(buf+cont, 1 , id ) == 0) {
			Write(buf+cont, 1 , ConsoleOutput );
			break;
		}
		Write(buf+cont, 1 , ConsoleOutput );		
		cont++;
	}
	return cont;
}
