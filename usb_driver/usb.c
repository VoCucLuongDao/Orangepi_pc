#include "usb.h"

static char current_data = 0;

static void usb_mouse_irq(struct urb *urb)
{
	struct usb_mouse *mouse = urb->context;
	signed char *data = mouse->data;
	struct input_dev *dev = mouse->dev;
	int status;
	
	switch (urb->status) {
	case 0:			// success 
		break;
	case -ECONNRESET:	// unlink
	case -ENOENT:
	case -ESHUTDOWN:
		return;
	// -EPIPE:  should clear the halt 
	default:		// error 
		return;
	}
        current_data = data[0];
        input_report_key(dev,BTN_LEFT,data[0]&0x01); //chuột trái
	input_report_key(dev,BTN_RIGHT,data[0]&0x02); //chuột phải
	input_report_key(dev,BTN_MIDDLE,data[0]&0x04); //chuột giữa
	input_report_rel(dev,REL_X,data[1]); //hoành độ
	input_report_rel(dev,REL_Y,data[2]); //tung độ
	input_report_rel(dev,REL_WHEEL,data[3]); //tọa độ núm xoay
	input_sync(dev); //đồng bộ dữ liệu với input device
        if(data[0] & 0x01){
		pr_info("Left mouse button clicked!\n");	
	}
	else if(data[0] & 0x02){
		pr_info("Right mouse button clicked!\n");
	}
        else if(data[0] & 0x04){
		pr_info("Middle mouse button clicked!\n");
	}
        pr_info("Toa do chuot (%d, %d)!\n",data[1],data[2]);
        pr_info("Toa do num xoay (%d)!\n",data[3]);
        printk(KERN_INFO "The call back function is called: %x - %x - %x - %x \n", data[0], data[1], data[2], data[3]);	
        
        status = usb_submit_urb (urb, GFP_ATOMIC);
	if (status)
		dev_err(&mouse->usbdev->dev,
			"can't resubmit intr, %s-%s/input0, status %d\n",
			mouse->usbdev->bus->bus_name,
			mouse->usbdev->devpath,
                        status);
}
//
static int usb_mouse_open(struct input_dev *dev)
{
	struct usb_mouse *mouse;
	mouse = (struct usb_mouse *) input_get_drvdata(dev);

	if (usb_submit_urb(mouse->irq, GFP_KERNEL))
		return -EIO;
	
	printk(KERN_INFO "The usb_mouse_open function is called \n");	
	return 0;
}

static void usb_mouse_close(struct input_dev *dev)
{
	struct usb_mouse *mouse;
	mouse = (struct usb_mouse *) input_get_drvdata(dev);
	usb_kill_urb(mouse->irq);
	printk(KERN_INFO "The usb_mouse_close function is called \n");	
}

//device file ops
int my_open(struct inode *i, struct file *f)
{
	printk(KERN_INFO "Driver: open()\n");
  	return 0;
}

int my_close(struct inode *i, struct file *f)
{
	printk(KERN_INFO "Driver: close()\n");
	return 0;
}

ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "Driver: read()\n");
	copy_to_user(buf, &current_data, 1);	//Copy current click data to buffer
	current_data = 0;			//Clear current data
	return 0;
}

ssize_t my_write(struct file *f, const char __user *buf,size_t len, loff_t *off)
{
	printk(KERN_INFO "Driver: write()\n");
	return len;
}

struct usb_class_driver 		class; 
 	
int usb_hid_probe (struct usb_interface *interface, const struct usb_device_id *id)
{
	struct usb_host_interface*		host_interface;
  	struct usb_host_endpoint*		endpoint;
  	u8  					bNumEndpoints;
  	u8 					bEndpointAddress;
  	u16					wMaxPacketSize;
  
  	struct usb_device*			usb_device;
	
  	int pipe, maxp;
  	
	class.name = "usb/pen%d";
        class.fops = &pugs_fops;
       if (usb_register_dev(interface, &class) < 0){
		printk(KERN_INFO "Not able to get a minor for this device.");
		return -ENODEV;
	} 
	printk(KERN_INFO "Minor obtained: %d\n", interface->minor);
	
	host_interface = interface->cur_altsetting;
	bNumEndpoints = host_interface->desc.bNumEndpoints;
	
	endpoint = host_interface->endpoint;
	if(!endpoint){
		printk(KERN_ERR "Not exist endpoint");
		return -1;
	}
	
	bEndpointAddress = endpoint->desc.bEndpointAddress;
	wMaxPacketSize   = endpoint->desc.wMaxPacketSize;
	
	//
	// unsigned int pipe: The endpoint information of the specific target usb_device to be sent by the urb. 
	// The value depends on the direction of data transmission:
 	//	usb_rcvintpipe() sets the endpoint as a control input endpoint.
 	//	usb_sndctrlpipe() sets the endpoint number as a control output endpoint
	//
	
	usb_device = interface_to_usbdev(interface);
	if(!usb_device){
		printk(KERN_ERR "The usb device isn't exist");
		return -1;
	}
	pipe = usb_rcvintpipe(usb_device, bEndpointAddress);
	maxp = usb_maxpacket(usb_device, pipe, usb_pipeout(pipe));
	/*********************************************************************/
	struct usb_mouse *mouse;
	struct input_dev *input_dev;
	
	mouse = kzalloc(sizeof(struct usb_mouse), GFP_KERNEL);
	if(!mouse){
		printk(KERN_ERR "The mouse is not allocated ");
		return -1;
	}
	
	input_dev = input_allocate_device();
	if(!input_dev){
		printk(KERN_ERR "The input_dev is not allocated ");
		return -1;
	}
	
	if (usb_device->manufacturer)
		strlcpy(mouse->name, usb_device->manufacturer, sizeof(mouse->name));

	if (usb_device->product) {
		if (usb_device->manufacturer)
			strlcat(mouse->name, " ", sizeof(mouse->name));
		strlcat(mouse->name, usb_device->product, sizeof(mouse->name));
	}

	if (!strlen(mouse->name))
		snprintf(mouse->name, sizeof(mouse->name),
			 "USB HIDBP Mouse %04x:%04x",
			 le16_to_cpu(usb_device->descriptor.idVendor),
			 le16_to_cpu(usb_device->descriptor.idProduct));

	usb_make_path(usb_device, mouse->phys, sizeof(mouse->phys));
	strlcat(mouse->phys, "/input0", sizeof(mouse->phys));

	mouse->usbdev = usb_device;
	mouse->dev    = input_dev;
	mouse->data   = usb_alloc_coherent(usb_device, 8, GFP_ATOMIC, &mouse->data_dma);
	if(!mouse->data){
		printk(KERN_ERR "The mouse->data is not allocated ");
		return -1;
	}
	mouse->irq    = usb_alloc_urb(0, GFP_KERNEL);
	if(!mouse->irq){
		printk(KERN_ERR "The mouse->irq is not allocated ");
		return -1;
	}
	
	usb_fill_int_urb(mouse->irq, usb_device, pipe, mouse->data,
			 (maxp > 8 ? 8 : maxp),
			 usb_mouse_irq, mouse, endpoint->desc.bInterval);
	mouse->irq->transfer_dma = mouse->data_dma;
	mouse->irq->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
	mouse->irq->dev = mouse->usbdev;
	
	/*****************************************************************/
	input_dev->name = mouse->name;
	input_dev->phys = mouse->phys;
	usb_to_input_id(usb_device, &input_dev->id);
	input_dev->dev.parent = &interface->dev;
	input_dev->open = usb_mouse_open;
	input_dev->close = usb_mouse_close;
	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
	input_dev->keybit[BIT_WORD(BTN_MOUSE)] = BIT_MASK(BTN_LEFT) | BIT_MASK(BTN_RIGHT)
						  | BIT_MASK(BTN_MIDDLE) | BIT_MASK(BTN_SIDE) | BIT_MASK(BTN_EXTRA);
	input_dev->relbit[0] = BIT_MASK(REL_X) | BIT_MASK(REL_Y) | BIT_MASK(REL_WHEEL);
		
	///Store the drive data ddata in the form of private data in the input device input for future use.
	input_set_drvdata(input_dev, mouse);
	usb_set_intfdata(interface, mouse);	
	input_register_device(mouse->dev);
	 
	printk(KERN_INFO "The probe function is called !!! \n");
	return 0;
}

void usb_hid_disconnect (struct usb_interface *interface)
{
	usb_deregister_dev(interface, &class);
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
