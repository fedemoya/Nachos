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
	name = debugName;
	s = new Semaphore(debugName, 1);
}
Lock::~Lock() {
	delete s;
}
void Lock::Acquire() {
	s->P();
}
void Lock::Release() {
	s->V();
}
bool Lock::isHeldByCurrentThread()
{
    return lockOwnerThread == currentThread;
}

Condition::Condition(const char* debugName, Lock* conditionLock) {
	lock = conditionLock;
	name = debugName;
	Semaphore s = new Semaphore(debugName, 0);
	sleepingProcsCounter = 0;
}

Condition::~Condition() {
	//nada, no estamos consumiendo memoria
}

void Condition::Wait() {
	lock->Release();
	sleepingProcsCounter++;
	s->P();
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
	 queue = new List <Slot*>;
}

Messages::~Messages(){
	delete lock;
	delete queue;
}


Port Messages::getNextPortNumber(){
	return portNumber++;
}

void Messages::Send(Port puerto, int mensaje) {
	lock->Acquire();
	List<Slot*>::iterator it;
	for (it = queue.begin(); it != queue.end(); it++){
		if(it->puerto == puerto){
			if(!it->messageBufQueue->empty){
				int *menssageBuf = it->messageBufQueue->remove();
				*messageBuf = mensaje;
				it->menssageBufQueue->condition->signal();
				lock->release();
				return;
			} else {
				it->messageBufQueue->message->Append(mensaje);
				it->messageBufQueue->condition->Wait();
				lock->release();
				return;
			}
		}
	}
	Slot* slot = new Slot();
	slot->condition = new Condition(name, lock);
	slot->message->Append(mensaje);
	slot->condition->Wait();
	//pensar bien donde van lock->Acquire(); lock->Release();
	//Buscar puerto por receive en queue
	// en caso de encontrarlo, copiar el mensaje al puntero del slot, borrar el slot y despertar a dicho proceso usando la condicion
	// en caso contrario crear el slot

}

void Messages::Receive(Port puerto, int* mensaje) {

	//pensar bien donde van lock->Acquire(); lock->Release();
	//Buscar puerto por send en queue
	// en caso de no encontrar, encolas un slot, e irse a dormir;
	// en caso contrario crear el slot


}
