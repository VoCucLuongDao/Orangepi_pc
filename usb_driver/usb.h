#ifndef _USB_H_
#define _USB_H_

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/usb/input.h>
#include <linux/usb.h>

#define USB_VENDOR_ID   0x1c4f
#define USB_PRODUCT_ID  0x0034 

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("vocucluongdao");
MODULE_DESCRIPTION (" USB HID Protocol driver");
MODULE_VERSION ("V1.0");

struct usb_mouse {
	char name[128];
	char phys[64];
	struct usb_device *usbdev;
	struct input_dev *dev;
	struct urb *irq;

	signed char *data;
	dma_addr_t data_dma;
};


int usb_hid_probe (struct usb_interface *interface, const struct usb_device_id *id);
void usb_hid_disconnect (struct usb_interface *interface);

static struct usb_device_id usb_hid_table[] = {
	{USB_DEVICE(USB_VENDOR_ID, USB_PRODUCT_ID)},
	{},
};

MODULE_DEVICE_TABLE (usb, usb_hid_table);

//device file ops
static int my_open(struct inode *i, struct file *f);
static int my_close(struct inode *i, struct file *f);
static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off);
static ssize_t my_write(struct file *f, const char __user *buf,size_t len, loff_t *off);

static struct file_operations pugs_fops = {
  .open = my_open,
  .release = my_close,
  .read = my_read,
  .write = my_write
};

static struct usb_driver usb_hid_driver = {
	.name 		= "usb_mouse",
	.probe 	= usb_hid_probe,
	.disconnect 	= usb_hid_disconnect,
	.id_table 	= usb_hid_table,
};

static int __init usb_hid_init(void);
static void __exit usb_hid_exit(void);

module_init(usb_hid_init);
module_exit(usb_hid_exit);
	
#endif
