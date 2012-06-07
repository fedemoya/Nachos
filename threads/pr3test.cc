/*
 * messagesTest.cc
 *
 *  Created on: 24/04/2012
 *      Author: federico
 */

#include "copyright.h"
#include "system.h"
#include "synch.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// El hijo termina antes de que el padre llame a Join.
void boba1(void *arg){
	printf("boba1 con hilo: %s\n",currentThread->getName());
	for(int i=0; i<10000; i++);
}

// Cuando el padre llama a Join el hijo todavia esta vivo.
void boba2(void *arg){
	printf("boba2 con hilo: %s\n",currentThread->getName());
	currentThread->Yield();
	for(int i=0; i<10000; i++);
}

void JoinTest(){
	char* threadname = new char[100];
	sprintf(threadname, "Hijo1");

	Thread *childThread = new Thread(threadname,true,PrioridadMaxima);
	childThread->Fork(boba2, NULL);
 	currentThread->Join(childThread);
	sprintf(threadname, "Hijo2");
	childThread = new Thread(threadname,true,PrioridadMedia);
	childThread->Fork(boba1, NULL);
	currentThread->Join(childThread);
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//Caso de prueba 4 b Inversion de prioridades
//////////////////////////////////////////////////////////////////////////////////////////////////
int compartida = 0;

void boba3(void*lock){
	printf("Entra boba3, hilo: %s, prioridad %d \n",currentThread->getName(),currentThread->getPrioridad());
	Lock*castLock = (Lock*)lock;
	castLock->Acquire();
	//currentThread->Yield();
	compartida++;
	castLock->Release();
	printf("Sale boba3, hilo: %s valor compartida %d\n",currentThread->getName(),compartida);
}

void boba4(void*lock){
	printf("boba4/hilo: %s\n",currentThread->getName());
	for(int i=0;i<1000;i++) {
		//currentThread->Yield();
	}
}

void entraConLockTomado(void*lock){
	printf("Entra entraConLockTomado, hilo: %s, prioridad %d \n",currentThread->getName(),currentThread->getPrioridad());
	Lock*castLock = (Lock*)lock;
	//castLock->Acquire();
	//currentThread->Yield();
	compartida++;
	castLock->Release();
	printf("Sale entraConLockTomado, hilo: %s valor compartida %d\n",currentThread->getName(),compartida);
}

void lockear(void*lock){
	printf("Entra lockear %s \n",currentThread->getName());
	Lock*castLock = (Lock*)lock;
	castLock->Acquire();
	currentThread->Yield();
	entraConLockTomado(lock);	
}

void inversionPrioridades()
{
	char* lockName = new char[30];
	sprintf(lockName, "lockMain");
	/*--    Causa problema de inversion de prioridades     --*/
	//Lock* lockVariableCompartida = new Lock(lockName,PrioridadBaja); 
	
	/*--    Evita problema de inversion de prioridades     --*/
	Lock* lockVariableCompartida = new Lock(lockName,PrioridadMaxima); 
	
	char* name = new char[30];
	sprintf(name, "TBaja > PrioridadBaja");
	Thread *tBaja = new Thread(name,true,PrioridadBaja);
			
	char* name1 = new char[30];
	sprintf(name1, "T1 > PrioridadMedia");
	Thread *t1 = new Thread(name1,true,PrioridadMedia);
		
	char* name2 = new char[30];
	sprintf(name2, "T2 > PrioridadMedia");
	Thread *t2 = new Thread(name2,true,PrioridadMedia);
	
	
	char* name3 = new char[30];
	sprintf(name3, "talta > PrioridadMaxima");
	Thread *talta = new Thread(name3,true,PrioridadMaxima	);
	
	tBaja->Fork(lockear, lockVariableCompartida);
	currentThread->Yield();//asi lockea tBaja	
	t1->Fork(boba4, lockVariableCompartida);
	t2->Fork(boba4, lockVariableCompartida);	
	talta->Fork(boba3, lockVariableCompartida);
	printf("termina creacion hilos \n");
	//currentThread->Join(tBaja);				
	
}

Messages *Mensajes;

typedef struct {
	Port puerto;
	int msg;
} Argumentos;

void Sender(void* _args){
	Argumentos *args = (Argumentos *) _args;
	printf("Se intenta enviar el msg %d por el puerto %d\n", args->msg, args->puerto);
	Mensajes->Send(args->puerto, args->msg);
	printf("Sender: Se envio el mensaje %d en el puerto %d\n", args->msg, args->puerto);
}

void Receiver(void* _args){
	Argumentos *args = (Argumentos *) _args;
	int buf;
	printf("Se intenta recibir un msg por el puerto %d\n", args->puerto);
	Mensajes->Receive(args->puerto, &buf);
	printf("Receiver: Se recibio el mensaje %d en el puerto %d\n", buf, args->puerto);
}

void MessagesTest(){

	DEBUG('t', "Iniciando test de mensajes.");

	char* msgName = new char[100];
	sprintf(msgName, "Mensajes");
	char* threadname = new char[100];
	sprintf(threadname, "Hilo");

	Mensajes = new Messages(msgName);
	srand(time(NULL));

//	for (int i=0; i<=20; i++){
//		Argumentos *args = new Argumentos;
//		args->puerto = rand() % 5; // Devuelve valores entre 0 y 9.
//		Thread* newThread = new Thread (threadname);
//		if(i%2 == 0){
//			args->msg = i;
//			newThread->Fork (Sender, (void *)args);
//		} else {
//			newThread->Fork (Receiver, (void *)args);
//		}
//	}

	for (int i=1; i<=20; i++){
		Argumentos *args = new Argumentos;
		args->puerto = 1;
		Thread* newThread = new Thread (threadname);
		args->msg = i;
		newThread->Fork (Sender, (void *)args);
	}

	for (int i=1; i<=20; i++){
			Argumentos *args = new Argumentos;
			args->puerto = 1;
			Thread* newThread = new Thread (threadname);
			newThread->Fork (Receiver, (void *)args);
		}

}
