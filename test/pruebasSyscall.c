#include "syscall.h"

void prueba1();
void prueba2();
void prueba3(int);

int main()
{
//	prueba2();
//	prueba3(5);
	pruebaClose();
    return 0;
}

// Escribe "ABC" a la consola.
void prueba1(){
	char algoParaEscribir[100];
	algoParaEscribir[0] = 'A';
	algoParaEscribir[1] = 'B';
	algoParaEscribir[2] = 'C';
	Write(algoParaEscribir, 3, ConsoleOutput);
}

// Crea y abre un archivo y escribe "ABC" en el archivo abierto
void prueba2(){
	int id;
	char algoParaEscribir[100];
	Create("prueba2");
	id = (int) Open("prueba2");
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
	Create("prueba3");
	id = (int) Open("prueba3");
	Write(buffer, size, id);
}

// Crea, abre el archivo y luego lo cierra
void pruebaClose(){
	int id;
	Create("prueba4");
	id = (int) Open("prueba4");
	Close(id);
}
