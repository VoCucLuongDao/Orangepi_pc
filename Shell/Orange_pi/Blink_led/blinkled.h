#ifndef _BLINK_LED_H_
#define _BLINK_LED_H_

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/device.h>
#include "asm/uaccess.h"
#include "asm/io.h"
#include <linux/timer.h>
#include <linux/jiffies.h>

MODULE_LICENSE     	("GPL");
MODULE_AUTHOR      	("VOCUCLUONGDAO");
MODULE_DESCRIPTION 	("A SIMPLE BLINK LED");
MODULE_VERSION     	("V_1.0");

#define LED_ON    	1
#define LED_OFF   	0
#define GPIO_BASE	0x01C20800

#define GPIOA_DAT      (GPIO_BASE + 0x10)
#define PORTA_MASK     0x07
#define BUFFER_SIZE    256

typedef struct _blinkled_register{
	char* ctrl_reg;
} blinkled_regs;

struct _blinkled_driver {
	dev_t 		  dev_num;
	struct cdev*	  dev_cdev;
	struct class*     dev_class;
	struct device*    dev_device;
	blinkled_regs*    dev_regs;
	struct timer_list dev_timer;
	uint32_t 	   counter_or;
	uint16_t 	   dev_pin_num;
	char* 		   status;
} led_obj;

int open_blinkled_driver (struct inode *, struct file *);
int release_blinkled_driver (struct inode *, struct file *);
ssize_t read_blinkled_driver (struct file *, char __user *, size_t, loff_t *);
ssize_t write_blinkled_driver (struct file *, const char __user *, size_t, loff_t *);
	
struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = open_blinkled_driver,
	.release = release_blinkled_driver,
	.read = read_blinkled_driver,
	.write = write_blinkled_driver,
} ;

int allocate_memory(void);
void destroy_memory(void);

int Ledport_init(void);
void Ledport_ChangeStatus(int led_num, int status);

static int __init initial_blink_led_driver(void);
static void __exit removeblink_led_driver(void);

module_init(initial_blink_led_driver);
module_exit(removeblink_led_driver);

#endif
