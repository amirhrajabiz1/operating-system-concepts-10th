import java.util.LinkedList;
import java.util.List;

/**
 * A simple thread pool API.
 * 
 * Tasks that wish to get run by the thread pool must implement the
 * java.lang.Runnable interface.
 */

public class ThreadPool
{
  	private final List<WorkerThread> threads;
	private final LinkedList<Runnable> taskQueue;

	/**
	 * Create a default size thread pool.
 	 */
	public ThreadPool() {
	  this(5); // Default pool size of 5 threads
    	}
	
	
	/**
	 * Create a thread pool with a specified size.
	 * 
	 * @param int size The number of threads in the pool.
	 */
	public ThreadPool(int size) {
	  threads = new LinkedList<>();
	  taskQueue = new LinkedList<>();

	  for (int i = 0; i < size; i++) {
	    WorkerThread thread = new WorkerThread();
	    threads.add(thread);
	    thread.start();
	  }
    	}
	
	
	/**
	 * shut down the pool.
	 */
	public void shutdown() {
	  for (WorkerThread thread : threads) {
	    thread.interrupt(); // Interrupt each thread
	  }
	}
	
	/**
	 * Add work to the queue.
	 */
	public void add(Runnable task) {
	  synchronized (taskQueue) {
	    taskQueue.add(task);
	    taskQueue.notify(); // Notify waiting threads that work is available
	  }
	}

	private class WorkerThread extends Thread {
	  @Override
	  public void run() {
	    while (!isInterrupted()) {
	      Runnable task = null;
	      synchronized (taskQueue) {
		// Wait until there's work in the queue
		while (taskQueue.isEmpty()) {
		  try {
		    taskQueue.wait();
		  } catch (InterruptedException e) {
		    // Handle interruption
		    interrupt();
		    return;
		  }
		}
		// Get the next task from the queue
		task = taskQueue.poll();
	      }
	      // Execute the task
	      try {
		task.run();
	      } catch (RuntimeException e) {
		// Handle any unexpected runtime exception from tasks
		e.printStackTrace();
	      }
	    }
	  }
	}
}
