import java.io.*;
import java.util.*;

class Fcfs_Sjf {
  public static void main(String[] args) {
    char[] nameList = new char[]{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P'
    , 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p'
    , 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    System.out.println("First Come First Serve Algorithm");
    for (int i = 0; i < 5; i++) {
      fcfs(nameList);
    }
    System.out.println("======================================================================================");
    System.out.println("Shortest Job First Algorithm");
    for (int i = 0; i < 5; i++) {
      sjf(nameList);
    }
    System.out.println("======================================================================================");
  }

  private static List<JOB> newRandomList(char[] nameList) { // method to create new random job list
    Random ran = new Random();
    List<JOB> jobList = new ArrayList<>();
    int processNum = 45;
    for (int i = 0; i < processNum; i++) {
      JOB job = new JOB(nameList[i], ran.nextInt(100), (float)(ran.nextInt(100) + 1) / 10, ran.nextInt(4) + 1);
      jobList.add(job);
    }
    for (JOB job : jobList) {
        System.out.println("Process Name: " + job._name + " Arrival Time: " + job._arrivalTime + " Expected Run Time: " + job._expected_servicetime + " Priority: " + job._priority);
    }
    return jobList;
  }

  private static void fcfs(char[] nameList) {
    System.out.println("Shortest job first algorithm");
    List<JOB> jobList = newRandomList(nameList);  // get new job list
    PriorityQueue<JOB> heap = new PriorityQueue<>((o1, o2) -> o1._arrivalTime - o2._arrivalTime); // use heap to sort the job by arrival time in accending order
    Queue<JOB> process_queue = new LinkedList<>();  // use a queue to represent the process queue
    for (JOB job : jobList) {
      heap.add(job);
    }
    StringBuilder time_chart = new StringBuilder();  // use string builder to represent the time chart
    JOB current = null;  // current running process
    for (int i = 0; i < 100; i++) {
      if (!heap.isEmpty() && i >= heap.peek()._arrivalTime) { // put the process into the process queue at arrival time
        process_queue.add(heap.poll());
      }
      if (current == null) {
        if (process_queue.isEmpty()) { // cpu is idle
          time_chart.append('-');
        } else {
          current = process_queue.poll(); // top process in the process queue start to run
          current._startTime = i;
        }
      }
      if (current != null) {
        time_chart.append(current._name);
        if (i - current._startTime + 1 == (int) Math.ceil(current._expected_servicetime)) { // operation when current process finish to run
          current._finishTime = i;
          current = null;
        }
      }
    }
    if (current != null) { // check if current process finished to run after time quantum 99
      int run_time = 100 - current._startTime;
      for (int i = 0; i < (int) Math.ceil(current._expected_servicetime) - run_time; i++) {
        time_chart.append(current._name);
      }
      current._finishTime = 99 + (int) Math.ceil(current._expected_servicetime) - run_time;
    }
    int total_turn = 0;
    int total_response = 0;
    int total_waiting = 0;
    int processRunNum = 0;
    int throughPut = 0;
    for (JOB job : jobList) { // calculate average statistic number
      if (job._startTime != -1) {
        processRunNum++;
        job._runTime = (int) Math.ceil(job._expected_servicetime);
        total_turn += job._finishTime - job._arrivalTime;
        total_response += job._startTime - job._arrivalTime;
        total_waiting += job._finishTime - job._arrivalTime - job._runTime + 1;
      }
    }
    System.out.println("Time Chart: " + time_chart);
    System.out.println("Average turnaround time: " + (double) total_turn / processRunNum);
    System.out.println("Average waiting time: " + (double) total_waiting / processRunNum);
    System.out.println("Average response time: " + (double) total_response / processRunNum);
    System.out.println("Throughput: " + (double)processRunNum / time_chart.length());
  }

  /*  The shortest job first algorithm is same as the first come first serve algorithm,
      the only different is that the process queue is implemented by an priority queue 
      which sorted by the job's expected run time in acending order
  */
  private static void sjf(char[] nameList) {
    List<JOB> jobList = newRandomList(nameList);
    System.out.println("Shortest job first algorithm");
    PriorityQueue<JOB> heap = new PriorityQueue<>((o1, o2) -> o1._arrivalTime - o2._arrivalTime);
    PriorityQueue<JOB> process_queue = new PriorityQueue<>( // use priority queue instead of queue
        (o1, o2) -> o1._expected_servicetime - o2._expected_servicetime > 0 ? 
        (int) Math.ceil(o1._expected_servicetime - o2._expected_servicetime) : 
        (int) Math.floor(o1._expected_servicetime - o2._expected_servicetime)
    );
    for (JOB job : jobList) {
      heap.add(job);
    }
    StringBuilder time_chart = new StringBuilder();
    JOB current = null;
    for (int i = 0; i < 100; i++) {
      if (!heap.isEmpty() && i >= heap.peek()._arrivalTime) {
        process_queue.add(heap.poll());
      }
      if (current == null) {
        if (process_queue.isEmpty()) {
          time_chart.append('-');
        } else {
          current = process_queue.poll();
          current._startTime = i;
        }
      }
      if (current != null) {
        time_chart.append(current._name);
        if (i - current._startTime + 1 == (int) Math.ceil(current._expected_servicetime)) {
          current._finishTime = i;
          current = null;
        }
      }
    }
    if (current != null) {
      int run_time = 100 - current._startTime;
      for (int i = 0; i < (int) Math.ceil(current._expected_servicetime) - run_time; i++) {
        time_chart.append(current._name);
      }
      current._finishTime = 99 + (int) Math.ceil(current._expected_servicetime) - run_time;
    }
    int total_turn = 0;
    int total_response = 0;
    int total_waiting = 0;
    int processRunNum = 0;
    int throughPut = 0;
    for (JOB job : jobList) {
      if (job._startTime != -1) {
        processRunNum++;
        job._runTime = (int) Math.ceil(job._expected_servicetime);
        total_turn += job._finishTime - job._arrivalTime;
        total_response += job._startTime - job._arrivalTime;
        total_waiting += job._finishTime - job._arrivalTime - job._runTime + 1;
      }
    }
    System.out.println("Time Chart: " + time_chart);
    System.out.println("Average turnaround time: " + (double) total_turn / processRunNum);
    System.out.println("Average waiting time: " + (double) total_waiting / processRunNum);
    System.out.println("Average response time: " + (double) total_response / processRunNum);
    System.out.println("Throughput: " + (double)processRunNum / time_chart.length());
  }

  private static class JOB{
    char _name;
    int _arrivalTime;
    int _startTime = -1;
    int _finishTime = -1;
    float _expected_servicetime;
    int _runTime = 0;
    int _priority;
    public JOB(char name, int arrival, float expected_servicetime, int priority) {
      _name = name;
      _arrivalTime = arrival;
      _expected_servicetime = expected_servicetime;
      _priority = priority;
    }
  }
}
