package program;

import java.io.FileWriter;
import java.io.IOException;
import java.util.*;
import model.Process;
import scheduling.LFU;
import scheduling.LRU;
import scheduling.MFU;
import scheduling.RandomPick;

class PageReplacementAlgorithm {
	//list to add output comments
	public static ArrayList<String> outputData = new ArrayList<>();
	public static int random_Page_size[] = {5,11,17,31};

	public static void main(String args[]) throws IOException {
		outputData.clear();
		Scanner src = new Scanner(System.in);
		outputData.add("Enter the number of process:\n");
		System.out.println("Enter the number of process:\n");
		int noOfProcesses = src.nextInt();
		src.close();
		outputData.add("noOfProcesses=" + noOfProcesses + "\n");
		Process processes[] = new Process[noOfProcesses];
		
		// prameter=1 for static values
		Random number = new Random();

		// populate random values to Process:PID, Arrival_time, Execution_time and Priority
		for (int i = 0; i < noOfProcesses;i++) {
			processes[i] = new Process(i + 1, number.nextInt(61), number.nextInt(5) + 1, random_Page_size[number.nextInt(4)]); 
		}
		//sorting using bubble sort
		sortByArrivalTime(processes);
		outputData.add("sorted List\n");
		printList(processes);//function to print lists
		ArrayList<Process> processList = new ArrayList<>();
		processList.clear();

		//restore process list for next algorithm
		processList = restore(processes);
		outputData.add("\n----------------------LRU-------------------\n");
		// LRU page replacement algorithm.
		// it returns the console output as a list
		// which is added to the main output list
		ArrayList<String> outLRU = LRU.calculateLRU(processList); 		
		outputData.addAll(outLRU);
		processList.clear();

		processList = restore(processes);
		outputData.add("\n----------------------LFU-------------------\n");
		// LFU page replacement algorithm.
		// it returns the console output as a list
		// which is added to the main output list
		ArrayList<String> outLFU = LFU.calculateLFU(processList); 	
		outputData.addAll(outLFU);
		processList.clear();

		processList = restore(processes);
		outputData.add("\n----------------------MFU-------------------\n");
		//MFU page replacement algorithm 
		ArrayList<String> outMFU = MFU.calculateMFU(processList); 
		outputData.addAll(outMFU);
		processList.clear();
		
		processList = restore(processes);
		outputData.add("\n----------------------Random pick-------------------\n");
		//Random Pick page replacement algorithm 
		ArrayList<String> outRandomPick = RandomPick.calculateRandomPick(processList); 
		outputData.addAll(outRandomPick);
		
		FileWriter writer = new FileWriter("output.doc"); 
		//writing the data in the output list in an output file
		for (String str : outputData) {
		  writer.write(str);
		}
		writer.close();
		outputData.clear();
	}

	//displaying the process data
	private static void printList(Process[] processList) {
		outputData.add("Pid\tArrival_time\tService_time\t\tPage_Size\n");
		// Printing Header Line
		for ( int i = 0; i < processList.length; i++) {
			// Print Data
			outputData.add(processList[i].processId + "\t" + processList[i].arrivalTime + "\t\t\t" + processList[i].serviceTime + "\t\t\t" + processList[i].pageSize + "\n");
		}
	}
	
	//sorting by arrival time
	public static void sortByArrivalTime(Process[] processList) {
		// Bubble Sort algorithm
		for (int i = 0; i < processList.length - 1; i++) {                      
			for(int j = 0; j < processList.length - 1; j++) {
				if (processList[j].arrivalTime > processList[j + 1].arrivalTime) {
					// Swapping stage of Bubble Sort
					Process temp = processList[j];                    
					processList[j] = processList[j + 1];
					processList[j + 1] = temp;
				}
			}
		}
	}
	
	// Function used to restore value to their default and/or initial values using temporary values
    public static ArrayList<Process> restore(Process restoreProcessList[]) {
    	ArrayList<Process> result = new ArrayList<>();
        for (int i = 0; i < restoreProcessList.length; i++) {
        	restoreProcessList[i].serviceTime = restoreProcessList[i].originalServiceTime;
        	restoreProcessList[i].addedFlag = true;
        	result.add(restoreProcessList[i]);
        }
        return result;
    }
}
