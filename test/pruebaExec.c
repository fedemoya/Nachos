#include "syscall.h"

int pruebaExec();
int pruebaExec2();
void pruebaJoin(SpaceId id);
void prueba1();
void prueba2();

int main()
{
	SpaceId id;
	id = pruebaExec2();
//	pruebaJoin(id);
//	prueba2();
    Exit(1);
}

//~ int pruebaExec(){
	//~ return Exec("./test/pruebasSyscall");
//~ }

int pruebaExec2(){
	char algoParaEscribir[2][5];
	algoParaEscribir[0][0]='h';
	algoParaEscribir[0][1]='o';
	algoParaEscribir[0][2]='l';
	algoParaEscribir[0][3]='a';
	algoParaEscribir[0][4]='\0';
	algoParaEscribir[1][0]='m';
	algoParaEscribir[1][1]='u';
	algoParaEscribir[1][2]='n';
	algoParaEscribir[1][3]='d';
	algoParaEscribir[1][4]='\0';
	return Exec("./test/masPruebasSyscall",2,algoParaEscribir);
}

void pruebaJoin(SpaceId id){
	Join(id);
}

void prueba1(){
	char algoParaEscribir[100];
	algoParaEscribir[0] = 'X';
	algoParaEscribir[1] = 'Y';
	algoParaEscribir[2] = 'Z';
	Write(algoParaEscribir, 2, ConsoleOutput);
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
