#include "syscall.h"

int main()
{
	int id;
	char algoParaEscribir[100];
    Create("Ema");
    id = (int) Open("Ema");
    algoParaEscribir[0] = 'R';
    algoParaEscribir[1] = 'C';
    algoParaEscribir[2] = 'O';
    Write(algoParaEscribir, 3, id);
    return 0;
}
