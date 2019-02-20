package model;

public class Page {
	public int pageNo;
	public int usedBy;
    public int frequency;
    public int lastAccessed;
	public Page (int pageNo, int usedBy) {
		this.pageNo = pageNo;
		this.usedBy = usedBy;
		this.frequency = 0;
		this.lastAccessed = -1;
	}
}
