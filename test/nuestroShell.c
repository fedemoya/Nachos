#include "syscall.h"
    
#define MAX_ARGUMENTOS 20
#define MAX_LINEA 100    

int extraerNombrePrograma(char* buf,char* nombre) {
	int i = 0;
	while(buf[i] != '\n' && buf[i] != ' '){
		nombre[i] = buf[i];
		i++;	
	}
	return i;
}
	
int listarArgumentos(char* buf) {
	int i = 0;
	int argc = 0;
	while(buf[i] != '\n'){
		if (buf[i] == ' ') {
			buf[i] = '\0'; 
			argc++;
		}	
		i++;	
	}
	buf[i] = '\0';
	return argc;//uno mas por el nombre del programa
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


void main()
{
	SpaceId nuevoProceso;
    OpenFileId input = ConsoleInput;
    OpenFileId output = ConsoleOutput;
    char ch, linea[MAX_LINEA],pesos;
    int ind_linea,h, longNombreProg,indArg;
	char *argv[MAX_ARGUMENTOS];
	char nombrePrograma[MAX_LINEA];
	int argc = 0;
	
	for(;;) {
		escribirEnConsola("Hola (otra vez)\n");
		escribirEnConsola("$");
		ind_linea = 0;			
		longNombreProg=0;
		do {
			Read(&linea[ind_linea],1,ConsoleInput);
		} while (linea[ind_linea++] != '\n');
		
		if (ind_linea==2 && linea[ind_linea-2] == 'q') {
			escribirEnConsola("Chau!\n");
			Halt();//nos vamos
		}
		
		longNombreProg = extraerNombrePrograma(linea,nombrePrograma);
		argc= 0;
		if (ind_linea>longNombreProg) {//puede que no haya argmentos
			argc = listarArgumentos(linea + longNombreProg);//dejamos afuera el nombre del programa de los argumentos
			escribirEnConsola("Los argumentos son\n");
			indArg=longNombreProg+1;
			Write(linea + longNombreProg,ind_linea-longNombreProg,ConsoleOutput); 
			escribirEnConsola("Los argumentos son\n");
			for(h=0;h<argc;h++){
				escribirEnConsola("Vamos uno por uno\n");
				escribirEnConsola(linea +indArg );
				indArg+=longitudString(linea + indArg)+1;
			}
		} else {
			linea[longNombreProg] = '\0';
		}
		nuevoProceso = Exec(nombrePrograma,argc,linea + longNombreProg+1);		
		if (nuevoProceso>0)
			Join(nuevoProceso);
		//Limpiamos las cadenas	
		for(h=0;h<ind_linea;h++)
			linea[h]='\0';
		for(h=0;h<longNombreProg;h++)
			nombrePrograma[h]='\0';
			
	}
 
}

//~ int listarArgumentos(char* buf,char** args) {
	//~ int i = 0;
	//~ int argc = 0;
	//~ int nuevoArg = 1;
	//~ while(buf[i] != '\n'){
		//~ if (buf[i] == ' ') {
			//~ buf[i] = '\0'; //no alocamos espacio para los argumentos de args, simplemente,
						   //~ // lo hacemos apuntar a donde empiezan en buf y pisamos los espacios por '\0'
			//~ argc++;
			//~ nuevoArg = 1;
			//~ i++;
			//~ continue;
		//~ } else if (nuevoArg) {
			//~ args[argc] = buf+i;
			//~ nuevoArg = 0;
		//~ }	
		//~ i++;	
	//~ }
	//~ return ++argc;//uno mas por el nombre del programa
//~ }
