#include <stdio.h>
#include <pthread.h>

struct reaction {
	// FILL ME IN
    //declare all the mutexes and threads and hydrogen variables
    pthread_mutex_t oxygen_lock;
    pthread_mutex_t hydrogen_availability_lock;
    pthread_mutex_t hydrogen_lock;
    int hydrogens_available;
    pthread_cond_t hydrogen_condition; 
    pthread_cond_t oxygen_condition;
};

void reaction_init(struct reaction *reaction);

void reaction_h(struct reaction *reaction);

void reaction_o(struct reaction *reaction);
