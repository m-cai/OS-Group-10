#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef hpf_h
#define hpf_h

// Helper function : turn 3.99 floats into 4 ints.
int float_integer(float time) {
    time = time * 10;
    return round(time);
}

float integer_float(int time) {
    return (float)time / 10;
}

float calc_stats(process_t *procs, int num_procs) {
    float turntime_total = 0.0;
    float resptime_total = 0.0;
    float waittime_total = 0.0;
    float max_complete = 0.0;
    float priority_stats[12] = {0.0};
    int priority_count[4] = {0};
    int valid_procs = 0;
    for (int i = 0; i < num_procs; i++) {
        process_t* proc = procs + i;
        if (proc->turnaroundTime > 0.0) {
            valid_procs++;
            turntime_total += proc->turnaroundTime;
            resptime_total += proc->responseTime;
            waittime_total += proc->waitTime;
            priority_stats[(proc->priority * 3) - 3] += proc->turnaroundTime;
            priority_stats[(proc->priority * 3) - 2] += proc->responseTime;
            priority_stats[(proc->priority * 3) - 1] += proc->waitTime;
            priority_count[proc->priority - 1]++;
            if (proc->completedTime > max_complete) {
                max_complete = proc->completedTime;
            }
        }
    }
    printf("\n******** Stats for run ********\n");
    printf("Avg. TAT: %.2f\n", turntime_total / (float)valid_procs);
    printf(" Avg. RT: %.2f\n", resptime_total / (float)valid_procs);
    printf(" Avg. WT: %.2f\n", waittime_total / (float)valid_procs);
    for (int i = 0; i < 4; i++) {
        printf("\n******** priority %d job queue *******\n", i + 1);
        printf("Avg. TAT: %.2f\n", priority_stats[(i * 3)] / (float)priority_count[i]);
        printf(" Avg. RT: %.2f\n", priority_stats[(i * 3) + 1] / (float)priority_count[i]);
        printf(" Avg. WT: %.2f\n", priority_stats[(i * 3)+ 2] / (float)priority_count[i]);
    }
    
    return valid_procs / max_complete;
}

float nonpreempt(int num_procs) {
    process_t *procs = gen_proc_arr (num_procs);
    node_t *p1queue = NULL;
    node_t *p2queue = NULL;
    node_t *p3queue = NULL;
    node_t *p4queue = NULL;
    node_t *proclist = NULL;
    printf("\nStarting HPF nonpreempt");
    printf("\nNo. of processes: %d\n", num_procs);
    
    for (int i = 0; i < num_procs; i++) {
        process_t* proc = procs + i;
        proc_print(proc);
        
        // Add procs to the list of procList.
        proclist = procList_pushLast_helper(proclist, proc);
    }
    
    int quanta_count = 0;
    process_t* proc_running = NULL;
    int process_start = 0;
    int idle_time = 0;
    while (quanta_count < 1000 || proc_running != NULL) {
        // Check if a process has arrived and add it to a priority queue if so.
        process_t *proc = NULL;
        if (proclist != NULL) {
            proc = proclist->process;
        }
        while (proc != NULL) {
            if (float_integer(proc->arrivalTime) == quanta_count) {
                if (proc->priority == 1) {
                    p1queue = procList_pushLast_helper(p1queue, proc);
                } else if (proc->priority == 2) {
                    p2queue = procList_pushLast_helper(p2queue, proc);
                } else if (proc->priority == 3) {
                    p3queue = procList_pushLast_helper(p3queue, proc);
                } else if (proc->priority == 4) {
                    p4queue = procList_pushLast_helper(p4queue, proc);
                }
                proclist = procList_popBeg_helper(proclist);
                if (proclist != NULL) {
                    proc = proclist->process;
                } else {
                    proc = NULL;
                }
            } else {
                proc = NULL;
            }
        }
            // check if the running process is done.
            if (proc_running != NULL && quanta_count - process_start == float_integer(proc_running->runTime)) {
                printf("P%d: pid %d completed time = %.1f\n", proc_running->priority, proc_running->pid, integer_float(quanta_count));
                proc_running->completedTime = integer_float(quanta_count);
                proc_running->waitTime = integer_float(process_start) - proc_running->arrivalTime;
                proc_running->turnaroundTime = integer_float(quanta_count) - proc_running->arrivalTime;
                proc_running->responseTime = integer_float(process_start) - proc_running->arrivalTime;
                proc_running = NULL;
            }
            // If no process is running, check the priority queues for processes to run.
            if (proc_running == NULL && quanta_count < 1000) {
                if (p1queue != NULL) {
                    proc_running = p1queue->process;
                    p1queue = procList_popBeg_helper(p1queue);
                    process_start = quanta_count;
                    printf("P1: pid %d started time = %.1f\n", proc_running->pid, integer_float(quanta_count));
                    idle_time = 0;
                } else if (p2queue != NULL) {
                    proc_running = p2queue->process;
                    p2queue = procList_popBeg_helper(p2queue);
                    process_start = quanta_count;
                    printf("P2: pid %d started time = %.1f\n", proc_running->pid, integer_float(quanta_count));
                    idle_time = 0;
                } else if (p3queue != NULL) {
                    proc_running = p3queue->process;
                    p3queue = procList_popBeg_helper(p3queue);
                    process_start = quanta_count;
                    printf("P3: pid %d started time = %.1f\n", proc_running->pid, integer_float(quanta_count));
                    idle_time = 0;
                } else if (p4queue != NULL) {
                    proc_running = p4queue->process;
                    p4queue = procList_popBeg_helper(p4queue);
                    process_start = quanta_count;
                    printf("P4: pid %d started time = %.1f\n", proc_running->pid, integer_float(quanta_count));
                    idle_time = 0;
                } else {
                    if (idle_time == 20 && integer_float(quanta_count) > procs->arrivalTime) {
                        printf("\n**************\nIdle time > 2 quanta!\n**************\n");
                    }
                    idle_time++;
                }
            }
            quanta_count++;
    }
  return calc_stats(procs, num_procs);
}

float preemptive(int num_procs) {
    process_t* procs = gen_proc_arr(num_procs);
    node_t *p1queue = NULL;
    node_t *p2queue = NULL;
    node_t *p3queue = NULL;
    node_t *p4queue = NULL;
    node_t *proclist = NULL;
    printf("\nStarting HPF preemptive");
    printf("\nNo. of processes: %d\n", num_procs);
    
    for (int i = 0; i < num_procs; i++) {
        process_t* proc = procs + i;
        proc_print(proc);
        
        // Add procs to the list of procList.
        proclist = procList_pushLast_helper(proclist, proc);
    }
    
    // To track the time a process ran.
    int proc_runtime[num_procs];
    memset(proc_runtime, 0, num_procs * sizeof(int));
    // To track the time a process waited after being started.
    int proc_starttime[num_procs];
    memset(proc_starttime, 0, num_procs * sizeof(int));
    
    int quanta_count = 0;
    process_t* proc_running = NULL;
    int idle_time = 0;
    int active_queue = 0;
    while (quanta_count < 1000 || proc_running != NULL) {
        // Drop any process that hasn't started after reaching 100 quanta.
        if (proc_running != NULL && proc_starttime[proc_running->pid] > 1000) {
            printf("Dropping P%d process pid %d at time %.1f\n", proc_running->priority, proc_running->pid, integer_float(quanta_count));
            proc_running = NULL;
        }
        // Check if a process arrived and add it to a priority queue if so.
        process_t *proc = NULL;
        if (proclist != NULL) {
            proc = proclist->process;
        }
        while (proc != NULL) {
            if (float_integer(proc->arrivalTime) == quanta_count) {
                if (proc->priority == 1) {
                    p1queue = procList_pushLast_helper(p1queue, proc);
                } else if (proc->priority == 2) {
                    p2queue = procList_pushLast_helper(p2queue, proc);
                } else if (proc->priority == 3) {
                    p3queue = procList_pushLast_helper(p3queue, proc);
                } else if (proc->priority == 4) {
                    p4queue = procList_pushLast_helper(p4queue, proc);
                }
                proclist = procList_popBeg_helper(proclist);
                if (proclist != NULL) {
                    proc = proclist->process;
                } else {
                    proc = NULL;
                }
            } else {
                proc = NULL;
            }
        }
        
        // Check if the running process is done.
        if (proc_running != NULL && proc_runtime[proc_running->pid] == float_integer(proc_running->runTime)) {
            printf("P%d: pid %d completed time = %.1f\n", proc_running->priority, proc_running->pid, integer_float(quanta_count));
            proc_running->completedTime = integer_float(quanta_count);
            proc_running->waitTime = integer_float(quanta_count) - proc_running->arrivalTime - proc_running->runTime;
            proc_running->turnaroundTime = integer_float(quanta_count) - proc_running->arrivalTime;
            proc_running->responseTime = integer_float(proc_starttime[proc_running->pid]) - proc_running->arrivalTime;
            proc_running = NULL;
            active_queue = 0;
        }
        
        // If no process is running, check the priority queues for processes to run.
        if (proc_running == NULL) {
            if (p1queue != NULL) {
                proc_running = p1queue->process;
                p1queue = procList_popBeg_helper(p1queue);
                printf("P1: PID %d start time = %.1f\n", proc_running->pid, integer_float(quanta_count));
                active_queue = 1;
                idle_time = 0;
            } else if (p2queue != NULL) {
                proc_running = p2queue->process;
                p2queue = procList_popBeg_helper(p2queue);
                printf("P2: PID %d start time = %.1f\n", proc_running->pid, integer_float(quanta_count));
                active_queue = 2;
                idle_time = 0;
            } else if (p3queue != NULL) {
                proc_running = p3queue->process;
                p3queue = procList_popBeg_helper(p3queue);
                printf("P3: PID %d start time = %.1f\n", proc_running->pid, integer_float(quanta_count));
                active_queue = 3;
                idle_time = 0;
            } else if (p4queue != NULL) {
                proc_running = p4queue->process;
                p4queue = procList_popBeg_helper(p4queue);
                printf("P4: PID %d start time = %.1f\n", proc_running->pid, integer_float(quanta_count));
                active_queue = 4;
                idle_time = 0;
            }
            
            // If empty in any queue, it's idle.
            if (proc_running == NULL) {
                // Only print this warning if two quanta have passed after a process has arrived.
                if (idle_time == 20 && integer_float(quanta_count) > procs->arrivalTime) {
                    printf("\n***************\nIdle time > 2 quanta!\n***************\n");
                }
                idle_time++;
            }
            
            // If something is running, check if any thing higher priority came in
            // and switch to that higher prority process/queue, if not, do
            // round robin on the active queue.
        } else {
            // If something higher priority came in, switch to that queue/process.
            if (active_queue > 1 && p1queue != NULL) {
                if (active_queue == 2) {
                    p2queue = procList_pushLast_helper(p2queue, proc_running);
                } else if (active_queue == 3) {
                    p3queue = procList_pushLast_helper(p3queue, proc_running);
                } else if (active_queue == 4) {
                    p4queue = procList_pushLast_helper(p4queue, proc_running);
                } else {
                    printf("BUG!\n");
                }
                proc_running = p1queue->process;
                p1queue = procList_popBeg_helper(p1queue);
                printf("P1: PID %d start time = %.1f\n", proc_running->pid, integer_float(quanta_count));
                active_queue = 1;
            } else if (active_queue > 2 && p2queue != NULL) {
                if (active_queue == 3) {
                    p3queue = procList_pushLast_helper(p3queue, proc_running);
                } else if (active_queue == 4) {
                    p4queue = procList_pushLast_helper(p4queue, proc_running);
                } else {
                    printf("BUG!\n");
                }
                proc_running = p2queue->process;
                p2queue = procList_popBeg_helper(p2queue);
                printf("P2: PID %d start time = %.1f\n", proc_running->pid, integer_float(quanta_count));
                active_queue = 2;
            } else if (active_queue > 3 && p3queue != NULL) {
                if (active_queue == 4) {
                    p4queue = procList_pushLast_helper(p4queue, proc_running);
                } else {
                    printf("BUG!\n");
                }
                proc_running = p3queue->process;
                p3queue = procList_popBeg_helper(p3queue);
                printf("P3: PID %d start time = %.1f\n", proc_running->pid, integer_float(quanta_count));
                active_queue = 3;
                
                // If nothing higher priority came in, do round robin on that queue.
            } else if (active_queue == 1) {
                if (p1queue != NULL) {
                    p1queue = procList_pushLast_helper(p1queue, proc_running);
                    proc_running = p1queue->process;
                    p1queue = procList_popBeg_helper(p1queue);
                    printf("P1: PID %d start time = %.1f\n", proc_running->pid, integer_float(quanta_count));
                }
            } else if (active_queue == 2) {
                if (p2queue != NULL) {
                    p2queue = procList_pushLast_helper(p2queue, proc_running);
                    proc_running = p2queue->process;
                    p2queue = procList_popBeg_helper(p2queue);
                    printf("P2: PID %d start time = %.1f\n", proc_running->pid, integer_float(quanta_count));
                }
            } else if (active_queue == 3) {
                if (p3queue != NULL) {
                    p3queue = procList_pushLast_helper(p3queue, proc_running);
                    proc_running = p3queue->process;
                    p3queue = procList_popBeg_helper(p3queue);
                    printf("P3: PID %d start time = %.1f\n", proc_running->pid, integer_float(quanta_count));
                }
            } else if (active_queue == 4) {
                if (p4queue != NULL) {
                    p4queue = procList_pushLast_helper(p4queue, proc_running);
                    proc_running = p4queue->process;
                    p4queue = procList_popBeg_helper(p4queue);
                    printf("P4: PID %d start time = %.1f\n", proc_running->pid, integer_float(quanta_count));
                }
            }
        }
        // Process runtime time and start time counting.
        if (proc_running != NULL) {
            
            if (proc_starttime[proc_running->pid] == 0 && quanta_count != 0) {
                proc_starttime[proc_running->pid] = quanta_count;
            }
            proc_runtime[proc_running->pid]++;
        }
        
        quanta_count++;
    }
    
    return calc_stats(procs, num_procs);
}

void HPF_NONPRE(int numproc) {
    float nonprethru = 0.0;
    for (int i = 0; i < 5; i++) {
        nonprethru += nonpreempt(numproc);
    }
    printf("\nAvg. nonpremptive HPF throughput: %.4f\n\n", nonprethru / 5.0);
}

void HPF_PREEMP(int numproc) {
    float preemptthru = 0.0;
    for (int i = 0; i < 5; i++) {
        preemptthru += preemptive(numproc);
    }
    printf("\nAvg. premptive HPF throughput: %.4f\n\n", preemptthru / 5.0);
    
}
#endif /* hpf_h */
