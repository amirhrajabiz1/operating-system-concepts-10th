#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/pid.h>

#define PROC_NAME "pid"		// Name of the proc file
#define BUFFER_SIZE 64		// Size of the buffer for reading/writing data

static long pid_value;		// Variable to store the PID value
static struct proc_dir_entry *proc_entry;

// Function declarations
ssize_t proc_read(struct file *file, char *buffer, size_t count, loff_t *pos);
ssize_t proc_write(struct file *file, const char *buffer, size_t count, loff_t *pos);

// File operations structure for the proc file
static struct file_operations proc_ops = {
  .owner = THIS_MODULE,
  .read = proc_read,
  .write = proc_write,
};

// Initialization function called when the module is loaded
int task_info_init(void) {
  // Create the proc entry with the given name, permissions, and file operations
  proc_entry = proc_create(PROC_NAME, 0666, NULL, &proc_ops);
  if (proc_entry == NULL) {
    return -ENOMEM;		// Return error if proc entry creation fails
  }

  return 0;			// Return success
}

// Cleanup function called when the module is unloaded
void task_info_exit(void) {
  proc_remove(proc_entry);	// Remove the proc entry
}

// Read function for the proc file
ssize_t proc_read(struct file *file, char *buffer, size_t count, loff_t *pos) {
  int error_count = 0;
  char output[BUFFER_SIZE];
  struct pid *proc_pid;
  struct task_struct *task;

  if (*pos > 0) {
    return 0;			// Return 0 to indicate end-of-file if the position is not 0
  }

  proc_pid = find_vpid(pid_value); // Find the PID structure for the given PID value
  if (!proc_pid) {
    return 0;			// Return 0 if the PID structure is not found
  }

  task = pid_task(proc_pid, PIDTYPE_PID); // Get the task_struct for given PID
  if (!task) {
    return 0;			// Return 0 if the task_struct is not found
  }

  // Format the task information into the output buffer
  snprintf(output, BUFFER_SIZE, "command = [%s] pid = [%ld] state = [%ld]\n", task->comm, task->pid, task->state);

  // Copy the output buffer to the user space buffer
  error_count = copy_to_user(buffer, output, strlen(output));

  if (error_count == 0) {
    *pos = strlen(output);	// Update the position
    return strlen(output);	// Return the number of bytes read
  }

  return -EFAULT;		// Return error if copying to user space failed
}

// Write function for the proc file
ssize_t proc_write(struct file *file, const char *buffer, size_t count, loff_t *pos) {
  char *k_buffer;
  int error_count;

  k_buffer = kmalloc(count + 1, GFP_KERNEL); // Allocate kernel memory for the input buffer
  if (!k_buffer) {
    return -ENOMEM;		// Return error if memory allocation fails
  }

  error_count = copy_from_user(k_buffer, buffer, count); // Copy data from user space to kernel space
  if (error_count != 0) {
    kfree(k_buffer);
    return -EFAULT;		// Return error if copying from user space fails
  }

  k_buffer[count] = '\0';	// Null-teminate the kernel buffer
  kstrtol(k_buffer, 10, &pid_value); // Convert the input string to a long value (PID)
  kfree(k_buffer);		// Free the kernel buffer memory

  return count;			// Return the number of bytes written
}

module_init(task_info_init);	// Specify the initialization function
module_exit(task_info_exit);	// Specify the cleanup function


MODULE_LICENSE("GPL");		// License type
MODULE_DESCRIPTION("Task Infomation Kernel Module");		// Description of the module
MODULE_AUTHOR("Amir");		// Author information

