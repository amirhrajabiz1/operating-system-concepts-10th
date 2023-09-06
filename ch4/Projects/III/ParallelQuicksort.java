import java.util.concurrent.RecursiveAction;
import java.util.concurrent.ForkJoinPool;

public class ParallelQuicksort {
  
  private static final int THRESHOLD = 100;

  public static void main(String[] args) {
    // Parse command-line arguments as integers
    Integer[] array = new Integer[args.length];
    for (int i = 0; i < args.length; i++) {
      array[i] = Integer.parseInt(args[i]);
    }

    ForkJoinPool pool = new ForkJoinPool();

    // Create a task for sorting the entire array
    QuicksortTask task = new QuicksortTask(array, 0, array.length - 1);

    
    // Submit the task to the ForkJoinPool
    pool.invoke(task);

    
    // Print the sorted array
    for (int num : array) {
      System.out.print(num + " ");
    }
    System.out.println();
  }

  static class QuicksortTask extends RecursiveAction {
    private final Integer[] array;
    private final int low;
    private final int high;

    public QuicksortTask(Integer[] array, int low, int high) {
      this.array = array;
      this.low = low;
      this.high = high;
    }

    @Override
    protected void compute() {
      if (high - low <= THRESHOLD) {
	// If the sublist size is smaller that the threashhost, use insertion sort
	insertionSort(array, low, high);
      } else {
	// Perform the partitioning step of quicksort
	int pivotIndex = partition(array, low, high);

	// Create tasks for the left and right sublists
	QuicksortTask left = new QuicksortTask(array, low, pivotIndex - 1);
	QuicksortTask right = new QuicksortTask(array, pivotIndex + 1, high);

	// Fork the task and let them run in parallel
	invokeAll(left, right);
      }
    }

    private int partition(Integer[] array, int low, int high) {
      // Choose the pivot element (usually the last element)
      int pivot = array[high];

      int i = low - 1;

      // Rearrange elements so that elements smaller than the pivot are on the left
      // and elements greater that or equal to the pivot are on the right
      for (int j = low; j < high; j++) {
	if (array[j] < pivot) {
	  i++;
	  swap(array, i, j);
	}
      }

      // Place the pivot in its correct position
      swap(array, i + 1, high);

      return i + 1; // Return the index of pivot
    }

    private void swap(Integer[] array, int i, int j) {
      // Swap two elements in the array
      int temp = array[i];
      array[i] = array[j];
      array[j] = temp;
    }

    private void insertionSort(Integer[] array, int low, int high) {
      // Perform insertion sort on a small sublist
      for (int i = low + 1; i <= high; i++) {
	int current = array[i];
	int j = i - 1;

	// Shift elements to the right until the corrent position for 'current' is found
	while (j >= low && array[j] > current) {
	  array[j + 1] = array[j];
	  j--;
	}

	// Place 'current' int the current position
	array[j + 1] = current;
      }
    }
  }
}
