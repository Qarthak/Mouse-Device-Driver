#include <linux/module.h>
#include <linux/string.h>
#include <linux/fs.h>
// #include <sys/types.h>

// module attributes
MODULE_LICENSE("GPL"); // this avoids kernel taint warning
MODULE_DESCRIPTION("Device Driver Demo");
MODULE_AUTHOR("Sarthak C");

static char msg[100]={0};
static short readPos=0;
static int times= 0;
static int majorNumber = 91;
// protoypes, else the structure initialization that follows fail
static int dev_open(struct inode *inod, struct file *fil)
{
	times++;
	printk(KERN_ALERT "Device opened %d times\n", times);
	return 0;
}

// called when 'close' system call is done on the device file
static int dev_rls(struct inode *inod, struct file *fil)
{
	printk(KERN_ALERT "Device closed\n");
	return 0;
}

static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

// structure containing callbacks
	static struct file_operations fops ={
		.read = dev_read, // address of dev_read
		.open = dev_open, // address of dev_open
		.write = dev_write, // address of dev_write
		.release = dev_rls // address of dev_ris
	};

// called when module is loaded, similar to main()
int init_module(void)
{
	int t = register_chrdev(majorNumber,"myfirst",&fops); //register driver with major:89
	
	if (t<0) printk(KERN_ALERT "Device registration failed..\n");
	else printk(KERN_ALERT "Device registered...\n");
	return t;
}

// called when module is unloaded, similar to destructor in OOP
void cleanup_module (void){
	unregister_chrdev(majorNumber,"myfirst");
}

// called when 'read' system call is done on the device file
static ssize_t dev_read(struct file *filp, char *buff, size_t len, loff_t *off)
{
	short count = 0;
	while (len && (msg[readPos] !=0))
	{
		copy_to_user(buff, msg+readPos,1); //copy byte from kernel space to user space
		buff++;
		count++;
		len--;
		readPos++;
	}
	return count;
}

// called when 'write' system call is done on the device file
static ssize_t dev_write(struct file *filp, const char *buff, size_t len, loff_t *off)
{
	short ind = len-1;
	short count=0;
	memset (msg,0,100);
	readPos=0;
	while(len>0)
	{
		copy_from_user(msg+count, buff+ind, 1); //copy the given string to the driver but in reverse
		ind--;
		count++;
		len--;
	}
	return count;
}
