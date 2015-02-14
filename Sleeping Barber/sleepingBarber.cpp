/* 
 * File:   sleepingBarber.cpp
 * Author: roman
 *
 * Created on February 8, 2015, 1:06 PM
 */

#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <stdlib.h>

using namespace std;

sem_t printSem;
sem_t barberReady;
sem_t accessSeats;
sem_t customerReady;
int numFreeSeats = 2;
int numCustomers = 15;

void *barber(void* name){
    sem_wait(&printSem);
    cout << "Start: " << endl;
    sem_post(&printSem);
    while(true){
        sem_wait(&customerReady);   //Barber sleeps if this is locked
        sem_wait(&accessSeats);
        numFreeSeats++;
        sem_post(&barberReady);
        sem_post(&accessSeats);     //Barber is working
        
    }
    pthread_exit(NULL);
}

void *customer(void* id){
    sem_wait(&printSem);
    cout << "Start customer: " << (long) id << endl;
    sem_post(&printSem);
    sleep(2);
    sem_wait(&accessSeats);
    if(numFreeSeats>0){
        numFreeSeats--;
        sem_post(&customerReady);
        sem_post(&accessSeats);
        sem_wait(&printSem);
        cout << "Customer #: " << (long) id << " waiting" << endl;
        sem_post(&printSem);
        sem_wait(&barberReady);
        sem_wait(&printSem);
        cout << "Customer #: " << (long) id << " cut" << endl;
        sem_post(&printSem);
        sleep(rand() % 3);
    }
    else{
        sem_wait(&printSem);
        cout << "New customer left: " << (long) id << endl;
        sem_post(&printSem);
        sem_post(&accessSeats);
    }
    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    srand(time(NULL));
    sem_init(&barberReady,0,0);
    sem_init(&printSem,0,1);
    sem_init(&accessSeats,0,1);
    sem_init(&customerReady,0,0);
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_t barberThread;
    long barberId = 15;
    pthread_t customers[numCustomers];
    pthread_create(&barberThread,&attr,barber,(void*) barberId);
    for(long i=0;i<numCustomers;i++){
        pthread_create(&customers[i],&attr,customer,(void*) i);
    }
    pthread_exit(NULL);
}

