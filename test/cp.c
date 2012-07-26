#include "syscall.h"

#define EOF -1

int longitud(char *s);
int escribirEnConsola(char *s);
int strcmp(const char* strA, const char* strB);
int leerArchivo(OpenFileId id, char*buf);

int main(int argc, char **argv) {
	
	OpenFileId openFileIdOrigen, openFileIdDestino;
	int longFile;
	char* fileOrigen,*fileDestino, buffer[100];
	escribirEnConsola("Estamos en cp\n");
	if (argc !=2) {
		escribirEnConsola("Argumentos incorrectos\n");
		Exit(0);
	} else {
		fileOrigen = argv[0];
		fileDestino = argv[1];
		
		escribirEnConsola("Origen\n");
		escribirEnConsola(fileOrigen);
		escribirEnConsola("\nDestino\n");
		escribirEnConsola(fileDestino);		
		
		openFileIdOrigen = Open(fileOrigen);	
		
		if(openFileIdOrigen < 0 ) {
			escribirEnConsola("No existe el archivo: ");
			escribirEnConsola(fileOrigen);
			escribirEnConsola("\n");
			Exit(0);
		}
		if(strcmp(fileOrigen, fileDestino) == 0) {//comprobamos que el origen sea distinto del destino
			escribirEnConsola("El origen es igual al destino\n");
			Exit(0);
		}	
		
		longFile = leerArchivo(openFileIdOrigen,buffer);
		escribirEnConsola("En el buffer esta:\n");	
		escribirEnConsola(buffer);
		escribirEnConsola("LLego aca\n");
		if (longFile > 0) {				
			Create(fileDestino);
			openFileIdDestino = Open(fileDestino);																				
			if(openFileIdDestino > 0) { //no hubo error
				Write(buffer, longFile, openFileIdDestino);
				Close(openFileIdDestino);	
			} else {
				escribirEnConsola("No se pudo crear el archivo:");
				escribirEnConsola(fileDestino);
				escribirEnConsola("\n");
				Exit(0);
			}							
		} 	
			
		Close(openFileIdOrigen);	
		Exit(0);
	} 
}


int leerArchivo(OpenFileId id, char*buf)  {
	int cont=0;
	Read(buf+cont, 1 , id );
	while (*(buf+cont) != EOF){
		cont++;
		Read(buf+cont, 1 , id );
		if (cont>2) break;
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

int strcmp(const char* strA, const char* strB) {
	do {
		if (*strA < *strB)
			return -1;
		if (*strA > *strB)
			return 1;
	} while (*(strA++) != '\0' && *(strB++) != '\0');

	return 0;
}
