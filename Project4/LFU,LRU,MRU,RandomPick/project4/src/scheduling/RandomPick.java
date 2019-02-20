package scheduling;

import java.util.*;
import java.util.ArrayList;
import model.Process;
import model.RAM;
import model.Page;

public class RandomPick {
	public static ArrayList<String> outputRandomPick = new ArrayList<>();
	public static ArrayList<String> calculateRandomPick(ArrayList<Process> processList) {
		int hits = 0;
		int faults = 0;
		int run_time = 0;
		RAM ram = new RAM();
		ArrayList<Process> running_list_process = new ArrayList<Process>();
		outputRandomPick.clear();
		// Indicating the paging algorithm will run for a minute
		while (run_time < 60) { 
			int temp = ram.get_available_size();
			outputRandomPick.add("\n" + run_time + "\n");
	        for (int i = 0; i < processList.size(); i++) {
	            if (processList.get(i).arrivalTime <= run_time && ram.get_available_size() >= 4) {
					//add the processes into running process list
	                running_list_process.add(processList.remove(i)); 
	                outputRandomPick.add(" -- Process id " + running_list_process.get(i).processId + " added to memory\n");
	                ram.updateSize(); // available_size = available_size - 1
	                i--; // as process is removed from the given index, go back to previous index
	            } else {
	                break;
	            }
	        }
	        ram.reloadSize(temp);
	        outputRandomPick.add("\n");
	        run_time = run_time + 1;
	        if (running_list_process.size() == 0) {
	            continue;
	        }
	        for (int i = 0; i < 10 ;i++) {
	        	outputRandomPick.add("\t" + (run_time - 1) + "." + i + "\t");
	            for (int j = 0; j < running_list_process.size(); j++) {
	                if (running_list_process.get(j).addedFlag) {
	                    running_list_process.get(j).addedFlag = false;
	                    boolean hit_or_miss = random_pick_page_placement(running_list_process.get(j).pagesOnDisk.get(0), ram);
	                    if (hit_or_miss) {
	                    	outputRandomPick.add("Process ID " + running_list_process.get(j).processId
							+ " requested for page number " + 0 + " it was a hit \n");
	                        hits++;
	                    } else {
	                    	outputRandomPick.add("Process ID " + running_list_process.get(j).processId
							+ " requested for page number " + 0 + " it was a miss | Page evicted was " + ram.display() + "\n");
	                        faults++;
	                    }
	                    running_list_process.get(j).lastReferencedPageNo = 0;
	                } else {
	                    //locality of reference
	                    int new_page_no = ScheduleHelper.randomNoByLocalityRef(running_list_process.get(j).lastReferencedPageNo, running_list_process.get(j).pageSize);
	                    boolean hit_or_miss = random_pick_page_placement(running_list_process.get(j).pagesOnDisk.get(new_page_no), ram);
	                    running_list_process.get(j).lastReferencedPageNo = new_page_no;
	                    if (hit_or_miss) {
	                    	outputRandomPick.add("Process ID " + running_list_process.get(j).processId
							+ " requested for page number " + new_page_no + " it was a hit \n");
	                        hits++;
	                    } else {
	                    	outputRandomPick.add("Process ID " + running_list_process.get(j).processId
							+ " requested for page number " + new_page_no + " it was a miss | Page evicted was " + ram.display() + "\n");
	                        faults++;
	                    }
	                }
	            }
	        }
	        for (int i = 0; i < running_list_process.size(); i++) {
	            running_list_process.get(i).serviceTime = running_list_process.get(i).serviceTime - 1;
	            if (running_list_process.get(i).serviceTime == 0) {
	                Process temp_removal = running_list_process.remove(i);
	                outputRandomPick.add("\t\tProcess ID " + temp_removal.processId + " is swapped OUT memory !");
	                ram.free(temp_removal);
	                i--;
	            }
	        }
	    }
		outputRandomPick.add("Hits =  "+hits);
		outputRandomPick.add("Miss =  "+faults);
		outputRandomPick.add("Ratio = "+(float)hits / faults);
		return outputRandomPick;
	}
	
	public static boolean random_pick_page_placement(Page new_page, RAM ram) {
	    int index = ram.check(new_page);
	    ram.available_size = 100 - ram.freePages.size();
		// check if page already present in the ram, if yes return true
	    if (index != -1) {
	        return true;
	    } else {// if page is not in memory	
			// if ram size is enough to add the page, then add it
	        if (ram.available_size >= 1) {
	            ram.freePages.add(new_page);
	            ram.available_size = 100 - ram.freePages.size();
	        } else {
				// randomly pick a page to remove that does not match the index used by the new_page
	            Random page_no = new Random();
	            while(true) {
	                int random_index = page_no.nextInt(ram.freePages.size());
	                Page temp = ram.freePages.get(random_index);
	                if(temp.usedBy != new_page.usedBy) {
	                    ram.freePages.remove(random_index);
	                    break;
	                }
	            }
	            ram.freePages.add(new_page);
	            ram.available_size = 100 - ram.freePages.size();
	        }
	    }
	    return false;
	}
}
