package scheduling;

import java.util.ArrayList;
import model.Page;
import model.Process;
import model.RAM;

public class MFU {
	public static ArrayList<String> outputMFU = new ArrayList<>();
	public static ArrayList<String> calculateMFU(ArrayList<Process> processList) {
		outputMFU.clear();
		int pageHit = 0;
		int pageMiss = 0;
		int time = 0;
		RAM ramobj = new RAM();
		ArrayList<Process> runningStateList = new ArrayList<>();
		while (time < 60) {
			int temp = ramobj.available_size;
			outputMFU.add("\n" + time + "\n");			
			//logic to add process to the running list based on arrival time and available ram space
			for (int i = 0; i < processList.size(); i++) {
				//check if the process has arrived and the available size in RAM is greater than 4
				if (processList.get(i).arrivalTime <= time && ramobj.available_size >= 4) {
					int id = processList.get(i).processId;
					runningStateList.add(processList.get(i));//add the process to the running process list
					processList.remove(i);//once added to the running list remove it from the process list
					outputMFU.add(" -- Process id " + id + " added to memory\n");
					ramobj.updateSize();//update available size in the RAM: available size=avaialble size-1
					i--;
				} else
				break;
			}
			ramobj.available_size = temp;
			outputMFU.add("\n");
			time++;//update time once the process is added to the running list
			
			//run process to add page to the memory and calculate hits or miss
			if (runningStateList.isEmpty()) {
				continue;
			} else {
				for (int i = 0; i < 10;i++) {
					outputMFU.add("\t" + (time - 1) + "." + (i) + "\t");
	                for (int j = 0; j < runningStateList.size(); j++) {
	                	// first time to run process and no pages has been added to the frames by checking the addedFlag counter
	                   if(runningStateList.get(j).serviceTime == runningStateList.get(j).originalServiceTime 
							&& runningStateList.get(j).addedFlag)
						{
							//mark the process has been added to the frame
	                    	runningStateList.get(j).addedFlag = false;
							//check if the page was added to the list
							//and if hit, the value is true, else false.	
	                        boolean hit_or_miss = placePageMFU(runningStateList.get(j).pagesOnDisk.get(0), ramobj);		
	                        if (hit_or_miss) {
	                        	outputMFU.add("Process ID " + runningStateList.get(j).processId
								+ " requested for page number " + 0 + " it was a hit \n");
	                            pageHit++;//update the pageHit counter
	                        } else {
	                        	outputMFU.add("Process ID " + runningStateList.get(j).processId
								+ " requested for page number " + 0 + " it was a miss | Page evicted was " + ramobj.display() + "\n");
	                            pageMiss++;//update the pageMiss counter
	                        }
							//0 as it is for the first time being added to the page frame
	                        runningStateList.get(j).lastReferencedPageNo = 0;
	                    } else {
	                        //locality of reference:get random page number based on th elocality of ref algorithm
	                        int new_page_no = ScheduleHelper.randomNoByLocalityRef(runningStateList.get(j).lastReferencedPageNo, runningStateList.get(j).pageSize);
							//add the page and check to see if it is hit or miss 
	                        boolean hit_or_miss = placePageMFU(runningStateList.get(j).pagesOnDisk.get(new_page_no), ramobj);
							//last ref page number would be the new page taht is being added to the frame
	                        runningStateList.get(j).lastReferencedPageNo = new_page_no;
	                        if (hit_or_miss) {
	                        	outputMFU.add("Process ID " + runningStateList.get(j).processId
								+ " requested for page number " + new_page_no + " it was a hit \n");
	                            pageHit++;//update hit counter
	                        }
	                        else {
	                        	outputMFU.add("Process ID " + runningStateList.get(j).processId
								+ " requested for page number " + new_page_no + " it was a miss | Page evicted was " + ramobj.display() + "\n");
	                            pageMiss++;//update miss counter
	                        }
	                    }
	                }
	                outputMFU.add("\n");
	            }
				
				//logic to remove process and swap page out of memory
				for (int i = 0; i < runningStateList.size(); i++) {
					//decrement the service time during each timer loop
					runningStateList.get(i).serviceTime = runningStateList.get(i).serviceTime - 1;
					//if service time is 0 then the page needs to be swaaped out
					if(runningStateList.get(i).serviceTime == 0) {
						Process removed = runningStateList.remove(i);
						outputMFU.add("\t\tProcess ID " + removed.processId + " is swapped OUT memory !\n");
						//function to remove page from memory and free up memory space
						ramobj.removeProcess(removed, ramobj);
						i--;
					}
				}
			}
		}
		outputMFU.add("Hits =  " + pageHit);
		outputMFU.add("Miss =  " + pageMiss);
		outputMFU.add("Ratio = " + (float)pageHit / pageMiss);
		return outputMFU;
	}

	private static boolean placePageMFU(Page page, RAM ram) {
		//check if the page is added for the first time or already present in memory
		int index = ram.check(page);
		page.frequency = 1;
		ram.available_size = 100 - ram.freePages.size();
		//indicates that page already present in the memory
		if(index != -1) {
			ram.freePages.get(index).frequency++;//update frequency counter
			return true;
		} else {//page not present in the memory and so has to be added in it
			if (ram.available_size >= 1) {
				ram.freePages.add(page);//add page to memory as frames are free
			} else {
				//if no frame is free remove page with the least frequency
				int maxFrequencyIndex = 0;
				for (int i = 0; i < ram.freePages.size(); i++) {
					if (ram.freePages.get(i).frequency > ram.freePages.get(maxFrequencyIndex).frequency 
							&& ram.freePages.get(i).usedBy != page.usedBy)
					{
						maxFrequencyIndex = i;
					}
				}
				ram.freePages.remove(maxFrequencyIndex);
				ram.freePages.add(page);
			}
			//after removing the page with least freq and adding the new page update the available space
			ram.available_size=100-ram.freePages.size();
		}
		return false;
	}
}
