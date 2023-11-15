import java.util.List;

/**
 * This class implements the Round-Robin (PriorityRR) scheduling algorithm.
 * In PriorityRR scheduling, processes are dispatched in a FIFO manner but are interrupted
 * after a specific time quantum has elapsed. If a process still has burst time
 * remaining, it is placed at the back of the queue and the next process is scheduled.
 */
public class PriorityRR implements Algorithm {
    private List<Task> queue; // Queue of tasks to be scheduled
    private static final int TIME_QUANTUM = 10; // Time quantum for each task

    /**
     * Constructor to initialize the PriorityRR scheduler with a list of tasks.
     *
     * @param queue The list of tasks to be scheduled.
     */
    public PriorityRR(List<Task> queue) {
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
     * Method to pick the next task based on priority.
     * The task with the highest priority in the queue is selected.
     * If multiple tasks have the same priority, the first one in the queue is selected.
     *
     * @return The next Task with the highest priority, or null if the queue is empty.
     */
    @Override
    public Task pickNextTask() {
        if (!queue.isEmpty()) {
            // Start with the first task as the highest priority
            Task highestPriorityTask = queue.get(0);
            for (Task task : queue) {
                // Select the task with higher priority
                if (task.getPriority() > highestPriorityTask.getPriority()) {
                    highestPriorityTask = task;
                }
            }
            return highestPriorityTask;
        }
        return null; // Return null when the queue is empty
    }
}
