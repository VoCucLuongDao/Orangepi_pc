#include "usb.h"

int usb_hid_probe (struct usb_interface *intf, const struct usb_device_id *id)
{
	
	printk(KERN_INFO "The probe function is called !!! \n");
	return 0;
}

void usb_hid_disconnect (struct usb_interface *intf)
{
	printk(KERN_INFO "The disconnect function is called !!! \n");
}

int usb_hid_init(void)
{
	int ret = 0;
	ret = usb_register_driver(&usb_hid_driver, THIS_MODULE, NULL);
	printk(KERN_INFO "USB_HID_DRIVER is successful !!! \n");
	return ret;
}

void usb_hid_exit(void)
{
	usb_deregister(&usb_hid_driver);
	printk(KERN_INFO "USB_HID_DRIVER is removed !!! \n");	
}