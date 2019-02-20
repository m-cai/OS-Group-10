/*
 * Author: Maggie Cai
 * File: utilities.c
 * Description: This file contains the implementation of the FIFO paging algorithm.
 */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "utilities.h"

const char PROCESS_NAMES[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
                              'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
                              'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 's', 'd',
                              'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                              'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
                              'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

struct process {
  int procName;
  int procSize;
  int arrTime;
  int servTime;
};

struct page {
  int pageNum;
  int procName;
  int pageRef;
  struct page *next;
};

struct page_list {
  int free;
  struct page *list;
};

struct proc_node {
  struct process *proc;
  double servTime;
  int currPage;
  struct page *pages;
  struct proc_node *next;
};

struct proc_queue {
  struct proc_node *head;
  struct proc_node *tail;
};

int assignSize(int size)
{
  switch(size) {
    case 0:
      return 5;
    case 1:
      return 11;
    case 2:
      return 17;
    case 3:
      return 31;
    default:
      printf("Invalid size\n");
  }

  return 0;
}

PageList *createPageList()
{
  PageList *page_list = malloc(sizeof(PageList));
  page_list->free = 99;
  page_list->list = NULL;
  Page *iter = NULL;

  for(int i = 0; i < 100; i++) {
    Page *page = malloc(sizeof(Page));
    page->pageNum = i;
    page->procName = -1;
    page->pageRef = 0;
    page->next = NULL;

    if(page_list->list == NULL) {
      page_list->list = page;
      iter = page_list->list;
    } else {
      iter->next = page;
      iter = iter->next;
    }
  }

  return page_list;
}

void deletePageList(PageList *list)
{
  Page *temp = NULL;
  if(list != NULL) {
    while(list->list != NULL) {
      temp = list->list;
      list->list = list->list->next;
      free(temp);
    }
  }

  free(list);
}

ProcessQueue *createQueue()
{
  ProcessQueue *pq = malloc(sizeof(ProcessQueue));
  if(pq != NULL) {
    pq->head = NULL;
    pq->tail = NULL;
  }
  return pq;
}

void deleteQueue(ProcessQueue *pq)
{
  ProcessNode *temp = NULL;
  if(pq != NULL) {
    while(pq->head != NULL)
    {
      temp = pq->head;
      pq->head = pq->head->next;
      free(temp->proc);
      free(temp);
    }
  }

  free(pq);
}

void enqueue(ProcessQueue *pq, ProcessNode *node)
{
  if(pq->head == NULL) {
    pq->head = node;
    pq->tail = node;
    node->next = NULL;
  } else {
    pq->tail->next = node;
    pq->tail = node;
    node->next = NULL;
  }
}

ProcessNode *dequeue(ProcessQueue *pq)
{
  if(pq->head == NULL) {
    return NULL;
  }

  ProcessNode *node = pq->head;
  if(pq->head == pq->tail) {
    pq->head = NULL;
    pq->tail = NULL;
  } else {
    pq->head = pq->head->next;
  }
  node->next = NULL;

  return node;
}

ProcessNode *createProcesses()
{
  ProcessNode *p_list = malloc(sizeof(ProcessNode));
  p_list = NULL;

  int seed = time(NULL);
  srand(seed);
  for(int i = 0; i < 150; i++) {
    int id = i;
    int size = assignSize(rand() % 4);
    int arrival_time = rand() % 60;
    int run_time = (rand() % 5) + 1;

    Process *proc = malloc(sizeof(Process));
    proc->procName = id;
    proc->procSize = size;
    proc->arrTime = arrival_time;
    proc->servTime = run_time;

    ProcessNode *node = malloc(sizeof(ProcessNode));
    node->proc = proc;
    node->servTime = 0.0;
    node->currPage = 0;
    node->pages = NULL;
    node->next = NULL;

    p_list = insertList(p_list, node);
    printProcess(node);
  }

  return p_list;
}

ProcessNode *insertList(ProcessNode *p_list, ProcessNode *node)
{
  ProcessNode *prev = NULL;
  ProcessNode *iter = p_list;

  if(p_list == NULL) {
    p_list = node;
    return p_list;
  } else {
    while(iter != NULL) {
      if(iter->proc->arrTime <= node->proc->arrTime) {
        prev = iter;
        iter = iter->next;
      } else {
        if(prev == NULL) {  //inserting at head
          node->next = p_list;
          p_list = node;
        } else {  //inserting in the middle
          node->next = iter;
          prev->next = node;
        }
        return p_list;
      }
    }
    prev->next = node;  //inserting at the end
    return p_list;
  }
}

ProcessNode *getList(ProcessNode **p_list)
{
  ProcessNode *temp = NULL;

  if(*p_list == NULL) {
    return NULL;
  }

  temp = (*p_list);
  *p_list = (*p_list)->next;
  return temp;
}

void deleteList(ProcessNode *p_list)
{
  ProcessNode *temp = NULL;
  while(p_list != NULL) {
    temp = p_list;
    p_list = p_list->next;
    free(temp->proc);
    free(temp);
  }
}

void deleteProcess(ProcessNode *node)
{
  Page *t_page = NULL;
  while(node->pages != NULL) {
    t_page = node->pages;
    node->pages = node->pages->next;
    free(t_page);
  }
  free(node->proc);
  free(node);
}

void printQueue(ProcessQueue *pq)
{
  if(pq == NULL || pq->head == NULL) {
    printf ("NULL Queue\n");
  } else {
    ProcessNode *iter = pq->head;
    while(iter != pq->tail){
      printf("%d, ", iter->proc->procName);
      iter = iter->next;
    }
    printf("%d", iter->proc->procName);
    printf("\n");
  }
}

void printList(ProcessNode *p_list)
{
  ProcessNode *iter = p_list;
  if(iter == NULL) {
    printf("NULL List\n");
  } else {
    while(iter != NULL) {
      printf("%d, ", iter->proc->procName);
      iter = iter->next;
    }
    printf("\n");
  }
}

void printProcess(ProcessNode *node)
{
  if(node == NULL) {
    printf("NULL process\n");
  } else {
    printf("Process: P%d \t", node->proc->procName);
    printf("Size: %d \t", node->proc->procSize);
    printf("Arrival Time: %d \t", node->proc->arrTime);
    printf("Run Time: %d \n", node->proc->servTime);
  }
}

void printMemoryMap(PageList *page_list)
{
  if(page_list != NULL) {
    printf("Memory-map: ");
    Page *iter = page_list->list;
    for(int i = 0; i < 100; i++) {
      printf("%d", iter->procName);
      iter = iter->next;
    }
    printf("\n");
  } else {
    printf("Memory map NULL\n");
  }
}

int pageReference(int currPage, int size)
{
  int r = rand() % 11;
  int i = 0;
  if(r < 7) {
    i = (rand() % 3) - 1;
    i = currPage + i;
    if(i < 0) {
      i = size - 1;
    }
  } else if(r < 9){
    i = rand() % (currPage - 1);
  } else {
    i = (rand() % (size - (currPage + 2))) + currPage + 2;
  }

  return i;
}

Page *setPage0(int procName)
{
  Page *temp = malloc(sizeof(Page));
  temp->pageNum = 0;
  temp->procName = procName;
  temp->pageRef = 0;
  temp->next = NULL;

  return temp;
}

Page *setPage(Page *dest, int pageRef, int srcNum)
{
  Page *newPage = malloc(sizeof(Page));
  newPage->pageNum = srcNum;
  newPage->pageRef = pageRef;
  newPage->next = NULL;

  Page *iter = dest;
  while(iter->next != NULL) {
    iter = iter->next;
  }
  iter->next = newPage;

  return dest;
}

void FIFO(ProcessNode *p_list)
{
  int hit = 0;
  int miss = 0;
  int numProcesses = 0;

  ProcessQueue *proc_queue = createQueue();
  ProcessQueue *running_queue = createQueue();
  PageList *free_list = createPageList();
  printMemoryMap(free_list);
  Page *free_page = free_list->list->next;
  ProcessNode *run_proc = NULL;
  ProcessNode *prev = NULL;

  // For processes with arrival time between 0 to 60
  for(double i = 0.0; i < 60.0; i += 0.1) {
    //Retrieve processes from sorted process list with arrival time = i
    while(p_list != NULL) {
      if(p_list->proc->arrTime == (int) (i + 0.0001)) {
        ProcessNode *temp = getList(&p_list);
        enqueue(proc_queue, temp);
      } else {
        break;
      }
    }

    //Check if there are currently running processes
    prev = NULL;
    run_proc = running_queue->head;
    while(run_proc != NULL) {
      if(run_proc->proc->servTime > (int) (run_proc->servTime + 0.0001)) {
        int ref = pageReference(run_proc->currPage, run_proc->proc->procSize);

        int pageInMem = -1;
        int evictedPage = -1;
        Page *iter = run_proc->pages;
        while(iter != NULL) {
          if(iter->pageRef == ref) {
            pageInMem = iter->pageNum;
            hit++;
            break;
          }
          iter = iter->next;
        }

        if(pageInMem > -1) {
          printf("[0:0%2.1f] Process: P%d, Page Referenced: %d, Page-in-memory: %d\n",
            i, run_proc->proc->procName, ref, pageInMem);
        } else {
          miss++;
          if(free_page == NULL) {
            free_page = free_list->list->next;
          }
          int evicted = free_page->procName;
          free_page->procName = run_proc->proc->procName;
          free_page->pageRef = ref;
          run_proc->pages = setPage(run_proc->pages, ref, free_page->pageNum);
          printf("[0:0%2.1f] Process: P%d, Page Referenced: %d, Page-in-memory: %d, Process Evicted: %d\n",
            i, run_proc->proc->procName, ref, free_page->pageNum, evicted);
          free_page = free_page->next;
          if(evicted == -1)
          {
            free_list->free--;
          }
        }
        run_proc->servTime += 0.1;

        prev = run_proc;
        run_proc = run_proc->next;  //move to next running process

      } else {  //If service duration is complete
        printf("[0:0%2.1f]", i);
        printf(" Process: P%d, Exit, Size: %d, Service Duration: %d\n",
          run_proc->proc->procName, run_proc->proc->procSize, run_proc->proc->servTime);
        printMemoryMap(free_list);

        if(prev == NULL) {  //remove finished process from running queue
          running_queue->head = run_proc->next;
        } else {
          prev->next = run_proc->next;
        }

        deleteProcess(run_proc);
        free_list->free++;

        if(prev == NULL) {  //move to next running process
          run_proc = running_queue->head;
        } else {
          run_proc = prev->next;
        }
      }
    }

    //Remove the first process in the queue to run
    if(free_list->free > 4) {
      run_proc = dequeue(proc_queue);
      if(run_proc != NULL) {
        run_proc->currPage = 0;
        run_proc->pages = setPage0(run_proc->proc->procName);
        free_list->list->procName = run_proc->proc->procName;
        enqueue(running_queue, run_proc);
        printf("[0:0%2.1f]", i);
        printf(" Process: P%d, Enter, Size: %d, Service Duration: %d\n",
          run_proc->proc->procName, run_proc->proc->procSize, run_proc->proc->servTime);
        printMemoryMap(free_list);
        numProcesses++;
      }
    }

  }

  deleteQueue(proc_queue);
  deleteList(p_list);
  deletePageList(free_list);

  printf("\n----------Printing Statistics----------\n");
  printf("Hits: %d\tMisses: %d\tHit/Miss Ratio: %.2f\n", hit, miss, (double) hit / miss);
  printf("Number of processes: %d\n", numProcesses);
}
