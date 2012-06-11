#include "syscall.h"

void pruebaExec();
void prueba1();
void prueba2();

int main()
{
	prueba2();
	pruebaExec();
    return 0;
}

void pruebaExec(){
	Exec("./test/pruebasSyscall");
}


void prueba1(){
	char algoParaEscribir[100];
	algoParaEscribir[0] = 'X';
	algoParaEscribir[1] = 'Y';
	algoParaEscribir[2] = 'Z';
	Write(algoParaEscribir, 3, ConsoleOutput);
}

void prueba2(){
	int id;
	char algoParaEscribir[100];
	Create("./test/recursos/padre");
	id = (int) Open("./test/recursos/padre");
	algoParaEscribir[0] = 'X';
	algoParaEscribir[1] = 'Y';
	algoParaEscribir[2] = 'Z';
	Write(algoParaEscribir, 3, id);
}
