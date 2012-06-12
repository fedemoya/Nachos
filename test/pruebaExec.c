#include "syscall.h"

int pruebaExec();
void pruebaJoin(SpaceId id);
void prueba1();
void prueba2();

int main()
{
	SpaceId id;
	id = pruebaExec();
	pruebaJoin(id);
	prueba2();
    Exit(1);
}

int pruebaExec(){
	return Exec("./test/pruebasSyscall");
}

void pruebaJoin(SpaceId id){
	Join(id);
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
