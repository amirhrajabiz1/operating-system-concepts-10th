import java.util.List;

/**
 * This class implements the Round-Robin (RR) scheduling algorithm.
 * In RR scheduling, processes are dispatched in a FIFO manner but are interrupted
 * after a specific time quantum has elapsed. If a process still has burst time
 * remaining, it is placed at the back of the queue and the next process is scheduled.
 */
public class RR implements Algorithm {
    private List<Task> queue; // Queue of tasks to be scheduled
    private static final int TIME_QUANTUM = 10; // Time quantum for each task

    /**
     * Constructor to initialize the RR scheduler with a list of tasks.
     *
     * @param queue The list of tasks to be scheduled.
     */
    public RR(List<Task> queue) {
        this.queue = queue;
    }

    private CPU cpu = new CPU(); // Instance of CPU to run tasks

    /**
     * Method to start the scheduling process.
     * Tasks are scheduled and executed based on Round-Robin algorithm.
     */
    @Override
    public void schedule() {
        while (!queue.isEmpty()) {
            Task task = pickNextTask();
            if (task == null) { // Handling the case when the queue is empty
                break;
            }

            // Run the task for a time slice or its remaining burst time
            int timeSlice = Math.min(task.getBurst(), TIME_QUANTUM);
            cpu.run(task, timeSlice);
            task.setBurst(task.getBurst() - timeSlice);

            // Re-queue the task if it still has burst time remaining
            if (task.getBurst() > 0) {
                queue.add(task);
            }
            queue.remove(task); // Remove the task from the front of the queue
        }
    }

    /**
     * Method to pick the next task from the queue.
     * The first task in the queue is selected.
     *
     * @return The next Task to be scheduled, or null if the queue is empty.
     */
    @Override
    public Task pickNextTask() {
        // Returns the first task in the queue or null if the queue is empty
        return queue.isEmpty() ? null : queue.get(0);
    }
}

