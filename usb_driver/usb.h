#ifndef _USB_H_
#define _USB_H_

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/usb/input.h>
#include <linux/usb.h>

#define USB_VENDOR_ID   0x1c4f
#define USB_PRODUCT_ID  0x0034 

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("vocucluongdao");
MODULE_DESCRIPTION (" USB HID Protocol driver");
MODULE_VERSION ("V1.0");

int usb_hid_probe (struct usb_interface *intf, const struct usb_device_id *id);
void usb_hid_disconnect (struct usb_interface *intf);

static struct usb_device_id usb_hid_table[] = {
	{USB_DEVICE(USB_VENDOR_ID, USB_PRODUCT_ID)},
	{},
};

MODULE_DEVICE_TABLE (usb, usb_hid_table);

static struct usb_driver usb_hid_driver = {
	.name 		= "usb_mouse",
	.probe 		= usb_hid_probe,
	.disconnect 	= usb_hid_disconnect,
	.id_table 	= usb_hid_table,
};

static int __init usb_hid_init(void);
static void __exit usb_hid_exit(void);

module_init(usb_hid_init);
module_exit(usb_hid_exit);
	
#endif