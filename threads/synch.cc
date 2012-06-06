// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

#include <stdio.h>

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(const char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List<Thread*>;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    
    while (value == 0) { 			// semaphore not available
	queue->Append(currentThread);		// so go to sleep
	currentThread->Sleep();
    } 
    value--; 					// semaphore available, 
						// consume its value
    
    interrupt->SetLevel(oldLevel);		// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
		scheduler->ReadyToRun(thread);
    value++;
    interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
Lock::Lock(const char* debugName) {
	Constructor(debugName,PrioridadMinima);
}
Lock::Lock(const char* debugName, PrioridadHilo prioriRecurso) {
	Constructor(debugName,prioriRecurso);
}

void Lock::Constructor(const char* debugName, PrioridadHilo prioriRecurso) {
	name = debugName;
	s = new Semaphore(debugName, 1);
	prioridadRecurso = prioriRecurso;
}

Lock::~Lock() {
	delete s;
}
void Lock::Acquire() {
	s->P();
	//Para evitar la inversion de prioridades, cada hilo que adquiera el cerrojo
	//se le sube la priorirdad en caso de que la suya sea menor que la del recurso
	if (currentThread->getPrioridad() < prioridadRecurso) {
		currentThread->setPrioridad(prioridadRecurso);
	}
}
void Lock::Release() {
	s->V();
}
bool Lock::isHeldByCurrentThread()
{
    return true; // FIXME
}

Condition::Condition(const char* debugName, Lock* conditionLock) {
	lock = conditionLock;
	name = debugName;
	s = new Semaphore(debugName, 0);
	sleepingProcsCounter = 0;
}

Condition::~Condition() {
	//nada, no estamos consumiendo memoria
}

// Las tres operaciones sobre variables condici�n.
// El hilo que invoque a cualquiera de estas operaciones debe tener
// adquirido el cerrojo correspondiente; de lo contrario se debe
// producir un error.
// El codigo cliente debe adquirir previamente el cerrojo de la condicion, ejemplo:
// mylock = new Lock("pepe");
// condicion = new Condition("pepe",mylock)
// mylock ->Acquire();
// condicion -> wait();
// ...
// mylock ->Acquire();
// condicion->Signal();
// mylock ->Release();

void Condition::Wait() {
	sleepingProcsCounter++;
	lock->Release();//si hay cambio de contexto aca y otro hilo hace signal, el v() correspondiente no despierta a nadie
	s->P();// y entonces  en este P(), el currentThread no se va a dormir por que la variable dentro del semaforo no es 0
	//por ende lo unico importante es proteger la zona crítica de esta porcion de codigo que es la variable sleepingProcsCounter,
	//con lo cual se debe incrementarla antes de liberar el cerrojo
}

void Condition::Signal() {
	if(sleepingProcsCounter > 0){
		s->V();
		sleepingProcsCounter--;
	}
}

void Condition::Broadcast() {
	while(sleepingProcsCounter>0){
		s->V();
		sleepingProcsCounter--;
	}
}


Messages::Messages(const char* debugName){
	 name = debugName;
	 lock = new Lock(debugName);
	 portNumber = 0;
	 queue = new List<Slot*>;
}

Messages::~Messages(){
	delete lock;
	delete queue;
}


void Messages::Send(Port port, int senderMsg) {
	lock->Acquire();
	Iterator<Slot*> *iterator = queue->GetIterator();
	while(iterator->HasNext()){
		 Slot *slot = iterator->Next();
		if(slot->port == port){
			if(!slot->receiverBufQueue->IsEmpty()){
				int *receiverBuf = slot->receiverBufQueue->Remove();
				*receiverBuf = senderMsg;
				slot->condition->Signal();
				lock->Release();
				return;
			} else {
				slot->senderMsgQueue->Append(senderMsg);
				slot->condition->Wait();
				return;
			}
		}
	}
	Slot* slot = new Slot();
	slot->port = port;
	slot->condition = new Condition(name, lock);
	slot->senderMsgQueue = new List<int>;
	slot->receiverBufQueue = new List<int*>;
	slot->senderMsgQueue->Append(senderMsg);
	queue->Append(slot);
	slot->condition->Wait();
}

void Messages::Receive(Port port, int *receiverBuf) {
	lock->Acquire();
	Iterator<Slot*> *iterator = queue->GetIterator();
	while(iterator->HasNext()){
		 Slot *slot = iterator->Next();
		if(slot->port == port){
			if(!slot->senderMsgQueue->IsEmpty()){
				int senderMsg = slot->senderMsgQueue->Remove();
				*receiverBuf = senderMsg;
				slot->condition->Signal();
				lock->Release();
				return;
			} else {
				slot->receiverBufQueue->Append(receiverBuf);
				slot->condition->Wait();
				lock->Release();
				return;
			}
		}
	}
	Slot* slot = new Slot();
	slot->port = port;
	slot->condition = new Condition(name, lock);
	slot->receiverBufQueue = new List<int*>;
	slot->senderMsgQueue = new List<int>;
	slot->receiverBufQueue->Append(receiverBuf);
	queue->Append(slot);
	slot->condition->Wait();
}
