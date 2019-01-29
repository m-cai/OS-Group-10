/*
 * Author: Maggie Cai
 * File: main.c
 * Description: This file contains the main program for the round robin scheduling algorithm.
 */
#include <stdlib.h>
#include <stdio.h>
#include "rr.h"

int main()
{
  printf("----------Creating Processes----------\n");
  ProcessNode *p_list = createProcesses(45);
  printf("----------Processes Created----------\n");

  printf("----------Beginning Round Robin----------\n");
  round_robin(p_list);
  printf("----------Round Robin Finished----------\n\n");

  return 0;
}
