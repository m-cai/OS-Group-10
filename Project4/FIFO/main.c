#include "utilities.h"
#include <stdlib.h>
#include <stdio.h>

int main()
{
  printf("----------Creating Processes----------\n");
  ProcessNode *p_list = createProcesses();
  printf("----------Processes Created----------\n");

  printf("----------Beginning FIFO Paging Algorithm----------\n");
  FIFO(p_list);
  printf("----------FIFO Paging Algorithm Finished----------\n\n");

  return 0;
}
