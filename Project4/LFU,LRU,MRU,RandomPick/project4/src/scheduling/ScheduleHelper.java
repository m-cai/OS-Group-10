package scheduling;
import java.util.Random;

public class ScheduleHelper {
	//assigning processes based on locality ref algorithm
	public static int randomNoByLocalityRef(int currentPageNo, int size) {
		Random a = new Random();
		if(currentPageNo == 0) {
			if (Math.random() < 0.7) {
				//random value will be 0 or 1. cannot be -1 as current page number is 0
				return a.nextInt(2);
			} else {
				while(true) {//continue till you get a value that is not 0 or 1
					int temp = a.nextInt(size);
					if (temp > 1) {
						return temp;
					}
				}
			}
		} else if (currentPageNo == size - 1) {
			if (Math.random() < 0.7) {//return number that is either currentPage no or currentPageno-1
				if (a.nextInt(2) == 0) {
					return (size-1);
				}
				return (size-2);
			} else {
				while(true) {//loop until value is not size-1 or size-2
					int temp = a.nextInt(size);
					if (temp != (size - 1) && temp != (size - 2)) {
						return temp;
					}
				}
			}
		} else if (Math.random() < 0.7) {
			switch(a.nextInt(3)) {
			case 0: return currentPageNo;
			case 1: return (currentPageNo - 1);
			case 2: return (currentPageNo + 1);
			}
		} else {
			while(true) {//loop till u get delta value other than -1,0,+1
				int temp = a.nextInt(size);
				if (temp != (currentPageNo - 1) && temp != (currentPageNo - 2) && temp != currentPageNo) {
					return temp;
				}
			}
		}
		return - 1;
	}
}
