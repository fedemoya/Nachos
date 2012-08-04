#ifndef COREMAP_H
#define COREMAP_H

//~ #include "system.h"
//~ #include "../threads/thread.h"
//~ #ifdef USER_PROGRAM
#include "list.h"
#include "machine.h"
#include "thread.h"
//~ #endif
//TODO: arreglar esta negrada!
//~ const int NumPhysPages2 = 32;//32;
//~ const int PageSize2 = 128;
//~ class Thread;
#define USE_LRU 1


typedef struct {
	Thread* hilo;//dueño de la pagina
	int virtAddr;//pagina virtual 
} FrameIPT;//Inverted Page Table Frame

class CoreMap {
	public:
        
        CoreMap();	
        ~CoreMap();
        int Find(int i);
        void Clear(int page);
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
        int matrizLRU[NumPhysPages][NumPhysPages];//para politica LRU
        void setearFilaEnUno(int indFila);
        void setearColumnaEnCero(int indCol);
        int elegirFrameLRU();
        int calcularEnteroPosicion(int indFila);
        ///////////////////////
        #endif
};

#endif // COREMAP_H
