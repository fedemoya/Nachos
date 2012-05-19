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
	printf("Entra boba3 con hilo: %s, prioridad %d \n",currentThread->getName(),currentThread->getPrioridad());
	Lock*castLock = (Lock*)lock;
	castLock->Acquire();
	currentThread->Yield();
	compartida++;
	printf("Sale boba3 con hilo: %s valor compartida %d\n",currentThread->getName(),compartida);
	castLock->Release();
}

void boba4(void*lock){
	printf("boba4/hilo: %s\n",currentThread->getName());
	for(int i=0;i<100;i++) {
		currentThread->Yield();
	}
}

void inversionPrioridades()
{
	char* lockName = new char[100];
	sprintf(lockName, "lockMain");
	Lock* lockVariableCompartida = new Lock(lockName,PrioridadMinima);
	
	char* threadname = new char[100];
	sprintf(threadname, "lockMain");
	sprintf(threadname, "Hijo PrioridadBaja");
	Thread *childThread = new Thread(threadname,true,PrioridadBaja);
	
	
	childThread->Fork(boba3, lockVariableCompartida);	
		
	for(int i=0;i<5;i++) {
		char* name = new char[100];
		sprintf(name, "PrioridadMedia hijo nro %d",i);
		Thread *newThread = new Thread(name,true,PrioridadMedia);
		newThread->Fork(boba4, NULL);	
		currentThread->Join(newThread);
	}
	
	
	
	char* name2 = new char[100];
	sprintf(name2, "Hijo PrioridaMaxima que puede quedar bloqueado");
	Thread *newchildThread = new Thread(name2,false,PrioridadMaxima);
	newchildThread->Fork(boba3, lockVariableCompartida);	
	
	
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
