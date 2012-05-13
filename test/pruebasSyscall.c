#include "syscall.h"

int main()
{
	int id;
	char algoParaEscribir[100];
    Create("archivo");
    id = (int) Open("archivo");
    algoParaEscribir[0] = 'R';
    algoParaEscribir[1] = 'C';
    algoParaEscribir[2] = '\0';
    Write(algoParaEscribir, 3, id);
    return 0;
}
