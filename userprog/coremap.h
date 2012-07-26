#define USE_LRU 1

//~ #include "../machine/machine.h"
//~ #include "../threads/thread.h"
#include "list.h"
//TODO: arreglar esta negrada!
const int NumPhysPages2 = 32;//32;
const int PageSize2 = 128;
class Thread;

typedef struct {
	Thread* hilo;//dueño de la pagina
	int virtAddr;//pagina virtual 
} FrameIPT;//Inverted Page Table Frame

class CoreMap {
	public:
    
    CoreMap();	
    ~CoreMap();
	int Find();
	#ifndef USE_LRU
	#else
	///////////////////////
    //para politica LRU
    void setearFrameReferenciado(int indPage) { 
		this->setearFilaEnUno(indPage);
		this->setearColumnaEnCero(indPage); 
	};
    ///////////////////////
    #endif
    
    private: 
    
    FrameIPT* tablaInvertida;
    #ifndef USE_LRU
	///////////////////////
    //para politica FIFO de paginacion
    List<int> *queuePages;
    ///////////////////////
    #else       
    ///////////////////////
    //para politica LRU
    int matrizLRU[NumPhysPages2][NumPhysPages2];//para politica LRU
    void setearFilaEnUno(int indFila);
    void setearColumnaEnCero(int indCol);
    int elegirFrameLRU();
    ///////////////////////
    #endif
}
