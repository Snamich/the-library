/*
  threads.c

  Created by Scott Brandt on 5/6/13 and edited by Scott Chatham on 6/2/13

  This file contains all the code for implementing and testing a lottery
  scheduler.
*/

/* Necessary includes */
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define _XOPEN_SOURCE
#include <ucontext.h>

/* Helpful constants */
#define MAX_THREADS 10
#define MAX_TICKETS 40
#define MAX_WORK 50

/* The Thread struct */
typedef struct Thread {
  int number;
  int tickets;
  int work;
} Thread;

/* Global variables */
static ucontext_t ctx[MAX_THREADS];
static Thread *threads[MAX_THREADS];
static int main_thread = 0;
static int scheduler_thread = 1;
static int current_thread = -1;
static int live_threads = MAX_THREADS - 2;
static int total_tickets = MAX_TICKETS;

/* Function prototypes */
void thread_create(int, int, void (*)(void));
void thread_delete(Thread *);
void thread_print(Thread *);
void thread_scheduler();
static void thread_work();
void timer_handler(int);

/* Our main function */
int main() {
  /* Set up the seed for our later calls to rand */
  srand(time(NULL));

  printf("Main starting\n");
  current_thread = main_thread;

  /* Setup signal handler */
  signal(SIGALRM, timer_handler);

  /* Set up scheduler thread */
  thread_create(scheduler_thread, main_thread, thread_scheduler);

  printf("Creating our worker threads\n");
  int i;
  for( i = 2; i < MAX_THREADS; ++i ) {
    thread_create(i, scheduler_thread, thread_work);
  }
  
  printf("All threads created\n");

  printf("Calling the scheduler\n");
  swapcontext(&ctx[current_thread], &ctx[scheduler_thread]);  

  /* If we get here then all threads are finished and we can exit */
  printf("All work finished, exiting main\n");
  exit(0);
}

/*
  thread_create
  RETURNS: void
  ARGUMENTS: thread_number - the thread we are creating
             exit_thread - the thread to run when the current thread is done
             thread_function - the function assigned to the thread

  This function creates a thread by populating the Thread structure and doing
  the voodoo magic of setting up its context.
*/
void thread_create(int thread_number, int exit_thread, void (*thread_function)(void)) {
  printf("Creating thread %d\n", thread_number);

  /* Create the thread */
  Thread *t = malloc(sizeof(Thread));

  /* Check if it was allocated properly */
  if(t == NULL) {
    printf("%s\n", strerror(errno)); 
    exit(-1);
  }

  t->number = thread_number;
  t->tickets = 5;
  t->work = rand() % MAX_WORK; /* Calculate the amount of work of the thread */

  threads[thread_number] = t;
  thread_print(t);

  // Create a valid execution context the same as the current one
  getcontext(&ctx[thread_number]);

  // Now set up its stack
  ctx[thread_number].uc_stack.ss_sp = malloc(8192);
  ctx[thread_number].uc_stack.ss_size = 8192;

  // This is the context that will run when this thread exits
  ctx[thread_number].uc_link = &ctx[exit_thread];

  // Now create the new context and specify what function it should run
  makecontext(&ctx[thread_number], thread_function, 0);

  printf("Done creating thread %d\n", thread_number);
}

/*
  thread_delete
  RETURNS: void
  ARGUMENTS: t - a Thread

  This function is in charge of deleting a thread.
*/
void thread_delete(Thread *t) {
  /* Turn off the alarm so deletion isn't interrupted */
  setitimer(ITIMER_REAL, 0, NULL);

  /* Delete the thread */
  printf("Thread %d is done!\n", t->number);
  total_tickets -= t->tickets;
  free(t);
  threads[current_thread] = NULL;
  --live_threads;

  /* Switch back to scheduler */
  swapcontext(&ctx[current_thread], &ctx[scheduler_thread]);
}

/*
  thread_print
  RETURNS: void
  ARGUMENTS: t - a Thread

  This function simply prints out the contents of the given thread.
*/
void thread_print(Thread *t)
{
  printf("Thread %d", t->number);
  printf(" has %d tickets", t->tickets);
  printf(" and %d work to do.\n", t->work);
}

/*
  thread_scheduler
  RETURNS: void
  ARGUMENTS: void

  This function is in charge of determining which thread gets to run next by using
  a lottery scheduler. A random number between [0,1] is chosen and using the current
  thread's ratio of tickets/total_tickets along with the previous threads ratios a
  random thread is selected to run.
*/
void thread_scheduler()
{
  int num_tickets, i;
  double r;

  while(live_threads > 0) {
    /* Determine which thread runs next */
    r = (double)rand() / (double)RAND_MAX;
    num_tickets = 0;

    for(i = 2; i < MAX_THREADS; ++i) {
      Thread *t = threads[i];
      /* Make sure the thread is valid */
      if(t != NULL) {
        if(((double)num_tickets / total_tickets < r) && 
           (r < (((double)num_tickets + t->tickets) / total_tickets))) {
          current_thread = i;
          break;
        }
        /* Not the lucky winner, add his tickets and move on */
        num_tickets += t->tickets;
      }
    }

    thread_print(threads[current_thread]);

    /* Set timer for interrupt */
    struct itimerval itv;
    itv.it_interval.tv_sec = 0;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 0;
    itv.it_value.tv_usec = 50;

    setitimer(ITIMER_REAL, &itv, NULL);

    /* Switch threads */
    swapcontext(&ctx[scheduler_thread], &ctx[current_thread]);
  }
}

/*
  thread_work
  RETURNS: void
  ARGUMENTS: void

  This function is assigned to each thread which when run simply
  decrements the work variable assigned to each thread until it is
  zero and then passes the thread to thread_exit to delete it.
*/
static void thread_work() {
  Thread *t = threads[current_thread];
  printf("First time thread %d has run\n", t->number);

  while( t->work > 0 ) {
    /* Do some work */
    --t->work;
    printf("Thread %d still has %d more work to do\n", t->number, t->work);
  }

  /* No more work to do, kill the thread */
  thread_delete(t);
}

/*
  timer_handler
  RETURNS: void
  ARGUMENTS: i - not used (only used to match signal handler declaration)

  This function is called every time a thread's quantum is used up and will
  pass control back to the scheduler.
*/
void timer_handler(int i)
{
  printf("Time's up!\n");
  /* Reset our signal handler */
  signal(SIGALRM, timer_handler);
  /* Call the scheduler */
  swapcontext(&ctx[current_thread], &ctx[scheduler_thread]);
}
