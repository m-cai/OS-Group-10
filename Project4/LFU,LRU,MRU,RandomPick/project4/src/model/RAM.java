package model;

import java.util.LinkedList;

public class RAM {
	public LinkedList<Page> freePages=new LinkedList<>();
	public int total_no_of_free_pages = 100;
	public int available_size = 100;
	private int leastRecentlyUsedPageIndex = -1;
	public Page evicted;

	//method to update available size whenever a process uses a page
	public void updateSize() {
		available_size = available_size - 1;
	}

	public int setLRUIndexInRAMToPageHavingPageID(int pageID) {
		return this.leastRecentlyUsedPageIndex = pageID;
	}

	public int getLRUIndexOfPageInRAM() {
		return this.leastRecentlyUsedPageIndex;
	}

	public void setLastAccessedTimeOfPage(Page page,int time) {
	    if(check(page)>=0) {
	       freePages.get(check(page)).lastAccessed=time;
		}
	}

	public void updateLRUIndexAndLastAccessTimeForPageWithTime(Page page, int time) {
		this.setLastAccessedTimeOfPage(page, time);
		int pageIndexWithLeastAccessTime = -1,leastAccessTime=9999;
		for (int pageIndex=0; pageIndex<freePages.size(); pageIndex++) {
			if ((freePages.get(pageIndex).lastAccessed < leastAccessTime) && (freePages.get(pageIndex).lastAccessed != -1)) {
				leastAccessTime = freePages.get(pageIndex).lastAccessed;
				pageIndexWithLeastAccessTime=pageIndex;
			}
		}
		this.setLRUIndexInRAMToPageHavingPageID(pageIndexWithLeastAccessTime);
	}
	
	//method to check if page is already being used by the same process
	public int check(Page page) {
		int index = -1;
		for (int i = 0; i < freePages.size(); i++) {
			if(page.pageNo == freePages.get(i).pageNo && page.usedBy == freePages.get(i).usedBy) {
				index = i;
			}
		}
		return index;
	}

	//method remove the process from the disk and update available size by adding 1 to it for each page being freed.
	public void removeProcess(Process p,RAM disk) {
		for (int i = 0; i < disk.freePages.size(); i++) {
			if(disk.freePages.get(i).usedBy == p.processId) {
				disk.freePages.remove(i);
				i--;
			}
		}
		disk.available_size = 100 - disk.freePages.size();
	}

	//to display details like used by and page no if the page is evicted
    public String display() {
        String s = "";
        try {
            s = evicted.pageNo + " of Process ID " + evicted.usedBy;
        }
        catch(Exception e) {
            s = "None";
        }
         return s;
    }
	
	public int get_available_size() {
		available_size = 100 - freePages.size();
		return available_size; 
	}

	public void reloadSize(int reload) {
		available_size = reload;
	}
	
	public void free(Process p) {
		for(int i = 0; i < freePages.size(); i++) {
		    if(freePages.get(i).usedBy == p.processId) {
				freePages.remove(i);
				i--;
		    }
        }
        available_size = 100 - freePages.size();
    }
}
