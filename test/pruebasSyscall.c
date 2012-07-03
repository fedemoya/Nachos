#include "syscall.h"

void prueba1();
void prueba2();
void prueba3(int);

int main()
{
	prueba1();
    Exit(2);
}

// Escribe "ABC" a la consola.
void prueba1(){
	char algoParaEscribir[100];
	algoParaEscribir[0] = 'E';
	algoParaEscribir[1] = 'F';
	algoParaEscribir[2] = 'G';
	Write(algoParaEscribir, 3, ConsoleOutput);
}

// Crea y abre un archivo y escribe "ABC" en el archivo abierto
void prueba2(){
	int id;
	char algoParaEscribir[100];
	Create("./test/recursos/hijo");
	id = (int) Open("./test/recursos/hijo");
	algoParaEscribir[0] = 'A';
	algoParaEscribir[1] = 'B';
	algoParaEscribir[2] = 'C';
	Write(algoParaEscribir, 3, id);
}

// Lee de la consola y escribe un archivo.
void prueba3(int size){
	char buffer[100];
	int id;
	Read(buffer, size, ConsoleInput); // Lee size caracteres de la consola.
	Create("./test/recursos/prueba3");
	id = (int) Open("./test/recursos/prueba3");
	Write(buffer, size, id);
}

// Crea, abre el archivo y luego lo cierra
void pruebaClose(){
	int ida,idb;
	char algoParaEscribir[100];

	Create("./test/recursos/prueba4a");
	Create("./test/recursos/prueba4b");

	ida = (int) Open("./test/recursos/prueba4a");
	idb = (int) Open("./test/recursos/prueba4b");

	algoParaEscribir[0] = 'A';
	algoParaEscribir[1] = 'B';
	algoParaEscribir[2] = 'C';

	Write(algoParaEscribir, 3, ida);
	Close(ida);Close(idb);
	Write(algoParaEscribir, 3, ida);
}

