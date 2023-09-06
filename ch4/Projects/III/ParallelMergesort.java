import java.util.concurrent.RecursiveAction;
import java.util.concurrent.ForkJoinPool;

public class ParallelMergesort {
  
  private static final int THRESHOLD = 100;

  public static void main(String[] args) {
    // Parse command-line arguments as integers
    Integer[] array = new Integer[args.length];
    for (int i = 0; i < args.length; i++) {
      array[i] = Integer.parseInt(args[i]);
    }

    ForkJoinPool pool = new ForkJoinPool();

    // Create a task for sorting the entire array
    MergesortTask task = new MergesortTask(array, 0, array.length - 1);

    
    // Submit the task to the ForkJoinPool
    pool.invoke(task);

    
    // Print the sorted array
    for (int num : array) {
      System.out.print(num + " ");
    }
    System.out.println();
  }

  static class MergesortTask extends RecursiveAction {
    private final Integer[] array;
    private final int low;
    private final int high;

    public MergesortTask(Integer[] array, int low, int high) {
      this.array = array;
      this.low = low;
      this.high = high;
    }

    @Override
    protected void compute() {
      if (high - low <= THRESHOLD) {
	// If the sublist size is smaller that the threashhost, use insertion sort
	selectionSort(array, low, high);
      } else {

	// Find the middle point
	int middle = low + (high - low) / 2;

	// Create tasks for the left and right sublists
	MergesortTask left = new MergesortTask(array, low, middle);
	MergesortTask right = new MergesortTask(array, middle + 1, high);

	// Fork the task and let them run in parallel
	invokeAll(left, right);

	// Wait for all tasks to finish
	left.join();
	right.join();

	// Merge the sorted halves
	merge(array, low, middle, high);
      }
    }

      
    void merge(Integer arr[], int l, int m, int r)
    {
      // Find sizes of two subarrays to be merged
      int n1 = m - l + 1;
      int n2 = r - m;

      // Create temp arrays
      int L[] = new int[n1];
      int R[] = new int[n2];

      // Copy data to temp arrays
      for (int i = 0; i < n1; ++i)
	L[i] = arr[l + i];
      for (int j = 0; j < n2; ++j)
	R[j] = arr[m + 1 + j];

      // Merge the temp array

      // Initial indices of first and second subarrays
      int i = 0, j = 0;

      // Initial index of merged subarray array
      int k = l;
      while (i < n1 && j < n2) {
	if (L[i] <= R[j]) {
	  arr[k] = L[i];
	  i++;
	}
	else {
	  arr[k] = R[j];
	  j++;
	}
	k++;
      }

      // Copy remaining elements of L[] if any
      while (i < n1) {
	arr[k] = L[i];
	i++;
	k++;
      }

      // Copy remaining elements of R[] if any
      while (j < n2) {
	arr[k] = R[j];
	j++;
	k++;
      }
    }
    private void swap(Integer[] array, int i, int j) {
      // Swap two elements in the array
      int temp = array[i];
      array[i] = array[j];
      array[j] = temp;
    }

    private void selectionSort(Integer[] array, int low, int high) {
      // Perform selection sort on a small sublist
      for (int i = low; i <= high - 1; i++) {
	int lowestIndex = i; // Initialize the index of the lowest element
	for (int j = i + 1; j <= high; j++) {
	  if (array[j] < array[lowestIndex]) {
	    lowestIndex = j; // Update the index of the lowest element
	  }
	}
	swap(array, i, lowestIndex);
      }
    }

  }
}
