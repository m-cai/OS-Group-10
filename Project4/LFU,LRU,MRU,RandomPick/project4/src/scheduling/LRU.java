package scheduling;

import java.util.ArrayList;
import model.Page;
import model.Process;
import model.RAM;

public class LRU {
	public static ArrayList<String> outputLRU=new ArrayList<>();
	public static ArrayList<String> calculateLRU(ArrayList<Process> processList) {
		outputLRU.clear();
		int pageHit = 0;
		int pageMiss = 0;
		int time = 0;
		RAM ramObj = new RAM();
		ArrayList<Process> runningStateList = new ArrayList<>();
		while (time < 60) {
			int originalAvailableSize = ramObj.available_size;
			outputLRU.add("\n" + time + "\n");
			//logic to add process to the running list based on arrival time and available ram space
			for (int i = 0; i < processList.size(); i++) {
				//check if the process has arrived and the available size in RAM is greater than 4
				if (processList.get(i).arrivalTime <= time && ramObj.available_size >= 4) {
					int id = processList.get(i).processId;
					runningStateList.add(processList.get(i));//add the process to the running process list
					processList.remove(i);//once added to the running list remove it from the process list
					outputLRU.add(" -- Process id " + id + " added to memory\n");
					ramObj.updateSize();//update available size in the RAM: available size=available size-1
					i--;
				}
			}
			ramObj.available_size = originalAvailableSize;
			outputLRU.add("\n");
			time++;//update time once the process is added to the running list
			
			//run process to add page to the memory and calculate hits or miss
			if (!runningStateList.isEmpty()) {
				for (int i = 0; i < 10; i++) {
					outputLRU.add("\t" + (time - 1) + "." + (i) + "\t");
	                for (int j = 0; j < runningStateList.size(); j++) {
	                	 // first time to run process and no pages has been added to the frames by checking the addedFlag counter
	                    if (runningStateList.get(j).serviceTime == runningStateList.get(j).originalServiceTime && runningStateList.get(j).addedFlag) {
	                    	//mark the process has been added to the frame
							runningStateList.get(j).addedFlag = false;
							//check if the page was added to the list
							//and if hit, the value is true, else false.					                   
	                        boolean isHit = checkPageInRAM(runningStateList.get(j).pagesOnDisk.get(0),ramObj,(time - 1) * 10 + i * 10);					
	                        if (isHit) {
	                        	outputLRU.add("Process ID " + runningStateList.get(j).processId 
								+ " requested for page number " + 0 + " it was a hit \n");
	                            pageHit++;//update the pageHit counter
	                        } else {
	                        	outputLRU.add("Process ID " + runningStateList.get(j).processId
								+ " requested for page number " + 0 + " it was a miss | Page evicted was " + ramObj.display() + "\n");
	                            pageMiss++;//update the pageMiss counter
	                        }
							//0 as it is for the first time being added to the page frame
	                        runningStateList.get(j).lastReferencedPageNo = 0;
	                    } else {
	                        //locality of reference:get page number based on the locality of ref algorithm
	                        int new_page_no = ScheduleHelper.randomNoByLocalityRef(runningStateList.get(j).lastReferencedPageNo, runningStateList.get(j).pageSize);
							//add the page and check to see if it is hit or miss 
	                        boolean isHit = checkPageInRAM(runningStateList.get(j).pagesOnDisk.get(new_page_no),ramObj, (time - 1) * 10 + i * 10);
							//last ref page number would be the new page that is being added to the frame  
	                        runningStateList.get(j).lastReferencedPageNo = new_page_no;
	                        if (isHit) {
	                        	outputLRU.add("Process ID " + runningStateList.get(j).processId 
								+ " requested for page number " + new_page_no + " it was a hit \n");
	                            pageHit++;//update hit counter
	                        } else {
	                        	outputLRU.add("Process ID " + runningStateList.get(j).processId
								+ " requested for page number " + new_page_no + " it was a miss | Page evicted was " + ramObj.display() + "\n");
	                            pageMiss++;//update miss counter
	                        }
	                    }
	                }
	                outputLRU.add("\n");
	            }
				
				//logic to remove process and swap page out of memory
				for (int i = 0; i < runningStateList.size(); i++) {
					//decrement the service time during each timer loop
					runningStateList.get(i).serviceTime = runningStateList.get(i).serviceTime - 1;
					//if service time is 0 then the process has finished
					if (runningStateList.get(i).serviceTime == 0) {
						Process removed = runningStateList.remove(i);
						outputLRU.add("\t\tProcess ID " + removed.processId + " is swapped OUT memory !\n");
						//function to remove page from memory and free up memory space
						ramObj.removeProcess(removed,ramObj);
						i--;
					}
				}
			}
		}
		outputLRU.add("Hits =  " + pageHit);
		outputLRU.add("Miss =  " + pageMiss);
		outputLRU.add("Ratio = " + (float)pageHit / pageMiss);
		return outputLRU;
	}
	
	private static boolean checkPageInRAM(Page page, RAM ram, int currentTime) {
		//check if the page is already present in Cache or not(-1) 
		int pageIndex = ram.check(page);
		page.frequency = 1;
		ram.available_size = 100 - ram.freePages.size();
		//indicates that page already present in the memory
		if (pageIndex != -1) {	
			ram.updateLRUIndexAndLastAccessTimeForPageWithTime(page,currentTime);
			return true;
		} else {//page not present in the memory and so has to be added in it
			if (ram.available_size >= 1) { 
				ram.freePages.add(page);//add page to memory as cache has free pages
				ram.setLastAccessedTimeOfPage(page,currentTime);
				if(ram.getLRUIndexOfPageInRAM() == -1){
					//make this page as the the LRU page as this is the first entry to this ptr
					ram.setLRUIndexInRAMToPageHavingPageID(page.pageNo);
				} else {
					ram.updateLRUIndexAndLastAccessTimeForPageWithTime(page, currentTime);
				}
			} else { 
				//if no pages in cache is free to accommodate a new page,  
				//remove the page having the least recently used pointer
				int lruPageIndex = ram.getLRUIndexOfPageInRAM();
				ram.freePages.remove(lruPageIndex);
				ram.freePages.add(page);
				ram.updateLRUIndexAndLastAccessTimeForPageWithTime(page,currentTime);
			}
			//after removing the page with least freq and adding the new page update the available space
			ram.available_size = 100 - ram.freePages.size();
			return false;
		} 
	}
}
