/* 
 * File:   main.cpp
 * Author: roman
 *
 * Created on February 12, 2015, 2:59 PM
 */

#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
using namespace std;

const int NTHREADS = 4;
int eat_count[NTHREADS];    // Philosopher eat count
pthread_mutex_t outlock;   // Print mutex
int nsteps=0;
int maxsteps = 100;
#define LEFT (i+NTHREADS-1)%NTHREADS //Philosopher to the left
#define RIGHT (i+1)%NTHREADS         //Philosopher to the right
#define THINKING 1                   // states
#define HUNGRY 2
#define EATING 3

pthread_cond_t CV[NTHREADS];    // Condition - chopsticks free
pthread_mutex_t M;               // Associated mutex
int state[NTHREADS];            // states for each philosopher

int 
update_state (int i) {
    if (state[i] == HUNGRY
     && state[LEFT] != EATING
     && state[RIGHT] != EATING) {
        state[i] = EATING;
        pthread_cond_signal (&CV[i]);
    }
    return 0;
}

void chopsticks_init () {
    int i;
    pthread_mutex_init (&M, NULL);
    for (i = 0; i < NTHREADS; i++) {
        pthread_cond_init (&CV[i], NULL);
        state[i] = THINKING;
    }
}

void chopsticks_take (int i) {
    pthread_mutex_lock (&M);      
    state[i] = HUNGRY;            
    update_state(i);              
    while (state[i] == HUNGRY)    
        pthread_cond_wait (&CV[i],&M);
    pthread_mutex_unlock (&M); 
}

void chopsticks_put (int i) {
    pthread_mutex_lock (&M);      
    state[i] = THINKING;
    update_state (LEFT);           
    update_state (RIGHT);
    pthread_mutex_unlock (&M);     
}

void trace(int i, char *s) {
    pthread_mutex_lock(&outlock);
    if (strcmp (s, "eating") == 0) {
        eat_count [i]++;
    }
    cout << i << ":" << s << endl;
    cout << nsteps << endl;
    if (nsteps++ > maxsteps) {
        if (strcmp(s,"thinking") == 0) {
            pthread_mutex_unlock(&outlock); 
            cout << "Thread done" << endl;
            pthread_exit(0);
        }
    }
    pthread_mutex_unlock(&outlock);
}

void * philosopher_body(void *arg) {
    int self = *(int *) arg;
    for (;;) {
        trace(self,"thinking");
        chopsticks_take(self);
        sleep(rand() % 3);
        trace(self,"eating");
        chopsticks_put(self);
        sleep(rand() % 3);
    }
}

int main() {
    int i;
    srand(time(NULL));
    pthread_t th[NTHREADS];
    int no[NTHREADS];

    for (i = 0; i < NTHREADS; i++) eat_count [i] = 0;

    pthread_mutex_init(&outlock, NULL);
    chopsticks_init();

    for (i = 0; i < NTHREADS; i++) {
        no[i] = i;
        pthread_create(&th[i], NULL, philosopher_body, (int *) &no[i]);
    }

    for (i = 0; i < NTHREADS; i++) pthread_join(th[i], NULL);

    for (i = 0; i < NTHREADS; i++) {
        cout << "philospher " << i << " ate " << eat_count[i] << " times" << endl;
    }

    return 0;
}


