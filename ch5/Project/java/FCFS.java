import java.util.List;

/**
 * This class implements the First-Come, First-Served (FCFS) scheduling algorithm.
 * In FCFS, the tasks are scheduled in the order they arrive in the task queue.
 * The first task in the queue is selected for execution, and once it finishes,
 * the next task in the queue is processed. This process continues until the queue is empty.
 */
public class FCFS implements Algorithm {
    private List<Task> queue; // Queue of tasks to be scheduled
    private CPU cpu = new CPU(); // Instance of CPU to run tasks

    /**
     * Constructor to initialize the FCFS scheduler with a list of tasks.
     *
     * @param queue The list of tasks to be scheduled.
     */
    public FCFS(List<Task> queue) {
        this.queue = queue;
    }

    /**
     * Method to start the scheduling process.
     * Tasks are scheduled and executed in the order they appear in the queue.
     */
    @Override
    public void schedule() {
        while (!queue.isEmpty()) {
            Task task = pickNextTask();
            if (task == null) { // Handling the case when the queue is empty
                break;
            }

            // Run the task for its entire burst time
            cpu.run(task, task.getBurst());
            // Remove the task from the queue
            queue.remove(task);
        }
    }

    /**
     * Method to pick the next task from the queue.
     * The first task in the queue is selected as it follows FCFS policy.
     *
     * @return The next Task to be scheduled, or null if the queue is empty.
     */
    @Override
    public Task pickNextTask() {
        // Returns the first task in the queue or null if the queue is empty
        return queue.isEmpty() ? null : queue.get(0);
    }
}

