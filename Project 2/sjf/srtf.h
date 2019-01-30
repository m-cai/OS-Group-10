#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#ifndef srtf_h
#define srtf_h
void driverProgram();
int obtainRemainingProcessCount(bool reachedEnd);
void printList(int start, int end, int basic);
float getquantumTillProcess(int index, int end);
int getMinRemainTimeProcess(int quantumInd);
float* computePerfMetrics(int endIndex);
int NUM_PROCESS;

struct Process {
    int processId;
    float arrivalTime;
    float expectedRunTime;
    float remainTime;
    float starts;
    float ends;
    float turnAroundTime;
    float waitTime;
    float responseTime;
    int priority;
}procObj[100];

float* executeSRTF(int n){
    NUM_PROCESS = n;
    driverProgram();
    return computePerfMetrics (NUM_PROCESS - 1);
}

void driverProgram() {
    printf ("\n\n *************************** Total Number of Processes: %d *************************** \n\n", NUM_PROCESS);
    process_t* procs = gen_proc_arr (NUM_PROCESS); //processes generate from proc.h
    for (int i = 0; i < NUM_PROCESS; i++) {
       //procObj[] is a wrapper for the generated processes
        procObj[i].processId = (procs+i)->pid;
        procObj[i].arrivalTime = (procs+i)->arrivalTime;
        procObj[i].expectedRunTime = (procs+i)->runTime;
        procObj[i].remainTime = (procs+i)->runTime;
        procObj[i].priority = (procs+i)->priority;
        procObj[i].starts = -1;
        procObj[i].ends = -1;
        procObj[i].turnAroundTime = -1;
        procObj[i].responseTime = -1;
        procObj[i].waitTime = -1;
    }
    
    int end = NUM_PROCESS - 1;
    printList(0,end,1);
    float timer = procObj[0].arrivalTime;
    float quantum = 0;
    int index = 0;
    int execProcInd = 0;
    bool endFlag = false;
    printf("\n\n *********************************** Starting to compute ***********************************");
    while(1) {
        if(index < NUM_PROCESS - 1){
            while(procObj[index].arrivalTime == procObj[index + 1].arrivalTime) {
                index += 1;
            }
            quantum = getquantumTillProcess (index, end);
        }
        if(index + 1 <= NUM_PROCESS) {
            index += 1;
        }
        while ((quantum > 0) || (endFlag)) {
            //get min process from the queue 
            execProcInd = getMinRemainTimeProcess(index);
            if (execProcInd >= 0) {
                if (endFlag) {
                   //reached the end because of unlimited quantum
                    quantum = 9999;
                    if (execProcInd == -1) {
                        endFlag = false;
                        break;
                    }
                }
                if (procObj[execProcInd].starts == -1) {
                    procObj[execProcInd].starts = timer;
                }
                if (quantum >= procObj[execProcInd].remainTime) {
                    quantum -= procObj[execProcInd].remainTime;
                    timer += procObj[execProcInd].remainTime;
                    printf("\nFrom Timer:%f To Timer:%f",timer - procObj[execProcInd].remainTime,timer);
                    procObj[execProcInd].remainTime = 0;
                    if(procObj[execProcInd].ends == -1){
                        procObj[execProcInd].ends = timer;
                    }
                    printf("\t\tProcess:%d completed!",procObj[execProcInd].processId);
                } else {
                    procObj[execProcInd].remainTime -= quantum;
                    timer += quantum;
                    printf("\nFrom Timer:%f To Timer:%f",timer - quantum,timer);
                    quantum = 0;
                    printf("\t\tProcess:%d partial \t-> Remains:%f",procObj[execProcInd].processId, procObj[execProcInd].remainTime);
                }
            } else if ((execProcInd == -1) && (!endFlag)) {
                printf("\tCPU idle for: %f",quantum);
                timer = procObj[index].arrivalTime;
                quantum = 0;
            } else {
                break;
            }
        }
        
        int remainingProcCount = 0;
        bool reachedEnd;
        if(index == end + 1) {
            quantum = 100 - timer;
            if(quantum > 0) {
                reachedEnd = false;
                remainingProcCount = obtainRemainingProcessCount(reachedEnd); //timer < 100
                if(remainingProcCount == 0) {
                    break;
                } else {
                    printf("\n\n\t Continue Processing for utilising full cpu cycle of 100 \n\n");
                }
            } else {
                reachedEnd = true;
                printf("\n\n");
                //quantumTime computation index reached end, so from here the quantums is assumed to be infinite
                //timer == 100
                remainingProcCount=obtainRemainingProcessCount(reachedEnd);
                endFlag = true;
                if(remainingProcCount > 0){
                    printf("\n\n ***********************************   Reached the end of Quantum 100    *********************************** \n");
                    printf("\n\n ***********************************   Executing with unlimited Quantum Time  ****************************** \n");
                }
            }
        }
        if ((execProcInd == -1) && (endFlag) && remainingProcCount == 0) {
            printf("\n\n***********************************   Finished executing all processes   *********************************** \n");
            endFlag = false;
            break;
        }
    }//end-while(1)
}

int obtainRemainingProcessCount(bool reachedEnd) {
    int rem = 0;
    printf("\n\n***********************************   Remaining Processes After Time Quantum 100   ***********************************");
    for (int i; i < NUM_PROCESS; i++) {
        if (procObj[i].remainTime > 0) {
            printf("\nPID:%d \tExpected Runtime:%f \tRemaining Time:%f", procObj[i].processId, procObj[i].expectedRunTime,procObj[i].remainTime);
            if ((procObj[i].remainTime == procObj[i].expectedRunTime) && (reachedEnd)) {
                procObj[i].remainTime = -1;
                printf("\t->\t This Process will not be executed");
            } else {
                rem += 1;
            }
        }
    }
    if (rem == 0) {
        printf("\n \t\t\t\t\t *** NO Remaining Process ***");
    }
    return rem;
}


float* computePerfMetrics(int endIndex) {
    int numberOfSuccessfullProcess = 0;
    float waitTime = 0.0;
    float tATime = 0.0;
    float respTime = 0.0;
    float endTime = 0.0;
    int success=0;
    
    for (int i = 0; i <= endIndex; i++){
        if((procObj[i].starts >= 0) && (procObj[i].ends >= 0)) {
            //compute only for process which has executed at least once in time <= 100  ==> Successfull processes
            success+=1;
            if(procObj[i].ends > endTime) {
                endTime=procObj[i].ends;
            }
            procObj[i].turnAroundTime  = procObj[i].ends - procObj[i].arrivalTime;
            procObj[i].waitTime     = (procObj[i].turnAroundTime - procObj[i].expectedRunTime > 0)?(procObj[i].turnAroundTime - procObj[i].expectedRunTime):(0.00);
            procObj[i].responseTime = (procObj[i].starts - procObj[i].arrivalTime > 0)?(procObj[i].starts - procObj[i].arrivalTime):(0.00);
            tATime += procObj[i].turnAroundTime;
            waitTime += procObj[i].waitTime;
            respTime += procObj[i].responseTime;
            numberOfSuccessfullProcess += 1;
        }
    }
    
    waitTime /= numberOfSuccessfullProcess;
    tATime /= numberOfSuccessfullProcess;
    respTime /= numberOfSuccessfullProcess;
    printList(0,endIndex,0);
    float throughput = success/endTime;
    printf("\n\n/****************/ \nCompleted in\t:%f \nProcesses ran\t:%d\nAvg. Waiting\t:%f \nAvg. TurnAround\t:%f \nAvg. Response\t:%f\nThroughput\t:%f\n\n",endTime,success,waitTime,tATime,respTime,throughput);
    float *perfArray = (float*)malloc(3*sizeof(float));
    perfArray[0] = waitTime;
    perfArray[1] = tATime;
    perfArray[2] = respTime;
    return perfArray;
}

float getquantumTillProcess(int index, int end){
    float quantum;
    for (; index < end; index++) {
        if (procObj[index].arrivalTime != procObj[index+1].arrivalTime) {
            quantum = procObj[index+1].arrivalTime - procObj[index].arrivalTime;
            index = index+1;
            break;
        }
    }
    return quantum;    
}

void printList(int start, int end, int basic) {
    if (basic) {
        for (int i = 0; i <= end; i++) {
            printf("\nPID:%d \t\tArrival Time:%f \t\tExpected Runtime:%f \tPriority:%d", procObj[i].processId, procObj[i].arrivalTime,procObj[i].expectedRunTime,procObj[i].priority);
        }
    } else {
        printf("\n\n***********************************  printing success processes  *********************************** \n");
        for (int i = 0; i <= end; i++) {
            if (procObj[i].remainTime != -1) {
                printf("PID:%d \tExp Runtime:%f \tArrival Time:%f \tStart:%f\t End:%f\tResponse:%f \tTurnAround:%f \tWait:%f \t\n",procObj[i].processId,procObj[i].expectedRunTime,procObj[i].arrivalTime,procObj[i].starts,procObj[i].ends,procObj[i].responseTime,procObj[i].turnAroundTime,procObj[i].waitTime);
                //break;
            }
        }
        printf("\n\n***********************************  printing failure processes  *********************************** \n");
        int fail = 0;
        for (int i = 0;i <= end; i++) {
            if (procObj[i].remainTime == -1) {
                fail += 1;
                printf("PID:%d \tExp Runtime:%f \tArrival Time:%f \tStart:%f\t End:%f\tResponse:%f \tTurnAround:%f \tWait:%f \t\n",procObj[i].processId,procObj[i].expectedRunTime,procObj[i].arrivalTime,procObj[i].starts,procObj[i].ends,procObj[i].responseTime,procObj[i].turnAroundTime,procObj[i].waitTime);
            }
        }
        if (fail == 0) {
            return;
        }
    }
}

int getMinRemainTimeProcess(int quantumInd){
    int mrtIndex = -1;
    float remainTime = 100;
    for (int i = 0; i < quantumInd; i++) {
        if ((procObj[i].remainTime < remainTime) && (procObj[i].remainTime != 0) && (procObj[i].remainTime != -1)) {
            remainTime = procObj[i].remainTime;
            mrtIndex = i;
        }
    }
    return mrtIndex;
}
#endif /* srtf_h */
