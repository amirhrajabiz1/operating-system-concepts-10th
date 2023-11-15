import java.util.List;

/**
 * This class implements a Priority scheduling algorithm.
 * In Priority scheduling, each task has a priority associated with it,
 * and the task with the highest priority is selected for execution first.
 * Tasks with equal priority are scheduled in the order they appear in the queue.
 * Note: The implementation assumes that higher numerical values represent higher priorities.
 */
public class Priority implements Algorithm {
    private List<Task> queue; // Queue of tasks to be scheduled
    private CPU cpu = new CPU(); // Instance of CPU to run tasks

    /**
     * Constructor to initialize the Priority scheduler with a list of tasks.
     *
     * @param queue The list of tasks to be scheduled.
     */
    public Priority(List<Task> queue) {
        this.queue = queue;
    }

    /**
     * Method to start the scheduling process.
     * Tasks are scheduled based on their priority.
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

