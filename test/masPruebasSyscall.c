#include "syscall.h"

int main(int argc,char**argv)
{
	int i;
	for(i=0;i<argc;i++)
		Write(argv[i], 4, ConsoleOutput);
		
	//~ char algoParaEscribir[100];
	//~ char buffer[100];
	//~ int id0, id1, id2;
	//~ const int size = 12;
//~ 
	//~ id0 = Open("./test/recursos/readme");
	//~ Read(buffer, size, id0);
//~ 
	//~ Create("./test/recursos/writeme");
	//~ id1 = Open("./test/recursos/writeme");
	//~ Write(buffer, size, id1);
//~ 
	//~ Create("./test/recursos/archivo");
	//~ id2 = (int) Open("./test/recursos/archivo");
	//~ algoParaEscribir[0] = 'D';
	//~ algoParaEscribir[1] = 'E';
	//~ algoParaEscribir[2] = 'F';
	//~ Write(algoParaEscribir, 3, id2);
//~ 
	//~ Close(id0);
	//~ Close(id1);
	//~ Close(id2);

    Exit(2);
}

