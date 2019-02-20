package model;
import java.util.LinkedList;

public class Process {
	public int processId;
    public int arrivalTime;
    public int serviceTime;
    public int originalServiceTime;
    public int pageSize;
    public boolean addedFlag;
    public int lastReferencedPageNo;
    public LinkedList<Page> pagesOnDisk; 
	public Process(int processId, int arrivalTime, int serviceTime, int pageSize) {
		this.processId = processId;
		this.arrivalTime = arrivalTime;
		this.serviceTime = serviceTime;
		this.originalServiceTime = serviceTime;
		this.pageSize = pageSize;
		pagesOnDisk = new LinkedList<Page>();
		addedFlag = true;
        for(int i = 0; i < pageSize; i++) {
        	pagesOnDisk.add(new Page(i,this.processId));
        }
	}
}
