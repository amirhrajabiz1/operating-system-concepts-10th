#include <linux/version.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
#include <linux/signal.h>
#else
#include <linux/sched/signal.h>
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("OpenAI");
MODULE_DESCRIPTION("A kernel module that lists all current tasks in a Linux system");
MODULE_VERSION("0.1");

static void dfs_iterate_tasks(struct task_struct *task)
{
  struct task_struct *child;
  struct list_head *list;

  printk(KERN_INFO "%d\t\t%ld\t\t%s\n", task->pid, task->state, task->comm);

  list_for_each(list, &task->children) {
    child = list_entry(list, struct task_struct, sibling);
    dfs_iterate_tasks(child);
  }
}

static int __init task_list_init(void) {
  struct task_struct *task;

  printk(KERN_INFO "Loading task_list module...\n");
  printk(KERN_INFO "PID\t\tSTATE\t\tCOMMAND\n");

  dfs_iterate_tasks(&init_task);

  return 0;
}

static void __exit task_list_exit(void) {
  printk(KERN_INFO "Unloading task_list module...\n");
}

module_init(task_list_init);
module_exit(task_list_exit);
