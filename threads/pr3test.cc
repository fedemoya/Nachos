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
	for(int i=0; i<10000; i++);
	printf("Current thread: %s\n",currentThread->getName());
}

// Cuando el padre llama a Join el hijo todavia esta vivo.
void boba2(void *arg){
	currentThread->Yield();
	for(int i=0; i<10000; i++);
	printf("Current thread: %s\n",currentThread->getName());
}

void JoinTest(){
	char* threadname = new char[100];
	sprintf(threadname, "Hijo1");
	Thread *childThread = new Thread(threadname, true);
 	childThread->Fork(boba2, NULL);
 	currentThread->Join(childThread);
 	printf("Current thread: %s\n",currentThread->getName());
	sprintf(threadname, "Hijo2");
	childThread = new Thread(threadname, true);
	childThread->Fork(boba2, NULL);
	currentThread->Join(childThread);
	printf("Current thread: %s\n",currentThread->getName());
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