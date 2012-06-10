#include "syscall.h"

void pruebaWrite(){
	int id;
	char algoParaEscribir[100];
	Create("hijo");
	id = (int) Open("hijo");
	algoParaEscribir[0] = 'A';
	algoParaEscribir[1] = 'B';
	algoParaEscribir[2] = 'C';
	Write(algoParaEscribir, 3, id);
}

int main() {
	pruebaWrite();
	return 0;
}


