#define USE_LRU 1

typedef struct {
	Thread* hilo;//dueño de la pagina
	int virtAddr;//pagina virtual 
} FrameIPT;//Inverted Page Table Frame

Clase CoreMap {
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
    int matrizLRU[NumPhysPages][NumPhysPages];//para politica LRU
    void setearFilaEnUno(int indFila);
    void setearColumnaEnCero(int indCol);
    int elegirFrameLRU();
    ///////////////////////
    #endif
}
