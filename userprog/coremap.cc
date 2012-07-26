#include "coremap.h"
void
CoreMap::CoreMap() {
	tablaInvertida = malloc(sizeof(struct FrameIPT)*NumPhysPages2);
	#ifndef USE_LRU
		queuePages = new List<int>;
    #else
		for(int i;i<NumPhysPages2;i++) 
			for(int j;j<NumPhysPages2;j++) 
				matrizLRU[i][j] = 0;
    #endif
}


void
CoreMap::~CoreMap() {
	delete [] tablaInvertida;
	
	#ifndef USE_LRU
		delete queuePages;
    #else
    #endif
}

int Find(int virtualAddres) {
	int nextPhysPage = machine->bitMapPagMemAdmin->Find();
	if (nextPhysPage < 0) {
		int pageFrameSwapear;
		//aca deberia estar la decision de pagina a sacar!
		#ifndef USE_LRU
		ASSERT(!queuePages->IsEmpty());
		pageFrameSwapear = queuePages->Remove();
		#else
		pageFrameSwapear = elegirFrameLRU();
		#endif
		Thread* hiloProp = tablaInvertida[nextPhysPage].hilo;
		hiloProp->space->writeToSwap(&(machine->mainMemory[pageFrameSwapear * PageSize2]) ,tablaInvertida[nextPhysPage].virtAddr);
	} 
	
	FrameIPT nuevoFrame = tablaInvertida[nextPhysPage];
	if (nuevoFrame == NULL) {
		nuevoFrame = new FrameIPT;
	} else {
		delete tablaInvertida[nextPhysPage]	;
	}
	nuevoFrame.hilo = currentThread;
	nuevoFrame.virtAddr = virtualAddres;
	tablaInvertida[nextPhysPage] = nuevoFrame;
	
	#ifndef USE_LRU
	queuePages->Append(nextPhysPage);
    #else
    //aca creo que no se hace nada, ya que al volver a ejecutar la instruccion se referenciara 
    //la pagina recien colocada en memoria quedando con bits en 1 y de esa forma (no hereda el uso de la recien sacada)
    #endif
	
	
	return nextPhysPage;
}
void 
CoreMap::setearFilaEnUno(int indFila) {
	ASSERT(indFila < NumPhysPages2);
	for(int i=0;i<NumPhysPages2;i++) 
		matrizLRU[indFila][i] = 0;
};
void 
CoreMap::setearColumnaEnCero(int indCol){
	ASSERT(indFila < NumPhysPages2);
	for(int i=0;i<NumPhysPages2;i++) 
		matrizLRU[i][indCol] = 0;
};  


int 
CoreMap::elegirFrameLRU() {
	int minimoValor = 0,nuevoValor=0;
	int pageLRU=0;
	for(int i=0;i<NumPhysPages2;i++)  {
		nuevoValor = calcularEnteroPosicion(i);
		if (nuevoValor < minimoValor) {
			minimoValor = nuevoValor;
			pageLRU=i;
		}
	}
	return pageLRU;
}

int calcularEnteroPosicion(int indFila) {
	ASSERT(indFila < NumPhysPages2);
	int entero = 0;
	for(int i=0;i<NumPhysPages2;i++) 
		entero += matrizLRU[indFila][i]*2^i;
	return i;
}
 


