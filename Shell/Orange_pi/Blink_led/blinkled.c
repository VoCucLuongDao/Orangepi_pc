#include "blinkled.h"

#define _IDEBUG_

static uint8_t led_status = 0x00;
static uint32_t long_time = 1000;
int open_blinkled_driver (struct inode *inode, struct file *flip)
{
	led_obj.counter_or++;
    #ifdef _IDEBUG_
        printk(KERN_INFO "open file /dev/blinkled_device: %d times\n",led_obj.counter_or);
    #endif
    return 0;
}

int release_blinkled_driver (struct inode *inode, struct file *flip)
{
	led_obj.counter_or--;
    #ifdef _IDEBUG_
        printk(KERN_INFO "release file /dev/blinkled_device \n");
    #endif
    return 0;
}

ssize_t read_blinkled_driver (struct file *flip, char __user *user_buf, size_t len, loff_t *off)
{
    int ret = 0;
    int read_length = len;
    if(user_buf == NULL || len == 0)
    	return -1;
    if(read_length > BUFFER_SIZE - *off){
    	read_length = BUFFER_SIZE - *off;
    }
    
    ret = copy_to_user(user_buf, led_obj.status, read_length);
    
    *off += read_length;
    
    #ifdef _ORANGE_PI_
	Ledport_ChangeStatus(0, 0);
    #endif
     
    #ifdef _IDEBUG_
        printk(KERN_INFO "read file \n");
    #endif
    
    long_time = 1000;
    
    mod_timer(&led_obj.dev_timer, jiffies + msecs_to_jiffies(long_time));
            
    return read_length;
}

ssize_t write_blinkled_driver (struct file *flip, const char __user *user_buf, size_t len, loff_t *off)
{
    int ret = 0;
    int write_length = len;
    if(user_buf == NULL || len == 0)
    	return -1;
    
    if(write_length > BUFFER_SIZE - *off){
    	write_length = BUFFER_SIZE - *off;
    }
    
    ret = copy_from_user(led_obj.status, user_buf, write_length);
    
    *off += write_length;
    
    #ifdef _ORANGE_PI_
	Ledport_ChangeStatus(0, 1);
    #endif
     
    #ifdef _IDEBUG_
        printk(KERN_INFO "write file \n");
    #endif
    
    long_time = (uint16_t) 100000;
    
    return write_length;
}

int allocate_memory(void)
{
	led_obj.dev_regs = kzalloc(sizeof(blinkled_regs), GFP_KERNEL);
	if(!led_obj.dev_regs){
		return -1;
	}
	
	led_obj.dev_regs->ctrl_reg = kzalloc(BUFFER_SIZE* sizeof(char), GFP_KERNEL);
	if(!led_obj.dev_regs->ctrl_reg){
		return -2;
	}
	
	led_obj.status = kzalloc(BUFFER_SIZE*sizeof(char), GFP_KERNEL);
	if(!led_obj.status ){
		return -3;
	}
	
	return 0;
}

void destroy_memory(void)
{
	kfree(led_obj.dev_regs->ctrl_reg);
	kfree(led_obj.dev_regs);
}

int Ledport_init(void)
{
	 void __iomem *base = (void __iomem *) (GPIO_BASE + 0*24);
	 
	 uint32_t port = __raw_readl(base);
        
	 port &=  ~PORTA_MASK;
	 port |=  0x01;
	    
	 __raw_writel(port, base);
	 
	return 0;
}

void Ledport_ChangeStatus(int led_num, int status)
{
    void __iomem *base = (void __iomem *)GPIOA_DAT;
   
    uint32_t data;
    
    data = __raw_readl(base);
    
    switch (status) {
        
        case LED_ON:
            data |= 0x01;
            break;
            
        case LED_OFF:
              data &= ~0x01;
            break;
    }
    
    __raw_writel(data, base);
}

void timer_handler(struct timer_list* data)
{
     led_status = (led_status ^ 0x01) & 0x01;
     
     #ifdef _ORANGE_PI_
	Ledport_ChangeStatus(0, led_status);
     #endif
     
    #ifdef _IDEBUG_
        printk(KERN_INFO "Status of Led: %d\n", led_status);
    #endif
    
	mod_timer(&led_obj.dev_timer, jiffies + msecs_to_jiffies(long_time));
}

int initial_blink_led_driver(void)
{
	int ret = 0;
	led_obj.dev_num = 0;
	ret = alloc_chrdev_region(&led_obj.dev_num , 0 , 1, "blink_led_driver" );
	if(ret < 0){
		return ret;
	}
    #ifdef _IDEBUG_
        printk(KERN_INFO "The evice is allocated (MAJOR - MINOR) = (%d - %d)\n", MAJOR(led_obj.dev_num), MINOR(led_obj.dev_num ));
    #endif
	
	led_obj.dev_cdev = cdev_alloc();
	if(!led_obj.dev_cdev){
		goto fail_cdev_alloc;	
	}
	
	cdev_init(led_obj.dev_cdev, &fops);
	
	ret = cdev_add(led_obj.dev_cdev, led_obj.dev_num , 1);
	if(ret < 0){
		goto fail_cdev_add;
	}
	
	led_obj.dev_class = class_create(THIS_MODULE, "class_blinkled_driver");
	if(!led_obj.dev_class){
		goto fail_class_create;
	}
	
	led_obj.dev_device = device_create(led_obj.dev_class, NULL, led_obj.dev_num , NULL, "blink_device_123" );
	if(!led_obj.dev_device){
		goto fail_device_create;
	}
	
	ret = allocate_memory();
	if(ret < 0){
		goto fail_allocate_memory;
	}
	
    #ifdef _ORANGE_PI_
	Ledport_init();
	Ledport_ChangeStatus(0,LED_ON);
    #endif  
        
	timer_setup(&led_obj.dev_timer, timer_handler, 0);
	
        mod_timer(&led_obj.dev_timer, jiffies + msecs_to_jiffies(long_time));
    
    #ifdef _IDEBUG_
        printk(KERN_INFO "Device driver is installed!!! \n");
    #endif

    return 0;
    
    /**************** FAIL *******************************/
    fail_allocate_memory:
    	device_destroy(led_obj.dev_class, led_obj.dev_num);
    fail_device_create:
    	class_destroy(led_obj.dev_class);
    fail_class_create:
    
    fail_cdev_add:
    	cdev_del(led_obj.dev_cdev);
    fail_cdev_alloc:
    	unregister_chrdev_region(led_obj.dev_num, 1);
    	return -1;
}

void  removeblink_led_driver(void)
{	
	del_timer(&led_obj.dev_timer);
	destroy_memory();
    	device_destroy(led_obj.dev_class, led_obj.dev_num);
	class_destroy(led_obj.dev_class);
	cdev_del(led_obj.dev_cdev);
	unregister_chrdev_region(led_obj.dev_num, 1);
	
        printk(KERN_INFO "Device driver is removed !!! \n");
}

