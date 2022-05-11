/*
	Linux Device driver module to change the brightness of the screen based on mouse click
*/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/fcntl.h>
#include <linux/unistd.h>
#include <linux/buffer_head.h>
#include <linux/input.h>
#include <asm/segment.h>
#include <asm/uaccess.h>

// module attributes
MODULE_LICENSE("GPL"); // avoid kernel taint warning
MODULE_DESCRIPTION("Mouse Driver");
MODULE_AUTHOR("Sarthak");

const int MAJOR_NUMBER = 45;
const bool TESTING_MODE = true;
static int times = 0;
char brightness_buff[10];
char mice_buff[10];
int k, btn_left, btn_right, btn_middle;
struct file *filehandle1, *filehandle2;

// protoypes, else the structure initialization that follows fail
static int dev_open(struct inode *, struct file *);
static int dev_rls(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

// structure containing callbacks
static struct file_operations fops =
{
	.read = dev_read,
	.open = dev_open,
	.write = dev_write,
	.release = dev_rls,
};

/* 
  * file_open, file_close, file_read and file_write are error-checking wrappers around filp_open, flip_close, vfs_read and vfs_write. 
  * The possible error code and their meanings can be found in errno-base.h 
  * (https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/include/uapi/asm-generic/errno-base.h)
 */

//file_open is a wrapper around filp_open
struct file* file_open(const char* path, int flags, int rights)
{
	struct file* filp = filp_open(path, flags, rights);
	if (IS_ERR(filp))
	{
		printk(KERN_ALERT "Error opening Files. Error number: %ld", PTR_ERR(filp));
		// printk(KERN_ALERT "Error opening File %s. Error number: %ld", path, PTR_ERR(filp));
		return NULL;
	}
	return filp;
}

//file_close is a wrapper around filp_close
int file_close(struct file* file)
{
	int ret = filp_close(file, NULL);
	printk(KERN_ALERT "filp_close gives %d",ret );
	return ret;
}

//file_read is a wrapper around vfs_read
int file_read(struct file* file, long long offset, unsigned char* data, unsigned int size)
{
	int ret = kernel_read(file, data, size, &offset);
	printk(KERN_ALERT "kernel_read gives %d",ret );
	return ret;
}

//file_write is a wrapper around vfs_write
int file_write(struct file* file, long long offset, unsigned char* data, unsigned int size)
{
	int ret = kernel_write(file, data, size, &offset);
	printk(KERN_ALERT "kernel_write gives %d",ret );
	return ret;
}

// called when module is loaded, similar to main()
int init_module(void)
{
	// register driver with major number MAJOR_NUMBER and name mousedev
	// returns a non-negative number on success
	int t = register_chrdev(MAJOR_NUMBER, "mousedev", &fops);
	if (t < 0) printk(KERN_ALERT "Device registration failed ...\n");
	else printk(KERN_ALERT "Device registered ...\n");
	return t;
}

// called when module is unloaded, similar to destructor in OOP
void cleanup_module(void)
{
	unregister_chrdev(MAJOR_NUMBER, "mousedev");
}

// called when 'open' system call is done on the device file
static int dev_open(struct inode *inod, struct file *fil)
{
	times++;
	printk(KERN_ALERT "Device opened %d times\n", times);
	return 0;
}

// called when 'read' system call is done on the device file
static ssize_t dev_read(struct file *filp, char *buff, size_t len, loff_t *off)
{
	int brightness =0;
	
	filehandle1 = file_open("/sys/class/backlight/amdgpu_bl1/brightness", O_RDONLY, 0);
	// printk(KERN_ALERT "Opened the brightness file");
	
	file_read((struct file*)filehandle1, 0, brightness_buff, 2);
	printk(KERN_ALERT "Current brightness is %s",brightness_buff);
	
	//TODO: This gives EINVAL error
	// filehandle2 = file_open("/dev/input/mice", O_RDONLY, 0);
	filehandle2 = open("/dev/input/mice", O_RDONLY);
	// printk(KERN_ALERT "Mouse file");
	
	file_read((struct file*)filehandle2, 0, mice_buff, 2);
	printk(KERN_ALERT "Mouse1 is %s\n",mice_buff);
	// printk(KERN_ALERT "Mice");
	// printk(KERN_ALERT "Location of mice buffer is %p",mice_buff);

	btn_left = mice_buff[0] & 0x1;
	btn_right = mice_buff[0] & 0x2;
	btn_middle = mice_buff[0] & 0x4;
	// printk(KERN_ALERT "Did partial math");

	brightness = (brightness_buff[0]-'0') * 10;
	brightness += (brightness_buff[1]-'0');
	

	// printk(KERN_ALERT "Did math");

	if (btn_left > 0)
	{
		if (brightness < 95)
			brightness += 5;
		printk(KERN_ALERT "Left click and Brightness= %s\n", brightness_buff);
	}
	else if (btn_right > 0)
	{
		if (brightness > 5)
			brightness -= 5;
		printk(KERN_ALERT "Right click and Brightness= %s\n", brightness_buff);
	}
	else
	{
		if(TESTING_MODE){
			if(brightness<90){
				brightness+=1;
			}
			else if(brightness==90){
				brightness-=1;
			}
		}
		//do nothing
		printk(KERN_ALERT "Right/Left click not detected and Brightness=%s\n", brightness_buff);
	}
	
	// printk(KERN_ALERT "Come outta math");

	brightness_buff[0] = brightness/10 + '0';
	brightness_buff[1] = brightness%10 + '0';
	// brightness_buff[0] = '1';
	// brightness_buff[1] = '1';
	
	// printk(KERN_ALERT "More math");

	file_close((struct file*)filehandle1);
	
	// printk(KERN_ALERT "Close file");

	filehandle1 = file_open("/sys/class/backlight/amdgpu_bl1/brightness", O_WRONLY, 0);

	// printk(KERN_ALERT "Open to write");

	printk(KERN_ALERT "Brightness written is %s\n", brightness_buff);
	file_write((struct file*)filehandle1, 0, brightness_buff, 2);
	
	// printk(KERN_ALERT "Wrote");
	
	file_close((struct file*)filehandle1);
	
	// printk(KERN_ALERT "Closed after write");

	
	return 0;
}

// called when 'write' system call is done on the device file
static ssize_t dev_write(struct file *filp, const char *buff, size_t len, loff_t *off)
{
	return 0;
}

// called when 'close' system call is done on the device file
static int dev_rls(struct inode *inod, struct file *fil)
{
	printk(KERN_ALERT "Device closed.\n");
	return 0;
}
