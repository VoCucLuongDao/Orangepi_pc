#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/delay.h>

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("VoCucLuongDao");
MODULE_DESCRIPTION ("Sameple gpio device driver");

#define GPIO_PIN_OUT (6)
#define GPIO_PIN_IN  (7)

static int __init gpio_driver_init(void);
static void __exit gpio_driver_exit(void);

irqreturn_t demo_irq_handler(int irq, void* dev_id)
{
	printk(KERN_INFO "This is the interrupt \n");
	
	return IRQ_HANDLED;
}

int gpio_driver_init(void)
{
	int ret = 0;
	ret = gpio_is_valid(GPIO_PIN_OUT);
	if(!ret){
		printk(KERN_INFO "ERROR: GPIO %d is not valid \n", GPIO_PIN_OUT);
		return -1;
	}
	
	ret = gpio_request(GPIO_PIN_OUT, "echo ");
	if(!ret){
		printk(KERN_INFO "ERROR: GPIO %d request \n", GPIO_PIN_OUT);
		return -1;
	}

	gpio_direction_output(GPIO_PIN_OUT, 0);

	gpio_export(GPIO_PIN_OUT, false);

	int i = 0;
	for(i = 0; i < 10; i++){
		gpio_set_value(GPIO_PIN_OUT, 1);
		msleep(1000);
		gpio_set_value(GPIO_PIN_OUT, 0);
		msleep(1000);
	}

	ret = gpio_request_one(GPIO_PIN_IN, GPIOF_IN, "button");
	gpio_export(GPIO_PIN_IN, false);
	int no_irq = gpio_to_irq(GPIO_PIN_IN);
	ret = irq_request()
	printk(KERN_INFO "The device driver is installed !! \n");

	return 0;
}

void gpio_driver_exit(void)
{
	gpio_unexport(GPIO_PIN_OUT);
	gpio_free(GPIO_PIN_OUT);

	printk(KERN_INFO "The device driver is removed !! \n");
}

module_init(gpio_driver_init);
module_exit(gpio_driver_exit);