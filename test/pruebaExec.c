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
	//No entiendo por que NO anda de esta forma!! (ver linea 267 exception.cc)
	//~ char algoParaEscribir[3][10];
	//~ algoParaEscribir[0][0]='h';
	//~ algoParaEscribir[0][1]='o';
	//~ algoParaEscribir[0][2]='l';
	//~ algoParaEscribir[0][3]='a';
	//~ algoParaEscribir[0][4]='\0';
	//~ algoParaEscribir[1][0]='m';
	//~ algoParaEscribir[1][1]='u';
	//~ algoParaEscribir[1][2]='n';
	//~ algoParaEscribir[1][3]='d';
	//~ algoParaEscribir[1][4]='\0';
	//~ algoParaEscribir[1][5]='a';
	//~ algoParaEscribir[2][0]='a';
	//~ algoParaEscribir[2][1]='c';
	//~ algoParaEscribir[2][2]='\0';
	//~ return Exec("./test/masPruebasSyscall",3,algoParaEscribir);
	
	//de esta forma anda bien!!
	char algoParaEscribir[16];
	//~ algoParaEscribir[0]='h';
	//~ algoParaEscribir[1]='o';
	//~ algoParaEscribir[2]='l';
	//~ algoParaEscribir[3]='a';
	//~ algoParaEscribir[4]='\0';
	//~ algoParaEscribir[5]='m';
	//~ algoParaEscribir[6]='u';
	//~ algoParaEscribir[7]='n';
	//~ algoParaEscribir[8]='d';
	//~ algoParaEscribir[9]='o';
	//~ algoParaEscribir[10]='\0';
	//~ algoParaEscribir[11]='m';
	//~ algoParaEscribir[12]='u';
	//~ algoParaEscribir[13]='n';
	//~ algoParaEscribir[14]='d';
	//~ algoParaEscribir[15]='o';
	//~ algoParaEscribir[16]='\0';
	//~ *algoParaEscribir = "Hola\0Mundo\0";
	
	//~ return Exec("./test/masPruebasSyscall",3,"Hola\0Mundo\0Mundo\0");
	//~ return Exec("./test/cp",2,"test/recursos/hijo\0test/recursos/hijoCopia\0");
	return Exec("test/cp",2,"Pepe\0hijoCopia\0");
	
	//~ char *argv[2];
	//~ argv[0] = "hola";
	//~ argv[1] = "mundo";
	//~ argv[2] = (char *) 0;
	//~ return Exec("./test/masPruebasSyscall",3,argv);
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
