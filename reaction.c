#include <pthread.h>
#include "reaction.h"

// Forward declaration. This function is implemented in reaction-runner.c,
// but you needn't care what it does. Just be sure it's called when
// appropriate within reaction_o()/reaction_h().
void make_water();


void reaction_init(struct reaction *reaction)
{
  //initialize the conditions and mutexes
  pthread_mutex_init(&reaction->hydrogen_availability_lock, NULL);
  pthread_mutex_init(&reaction->oxygen_lock, NULL);
  pthread_mutex_init(&reaction->hydrogen_lock, NULL);
  reaction->hydrogens_available = 0;
  pthread_cond_init(&reaction->oxygen_condition, NULL);
  pthread_cond_init(&reaction->hydrogen_condition, NULL);
}


void reaction_h(struct reaction *reaction)
{
  //lock the two threads for oxygen and the hydrogen availability
  pthread_mutex_lock(&reaction->oxygen_lock);
  pthread_mutex_lock(&reaction->hydrogen_availability_lock);
  //increment the number of hydrogens
  reaction->hydrogens_available++;
  //set the hydrogen counting lock free
  pthread_mutex_unlock(&reaction->hydrogen_availability_lock);
  //lock the hydrogen thread so that it receives the signal from reaction_o
  pthread_mutex_lock(&reaction->hydrogen_lock);
  //signal the oxygen lock
  pthread_cond_signal(&reaction->oxygen_condition);
  //release the oxygen lock
  pthread_mutex_unlock(&reaction->oxygen_lock);
  //wait to know that it was used by reaction_o before unlocking and exiting
  pthread_cond_wait(&reaction->hydrogen_condition, &reaction->hydrogen_lock);
  pthread_mutex_unlock(&reaction->hydrogen_lock);
}


void reaction_o(struct reaction *reaction)
{
    pthread_mutex_lock(&reaction->oxygen_lock);
    do
    {
      //lock up the hydrogen availability lock
       pthread_mutex_lock(&reaction->hydrogen_availability_lock);
       //check hydrogens
        if(reaction->hydrogens_available>=2)
        {
          //decrement if the option
           reaction->hydrogens_available-=2;
          break;
        }
        //unlock it
       pthread_mutex_unlock(&reaction->hydrogen_availability_lock);
       //wait for a signal from the hydrogens
        pthread_cond_wait(&reaction->oxygen_condition, &reaction->oxygen_lock);
    } while(1);
    //unlock the threads
    pthread_mutex_unlock(&reaction->hydrogen_availability_lock);
    pthread_mutex_unlock(&reaction->oxygen_lock);

    //singal and let two hydrogens be released
    pthread_mutex_lock(&reaction->hydrogen_lock); 
    pthread_cond_signal(&reaction->hydrogen_condition);
    pthread_mutex_unlock(&reaction->hydrogen_lock);

    pthread_mutex_lock(&reaction->hydrogen_lock);
    pthread_cond_signal(&reaction->hydrogen_condition);
    pthread_mutex_unlock(&reaction->hydrogen_lock);
    //make the water
    make_water();
}