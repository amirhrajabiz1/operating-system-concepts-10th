#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Collatz sequence kernel module");

// Define the color structure
struct color {
    int red;
    int blue;
    int green;
    struct list_head list;
};

// Declare and initialize the color list
static LIST_HEAD(color_list);

// Module parameter
static int start = 25;
module_param(start, int, 0);

// Function to generate the Collatz sequence
static void generate_collatz_sequence(int start_val) {
    struct color *new_color;
    int curr = start_val;

    while (curr != 1) {
        new_color = kmalloc(sizeof(*new_color), GFP_KERNEL);
        if (!new_color) {
            return;
        }

        new_color->red = curr % 256;
        new_color->blue = (curr * 2) % 256;
        new_color->green = (curr * 3) % 256;

        INIT_LIST_HEAD(&new_color->list);
        list_add_tail(&new_color->list, &color_list);

        if (curr % 2 == 0) {
            curr /= 2;
        } else {
            curr = 3 * curr + 1;
        }
    }
}

static int __init collatz_init(void) {
    printk(KERN_INFO "Collatz module loaded with start=%d\n", start);

    // Generate the Collatz sequence and store it in the color list
    generate_collatz_sequence(start);

    // Traverse the color list and print its contents
    struct color *ptr;
    list_for_each_entry(ptr, &color_list, list) {
        printk(KERN_INFO "Color: (R: %d, G: %d, B: %d)\n", ptr->red, ptr->green, ptr->blue);
    }

    return 0;
}

static void __exit collatz_exit(void) {
    struct color *ptr, *next;

    // Remove all elements from the color list and free the memory
    list_for_each_entry_safe(ptr, next, &color_list, list) {
        list_del(&ptr->list);
        kfree(ptr);
    }

    printk(KERN_INFO "Collatz module unloaded\n");
}

module_init(collatz_init);
module_exit(collatz_exit);
