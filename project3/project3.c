#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "utilities.h"
#define NUM_SELLERS 10
#define MAX_CUSTOMERS 15

// Condition and mutex to start the threads
pthread_cond_t start_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t start_mutex = PTHREAD_MUTEX_INITIALIZER;

// Artificial minutes time and lock and condition for proceeding each minute.
int current_time = 0;
pthread_cond_t time_cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t time_cond2 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t time_mutex = PTHREAD_MUTEX_INITIALIZER;

// Lock for the seat data structure.
pthread_mutex_t seat_mutex = PTHREAD_MUTEX_INITIALIZER;

// Lock and thread complete counters
int threads_ready = 0;
int threads_done = 0;
int threads_finished = 0;
pthread_cond_t thread_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t thread_mutex = PTHREAD_MUTEX_INITIALIZER;

// Global seat variable
// Generate seating arrangement
seat_t seats[10][10];

int high_seats_sold = 0;
int high_seats_unsold = 0;
int med_seats_sold = 0;
int med_seats_unsold = 0;
int low_seats_sold = 0;
int low_seats_unsold = 0;

// seller thread to serve one time slice (1 minute)
// void * sell(void *seller_type)
void* sell(customerQ* customer_queue)
{
   char seller_type = customer_queue->type;
   int seller_id_num = customer_queue->id;
   // int cust_id_num = customer_queue->customers[customer_queue->front].id;
   int cust_id_num = 1; // ^ Since the method up here just gives 0

   // Customer currently being sold a seat.
   bool selling = false;
   int started_selling_time = 0;
   int selling_time = 0;

   // printf("%c%d queue size = %d\n", seller_type, seller_id_num, customer_queue->size);
   pthread_mutex_lock(&start_mutex);
   pthread_cond_wait(&start_cond, &start_mutex);
   pthread_mutex_unlock(&start_mutex);

   // Loop until we run out of customers and are done selling.
   while (customer_queue->size > 0 || selling) {
      // Tell the main thread we're ready and wait for it to start the next time slice.
      pthread_mutex_lock(&time_mutex);
      threads_ready++;
      pthread_cond_signal(&time_cond1);
      pthread_cond_wait(&time_cond2, &time_mutex);
      pthread_mutex_unlock(&time_mutex);

      if (customer_queue->customers[cust_id_num-1].arrivalTime == current_time) {
	 printf("0:%02d | Customer %d has arrived in seller queue %c%d.\n",
		   current_time, cust_id_num, seller_type, seller_id_num);
      }
      
      // If we're not selling to a customer, and the next customer in the queue
      // has arrived, we need to sell them a seat.
      if (selling == false && customer_queue->customers->arrivalTime <= current_time) {
         selling_time = customer_queue->customers->sellingTime;
         selling = true;
         started_selling_time = current_time;
         // Sell the seat while in a lock 
         pthread_mutex_lock(&seat_mutex);

	 if (sellAvailableSeat(seats, seller_type, seller_id_num, cust_id_num++)) {
	    printf("0:%02d | Customer %d of seller queue %c%d has purchased a seat and is being seated.\n",
		   current_time, cust_id_num-1, seller_type, seller_id_num);

            if (seller_type == 'H') {
               high_seats_sold++;
            } else if (seller_type == 'M') {
               med_seats_sold++;
            } else {
               low_seats_sold++;
            }
               
	 } else {
	    printf("0:%02d | Customer %d of seller queue %c%d has been turned away. (Concert full)\n",
		   current_time, cust_id_num-1, seller_type, seller_id_num);

            if (seller_type == 'H') {
               high_seats_unsold++;
            } else if (seller_type == 'M') {
               med_seats_unsold++;
            } else {
               low_seats_unsold++;
            }
               
	 }
         pthread_mutex_unlock(&seat_mutex);
         Dequeue(customer_queue);
         //print_seat_table(seats);

      // Check if we're selling and if we're done with this customer.
      } else if (selling && started_selling_time + selling_time == current_time) {
         // We're done, so reset the selling info.
         selling = false;
         selling_time = 0;
         started_selling_time = 0;
      }

      // If we're not selling, nothing to do but wait until the next time tick.
      if (current_time == 59) {
	 if (customer_queue->rear >= customer_queue->front) {
	    for (int i = customer_queue->front; i <= customer_queue->rear; i++) {  
	    printf("0:%02d | Customer %d of seller queue %c%d has been turned away. (Time >= 59)\n",
		   current_time, i+1, seller_type, seller_id_num);
	    }
	 }
         break;
      }
 
      // Only say we're done with this timeslice if there's more for this thread to do.
      if (customer_queue->size > 0 || selling) {
         pthread_mutex_lock(&thread_mutex);
         threads_done++;
         pthread_cond_signal(&thread_cond);
         pthread_mutex_unlock(&thread_mutex);
      }
   }

   pthread_mutex_lock(&seat_mutex);
   if (seller_type == 'H') {
      high_seats_unsold += customer_queue->size;
   } else if (seller_type == 'M') {
      med_seats_unsold += customer_queue->size;
   } else {
      low_seats_unsold += customer_queue->size;
   }
   pthread_mutex_unlock(&seat_mutex);
               
   // Increment the finished tracker after the thread is done selling.
   pthread_mutex_lock(&thread_mutex);
   threads_finished++;
   pthread_cond_signal(&thread_cond);
   pthread_mutex_unlock(&thread_mutex);

   //printf("DEBUG: Thread done!\n");
   return NULL; // thread exits
}

void wakeup_all_seller_threads() {
   pthread_mutex_lock(&start_mutex);
   pthread_cond_broadcast(&start_cond);
   pthread_mutex_unlock(&start_mutex);
}

void generate_seats(seat_t seats[][10]) {
   // cycle through rows
   for (int i = 0; i < 10; i++) {
      // cycle through seats within a row
      for (int j = 0; j < 10; j++) {
	 seats[i][j].id[0] = ' ';
	 seats[i][j].id[1] = '-';
	 seats[i][j].id[2] = '-';
	 seats[i][j].id[3] = ' ';
	 seats[i][j].id[4] = '\0';
	 seats[i][j].number = (i)*10 + (j+1);
	 seats[i][j].sold = 0;
      }
   }
}

int main(int argc, char *argv[]) {
   generate_seats(seats);
   print_seats(seats);
   print_seat_table(seats);
   
   int i;
   pthread_t tids[NUM_SELLERS];

   // Process the number of customers per seller command line argument
   if (argc < 2) {
      printf("Must specify number of customers per seller.\n");
      printf("\n Usage: project3 <number of customers 1 to %d\n\n", MAX_CUSTOMERS);
      exit(1);
   }

   int num_customers = (int)strtol(argv[1], NULL, 10);
   if (num_customers < 1 || num_customers > MAX_CUSTOMERS) {
      printf("Must enter a number of customers per seller between 1 and %d\n", MAX_CUSTOMERS);
      exit(2);
   }
   printf("Number of customers per seller: %d\n", num_customers);

   sellerQ* sellerQ = createCustomerDataStructure(num_customers);

   printf("\n\nCreating one high price ticket seller\n");
   pthread_create(&tids[0], NULL, (void *)sell, &sellerQ->sellQs[0]);

   printf("Creating three medium price ticket sellers\n");
   for (i = 1; i < 4; i++) { 
      pthread_create(&tids[i], NULL, (void *)sell, &sellerQ->sellQs[i]);
   }

   printf("Creating six low price ticket sellers\n");
   for (i = 4; i < 10; i++) {
      pthread_create(&tids[i], NULL, (void *)sell, &sellerQ->sellQs[i]);
   }

   sleep(1);
   printf("\nAll seller threads created\n");

   wakeup_all_seller_threads();

   while (current_time < 60) {
      printf("\nCurrent time: 0:%02d\n\n", current_time);   

      // We need to wait until all of the running threads are ready before
      // signaling them to sell with the next time slice.
      pthread_mutex_lock(&time_mutex);
      while (threads_ready + threads_finished < 10) {
         pthread_cond_wait(&time_cond1, &time_mutex);
      }
      threads_ready = 0;
      pthread_cond_broadcast(&time_cond2);
      pthread_mutex_unlock(&time_mutex);

      // Wait until all threads are done with this time slice.
      pthread_mutex_lock(&thread_mutex);
      while (threads_done + threads_finished < 10) {
         pthread_cond_wait(&thread_cond, &thread_mutex);
      }
      threads_done = 0;
      pthread_mutex_unlock(&thread_mutex);
         
      print_seat_table(seats);
      current_time++;
   }

   // wait for all seller threads to exit
   for (i = 0; i < 10; i++) {
      pthread_join(tids[i], NULL);
   }

   printf("Final table:\n\n");
   print_seat_table(seats);
   
   printf("The high cost seller sold seats to %d customers and turned %d away\n",
          high_seats_sold, high_seats_unsold);
   printf("The three medium cost sellers sold seats to %d customers and turned %d away\n",
          med_seats_sold, med_seats_unsold);
   printf("The six low cost sellers sold seats to %d customers and turned %d away\n",
          low_seats_sold, low_seats_unsold);

   exit(0);
}
