#include <time.h>
#include <stdbool.h>
#include <string.h>

#define HIGH_SELLER   0
#define MEDIUM_SELLER 1
#define LOW_SELLER    2

typedef struct seat {
   char id[5]; // 0000 should be defualt value.
               // H101 for H1's first customer, H102 for the second, ...
               // M201 for M2's first customer, M202 for the second, ...
               // etc.
   
   int number; // Each seat number should be unique
   int sold; // 0 = not sold (open), 1 = sold (not open)
} seat_t;

struct customer{                   //struct for each customer in the queue
   int id;
   int arrivalTime;
   int sellingTime;
};

typedef struct customer customer;

struct customerQ{
   int id;                          // H->1, M-{1,2,3} L-{1,2,3,4,5,6}
   char type;                       // H,L,M
   int size;                        // Current size
   int capacity;                    // Maximum size
   int front;
   int rear;
   customer* customers;
};

typedef struct customerQ customerQ;

struct sellerQ{                      //struct  holding the pointers of each customerQs alias sellerQs
   customerQ* sellQs;
};


typedef struct sellerQ sellerQ;

void Enqueue(customerQ* Q,customer *cust);
void Dequeue(customerQ* Q);
void printQueue(customerQ Q);
customerQ* generateCustomersForQueue(customerQ* q, int numberOfCust, int sellerType);


customerQ* createQueue(int numOfCust,int id, char type)                //skeleton with init operations of each queue
{
   /* Create a Queue */
   customerQ* Q;
   Q = (customerQ *)malloc(sizeof(customerQ));
   Q->customers = malloc(sizeof(customer)*numOfCust);
   Q->size = 0;
   Q->capacity = numOfCust;
   Q->front = 0;
   Q->rear = -1;
   Q->id=id;
   Q->type=type;
   return Q;
}


sellerQ* createCustomerDataStructure(int numberOfCust){               //returned to main() to create sellerQ and customers in random sorted by arrival time
   /*To be populated based on requirement*/
   sellerQ* sellerQs;
   sellerQs = (sellerQ *)malloc(sizeof(sellerQ));
   sellerQs->sellQs = malloc(sizeof(customerQ)*10);
   int q=0;
   //H -> 1; 
   customerQ* Q = createQueue(numberOfCust,1,'H');                   //create the queue
   sellerQs->sellQs[0] = *generateCustomersForQueue(Q, numberOfCust, HIGH_SELLER);   //use that queue to generate customers and then assign the pointer to sellerQs
   //printQueue(sellerQs->sellQs[0]);
   for (q = 1; q <= 3; q++) {
      //M -> 1,2,3; 
      customerQ* Q = createQueue(numberOfCust,q,'M');
      sellerQs->sellQs[q] = *generateCustomersForQueue(Q, numberOfCust, MEDIUM_SELLER);
      //printQueue(sellerQs->sellQs[q]);
   }
   for (q = 4;q <= 9; q++) {
      //L -> 1,2,3,4,5,6
      customerQ* Q = createQueue(numberOfCust,q-3,'L');
      sellerQs->sellQs[q] = *generateCustomersForQueue(Q, numberOfCust, LOW_SELLER);
      //printQueue(sellerQs->sellQs[q]);
   }
   return sellerQs;
}

customerQ* generateCustomersForQueue(customerQ* q, int numberOfCust, int sellerType) {      //customer generation/queue
   //sleep(1);
   int i;
   int arrivalTime[numberOfCust];
   srand(time(NULL)); // seed RNG, enable when testing complete
   //printf("\n");

   // Seed the arrival times.
   for (i = 0; i < numberOfCust; i++) {
      arrivalTime[i]= (rand() % 60);
      //printf("A:%d", arrivalTime[i]);
   }


   // Sort by arrival time.
   int j;
   for (i = 0; i < numberOfCust - 1; i++) {
      for (j = i + 1; j < numberOfCust; j++) {
         if (arrivalTime[i] > arrivalTime[j]) {
            int temp = arrivalTime[j];
            arrivalTime[j] = arrivalTime[i];
            arrivalTime[i] = temp;
         }
      }	
   }

   // Construct the customer data;
   for (i = 0; i < numberOfCust; i++) {
      customer* cust;
      cust = (customer *)malloc(sizeof(customer));
      cust->id = i;
      cust->arrivalTime = arrivalTime[i];

      // Assign the seller time: 1 or 2 for high, 2, 3 or 4 for medium
      // and 4, 5, 6 or 7 for low.
      if (sellerType == (HIGH_SELLER)) {
         cust->sellingTime = (rand() % 2) + 1;
      } else if (sellerType == (MEDIUM_SELLER)) {
         cust->sellingTime = (rand() % 3) + 2;
      } else {
         // must be a LOW_SELLER
         cust->sellingTime = (rand() % 4) + 4; 
      }

      Enqueue(q, cust); 
   }
   return q;
}

//CHANGED
void Dequeue(customerQ* Q)
{
   /* If Queue size is zero then it is empty. So we cannot pop */
   if(Q->size == 0) {
      printf("Queue is Empty\n");
      return;
   
   /* Removing an element is equivalent to incrementing index of front by one */
   } else {
      Q->size--;
      Q->front++;
   }
   return;
}
customer front(customerQ Q)
{
   if (Q.size == 0) {
      printf("Queue is Empty\n");
      exit(0);
   }
   /* Return the element which is at the front*/
   printf("\nfront:%d", Q.customers[Q.front].arrivalTime);
   return Q.customers[Q.front];
}

void printQueue(customerQ Q){
   int i;
   printf("\n*****************************************");
   printf("\nQueue name:%c ID:%d",Q.type,Q.id);
   printf("\n*****************************************");
   for( i=Q.front;i<=Q.rear;i++){
      printf("\nCust id:%d \narrival:%d", Q.customers[i].id,Q.customers[i].arrivalTime);
   }
}

void Enqueue(customerQ* Q,customer* cust)
{
   /* If the Queue is full, we cannot push an element into it as there is no space for it.*/
   if(Q->size == Q->capacity)
      {
	 printf("Queue is Full\n");
      }
   else
      {
	 Q->size++;
	 Q->rear++;
	 Q->customers[Q->rear] = *cust;
      }
   return;
}

// ======================================================================
// Seller helper functions
// ======================================================================

// Get the first free seat index in row "row"
// Return -1 if a seat is not found.
int getFreeSeat(seat_t seats[][10], int row) {
   for (int i = 0; i < 10; i++) {
      if (seats[row][i].sold == 0) return i;
   }
   // If there are no free seats, return -1
   return -1;
}

// Return true if no seats are left (concert is full)
// Return false otherwise
bool isConcertFull(seat_t seats[][0]) {
   for (int i = 0; i < 10; i++) {
      for (int j = 0; j < 10; j++) {
	 if (seats[i][j].sold == 0) return false;
      }
   }
   return true;
}

// Finds the first available seat in seats based on seller_type, and
// stores the coordinates in avail_loc, which can then be used  from
// where the function was called.
// EXAMPLE: If avail_loc contains [0, 1], then the seat found is the
// second seat in row 1. If it contains [-1, -1], no seat was found.
bool sellAvailableSeat(seat_t seats[][10], char seller_type,
		       int seller_id_num, int cust_id_num) {
   // Create a marking for each seat.
   // Ex: If a seat is sold by H to their first customer, then the mark is H001
   //     If a seat is sold by M2 to their 11th customer, then the mark is M211
   //     etc.
   
   char mark[5];
   
   mark[0] = seller_type;
   mark[1] = seller_id_num + '0'; // Converts seller_id_num to char

   // mark[2] and mark[3] will be the 2-digit form of cust_id_num
   mark[2] = cust_id_num / 10 + '0';
   mark[3] = cust_id_num % 10 + '0';
   
   mark[4] = '\0';
   
   if (seller_type == 'H') {
      // Traverse from row 1 to row 10, checking the seats from left to right
      for (int i = 0; i < 10; i++) {
	 for (int j = 0; j < 10; j++) {
	    // If an open seat is found, sell it
	    if (seats[i][j].sold == 0) {
	       // printf("filling %s\n", seats[i][j].id);
	       seats[i][j].sold = 1;
	       strcpy(seats[i][j].id, mark);
	       return true;
	    }
	 }
      }
   }
   else if (seller_type == 'M') {
      // sell row 5 then 6 then 4 then 7 then 3 then 8 then 2 then 9 then 1 then 10
      //      index 4 then 5 then 3 then 6 then 2 then 7 then 1 then 8 then 0 then 9
      // 4 (+ 1 - 2 + 3 - 4 + 5 - 6 + 7 - 8 + 9)
      
      int counter = 0;   // Controls magnitude
      int mult = -1;     // Controls sign
      int row = 4;
      while (true) {
	 row += counter * mult;

	 for (int j = 0; j < 10; j++) {
	    if (seats[row][j].sold == 0) {
	       // printf("filling %s\n", seats[row][j].id);
	       seats[row][j].sold = 1;
	       strcpy(seats[row][j].id, mark);
	       return true;
	    }
	 }

	 mult = -mult;
	 counter++;

	 // Once all rows are checked
	 if (counter == 10) break;
      }
   }
   else if (seller_type == 'L') {
      // Traverse from row 10 to row 1, checking the seats from left to right
      for (int i = 9; i >= 0; i--) {
	 for (int j = 0; j < 10; j++) {
	    // If an open seat is found, sell it
	    if (seats[i][j].sold == 0) {
	       // printf("filling %s\n", seats[i][j].id);
	       seats[i][j].sold = 1;
	       strcpy(seats[i][j].id, mark);
	       return true;
	    }
	 }
      }
   }
   return false;
}

void print_seats(seat_t seats[][10]) {
   for (int i = 0; i < 10; i++) {
      // cycle through seats within a row
      for (int j = 0; j < 10; j++) {
	 printf("seats[%d][%d]: %s %d %d\n", i, j,
	 seats[i][j].id, seats[i][j].number, seats[i][j].sold);
      }
   }
}

void print_seat_table(seat_t seats[][10]) {
   printf(" =============== Seating Table =============== \n");
   for (int i = 0; i < 10; i++) {
      // cycle through seats within a row
      for (int j = 0; j < 10; j++) {
	 printf(" %s ", seats[i][j].id);
      }
      printf("\n");
   }
   printf("\n");
}
