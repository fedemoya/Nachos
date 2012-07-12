#include "syscall.h"

int main(int argc,char**argv)
{
	int i,j;
	//~ char algoParaEscribir[100];
	//~ algoParaEscribir[0] = 'X';
	//~ algoParaEscribir[1] = 'Y';
	//~ algoParaEscribir[2] = 'Z';
	//~ Write(algoParaEscribir, 3, ConsoleOutput);
	char algoParaEscribir[100];
	//~ algoParaEscribir[0] =  (char)('9' - '0' + (char)(argc & 0xFF)); 
	//~ algoParaEscribir[1] = '\n';
	//~ algoParaEscribir[2] =  '9' - (char)(argc & 0xff) + '0';
	//~ algoParaEscribir[3] =  '9' - (char)(argc>>8 & 0xff) + '0';
	//~ algoParaEscribir[4] =  '9' - (char)(argc>>16 & 0xff) + '0';
	//~ algoParaEscribir[5] =  '9' - (char)(argc>>24 & 0xff) + '0';
	//~ algoParaEscribir[6] = '\n';
	//~ Write(algoParaEscribir, 7, ConsoleOutput);
	
	//~ Write((char)argc, 1, ConsoleOutput);
	//~ Write((char)argv, 1, ConsoleOutput);
	
	
	//~ for(i=0;i<argc;i++){
		//~ Write(argv[i], 4, ConsoleOutput);
	//~ }
	for(i=0;i<argc;i++){
		int j = 0;
		while (argv[i][j] !='\0'){
			algoParaEscribir[j] = argv[i][j];
			j++;
		}
		algoParaEscribir[j] = '\n';
		Write(algoParaEscribir, ++j, ConsoleOutput);
	}
		
		
		//~ 
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

