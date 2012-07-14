#include "syscall.h"

#define EOF -1

int longitud(char *s);
int escribirEnConsola(char *s);
int strCopy(char *src,char *dest);
int leerArchivo(OpenFileId id, char*buf);

int main(int argc, char **argv) {
	
	OpenFileId openFileIdOrigen;
	int i,contBufSalida, contBufError;
	char bufferSalida[100],buferError[100];
	escribirEnConsola("Estamos en cat\n");
	
	if (argc <=0) {
		escribirEnConsola("Argumentos incorrectos\n");
		Exit(0);
	} else {
		bufferSalida[0]='\0';
		buferError[0]='\0';
		contBufSalida = 0;
		contBufError = 0;
		for(i=0;i<argc;i++) {
			openFileIdOrigen = Open(argv[i]);	
			if(openFileIdOrigen < 0 ) {
				contBufError += strCopy("No existe el archivo: ",buferError+contBufError);
				buferError[contBufError]='\n';
				contBufError++;
			} else {
				contBufSalida += leerArchivo(openFileIdOrigen,bufferSalida+contBufSalida);
				Close(openFileIdOrigen);	
			}
		}
	
		escribirEnConsola(bufferSalida);
		escribirEnConsola(buferError);
		escribirEnConsola("\n");
		Exit(0);
	} 
}

//~ int leerArchivo(OpenFileId id, char*buf)  {
	//~ int cont=0, tamanio;
	//~ tamanio = LongFile(id);  llamada a sistema que falta implementar!
	//~ tamanio = 3;
	//~ while(cont< tamanio){
		//~ Read(buf+cont, 1 , id );
		//~ cont++;
	//~ }
	//~ return cont;
//~ }

int leerArchivo(OpenFileId id, char*buf)  {
	int cont=0;
	Read(buf+cont, 1 , id );
	while (*(buf+cont) != EOF){
		cont++;
		Read(buf+cont, 1 , id );
		Write(buf+cont, 1 , ConsoleOutput );
	}
	return cont;
}

int longitudString(char *s) {
	int i;
	for (i = 0; s[i] != '\0'; i++);
	return i;
}

int escribirEnConsola(char *s) {
	int l = longitudString(s);
	Write(s, l, ConsoleOutput);
	return l;
}

int strCopy(char *src,char *dest) {
	int cont = 0;
	while(src!='\0') {
		dest[cont] = src[cont];
		cont++;
	}
	return cont;
}
