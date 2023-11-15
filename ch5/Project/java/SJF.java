import java.util.List;

/**
 * This class implements the Shortest Job First (SJF) scheduling algorithm.
 * In SJF, the task with the shortest burst time is scheduled next. This approach
 * aims to minimize the average waiting time for all tasks in the queue.
 * Note: This implementation assumes that the burst time for each task is known in advance.
 */
public class SJF implements Algorithm {
    private List<Task> queue; // Queue of tasks to be scheduled
    private CPU cpu = new CPU(); // Instance of CPU to run tasks

    /**
     * Constructor to initialize the SJF scheduler with a list of tasks.
     *
     * @param queue The list of tasks to be scheduled.
     */
    public SJF(List<Task> queue) {
        this.queue = queue;
    }

    /**
     * Method to start the scheduling process.
     * Tasks are scheduled based on the length of their burst time, with shorter tasks being prioritized.
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
     * Method to pick the next task based on the shortest burst time.
     * The task with the shortest burst time in the queue is selected.
     *
     * @return The next Task with the shortest burst time, or null if the queue is empty.
     */
    @Override
    public Task pickNextTask() {
        if (!queue.isEmpty()) {
            // Start with the first task as the shortest burst task
            Task shortestBurstTask = queue.get(0);
            for (Task task : queue) {
                // Check if the current task has a shorter burst time
                if (task.getBurst() < shortestBurstTask.getBurst()) {
                    shortestBurstTask = task;
                }
            }
            return shortestBurstTask;
        }
        return null; // Return null when the queue is empty
    }
}

