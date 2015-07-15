/*
 * @file                : char device driver
 * @brief               : This is the source code for implementing the
                          basic char device driver with semaphores for read and write
 * @author              : Sanjay kumar U
 * @Copyright           : (c) 2014 , VVDN Technologies Pvt. Ltd.
                          Permission is hereby granted to everyone in VVDN Technologies
                          to use the Software without restriction, including without
                          limitation the rights to use, copy, modify, merge, publish,
                          distribute, distribute with modifications.
 */


/* Necessary includes for device drivers */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/file.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <asm/uaccess.h> /* copy_from/to_user */
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>


MODULE_LICENSE("Dual BSD/GPL");
dev_t first;
static int major;
/* Declaration of memory.c functions */
static int memory_open(struct inode *inode, struct file *filp);
static int memory_release(struct inode *inode, struct file *filp);
static ssize_t memory_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
static ssize_t memory_write(struct file *filp,const char *buf, size_t count, loff_t *f_pos);
void memory_exit(void);
int memory_init(void);

/* Structure that declares the usual file */
/* access functions */
struct file_operations memory_fops = {
  read: memory_read,
  write: memory_write,
  open: memory_open,
  release: memory_release
};

/* Declaration of the init and exit functions */
module_init(memory_init);
module_exit(memory_exit);

struct semaphore sem;
struct cdev *kernel_cdev;
char memory_buffer[64];


int memory_init(void) {
  int result;

  /* Registering device */
  result = alloc_chrdev_region(&first,0,1,"device2");
  if (result < 0) {
    printk("<1>memory: cannot obtain major number ");
    return -1;
  }
  major = MAJOR(first);
  printk(KERN_INFO "The major number is %d",major);	

 /* cdev initialisation */
  kernel_cdev = cdev_alloc();
  kernel_cdev->ops = &memory_fops;
  result=cdev_add(kernel_cdev,first,1);
	if(result<0){
		unregister_chrdev_region(first,1);
		printk(KERN_INFO "unable to add cdev");
		return -1;
	}
  return 0;
}


void memory_exit(void) {
  cdev_del(kernel_cdev);  
  unregister_chrdev_region(first,1);
  printk("<1>Removing memory module\n");

}


static int memory_open(struct inode *inode, struct file *filp) {

  /* Success */
sema_init(&sem,1);
memset(memory_buffer,'\0',64);	
  return 0;
}


static int memory_release(struct inode *inode, struct file *filp) {
 
  /* Success */
  return 0;
}


static ssize_t memory_read(struct file *filp, char *buf,size_t count, loff_t *f_pos) { 
 
  down(&sem);

  /* Transfering data to user space */ 
  copy_to_user(buf,memory_buffer,count);

  up(&sem);
  return 0;
}


static ssize_t memory_write(struct file *filp,const char *buf,size_t count, loff_t *f_pos) {

  down(&sem);

  copy_from_user(memory_buffer,buf,count);
	//printk(" writing into membufer  \n");
  up(&sem);
  return 0;
}
