#include "coremap.h"
#include "system.h"
#include <math.h>

CoreMap::CoreMap() {
	tablaInvertida = (FrameIPT*)malloc(sizeof(FrameIPT)*NumPhysPages);
	#ifndef USE_LRU
		queuePages = new List<int>;
    #else
		for(int i;i<NumPhysPages;i++) 
			for(int j;j<NumPhysPages;j++) 
				matrizLRU[i][j] = 0;
    #endif
}


CoreMap::~CoreMap() {
	delete [] tablaInvertida;
	
	#ifndef USE_LRU
		delete queuePages;
    #else
    #endif
}

int CoreMap::Find(int virtualAddres) {
	int nextPhysPage = machine->bitMapPagMemAdmin->Find();
	if (nextPhysPage < 0) {
		int pageFrameSwapear;
		//aca deberia estar la decision de pagina a sacar!
		#ifndef USE_LRU
		ASSERT(!queuePages->IsEmpty());
		pageFrameSwapear = queuePages->Remove();
		DEBUG('y', "Swapeo FIFO page swap elegida %d \n", pageFrameSwapear);	
		#else
		pageFrameSwapear = elegirFrameLRU();
		DEBUG('y', "Swapeo LRU page swap elegida %d \n", pageFrameSwapear);
		this->setearFrameReferenciado(pageFrameSwapear);
		#endif
		Thread* hiloProp = tablaInvertida[pageFrameSwapear].hilo;
		bool write = hiloProp->space->writeToSwap(&(machine->mainMemory[pageFrameSwapear * PageSize]) ,tablaInvertida[pageFrameSwapear].virtAddr);
		if (!write)
			DEBUG('y', "Se cago el write del swap!!!!!!!!!!!!!!!", pageFrameSwapear);	
		
		nextPhysPage = pageFrameSwapear;
		
	} else {
		DEBUG('y', "bitmap page libre %d \n", nextPhysPage);	
	}
	
	//~ FrameIPT nuevoFrame = tablaInvertida[nextPhysPage];
	//~ if (nuevoFrame == NULL) {
		//~ nuevoFrame = new FrameIPT;
	//~ } else {
		//~ delete tablaInvertida[nextPhysPage]	;
	//~ }
	tablaInvertida[nextPhysPage].hilo = currentThread;
	tablaInvertida[nextPhysPage].virtAddr = virtualAddres;
	//~ tablaInvertida[nextPhysPage] = nuevoFrame;
	
	#ifndef USE_LRU
	queuePages->Append(nextPhysPage);
    #else
    //aca creo que no se hace nada, ya que al volver a ejecutar la instruccion se referenciara 
    //la pagina recien colocada en memoria quedando con bits en 1 y de esa forma (no hereda el uso de la recien sacada)
    #endif
	
	
	return nextPhysPage;
}

 ///////////////////////
 //para politica FIFO de paginacion
 
 #ifdef USE_LRU
void 
CoreMap::setearFilaEnUno(int indFila) {
	ASSERT(indFila < NumPhysPages);
	for(int i=0;i<NumPhysPages;i++) 
		matrizLRU[indFila][i] = 1;
};
void 
CoreMap::setearColumnaEnCero(int indCol){
	ASSERT(indCol < NumPhysPages);
	for(int i=0;i<NumPhysPages;i++) 
		matrizLRU[i][indCol] = 0;
};  

void
CoreMap::printMatrizLRU() {
	int nuevoValor;
	for(int i=0;i<NumPhysPages;i++) { 
		nuevoValor = calcularEnteroPosicion(i);
		DEBUG('k', "**>> fila %d -- valor %d \n", i,nuevoValor);
	}
}

int 
CoreMap::elegirFrameLRU() {
	int minimoValor;
	int pageLRU=0;
	minimoValor = calcularEnteroPosicion(0);
	for(int i=1;i<NumPhysPages;i++)  {
		if (calcularEnteroPosicion(i) < minimoValor) 
			pageLRU=i;
	}
	DEBUG('y', "frame matriz LRU elegido %d (con valor igual a %d) \n", pageLRU,calcularEnteroPosicion(pageLRU));
	return pageLRU;
}

int CoreMap::calcularEnteroPosicion(int indFila) {
	ASSERT(indFila < NumPhysPages);
	int entero = 0;
	for(int i=0;i<NumPhysPages;i++) {
		//entero += matrizLRU[indFila][i]*(int)pow(2,i);
		int potencia = matrizLRU[indFila][i];
		entero += (potencia<<i);
	}
	return entero;
}
#endif

void CoreMap::Clear(int which) {
    if ((which >= 0 && which < NumPhysPages))
        machine->bitMapPagMemAdmin->Clear(which);
}
 


