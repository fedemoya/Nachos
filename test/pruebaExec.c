#include "syscall.h"

int pruebaExec();
int pruebaExec2();
void pruebaJoin(SpaceId id);
void prueba1();
void prueba2();

int main()
{
	SpaceId id;
	id = pruebaExec();
	pruebaExec2();
//	pruebaJoin(id);
//	prueba2();
    Exit(1);
}

int pruebaExec(){
	return Exec("./test/pruebasSyscall");
}

int pruebaExec2(){
	return Exec("./test/masPruebasSyscall");
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
