# Mouse-Device-Driver
A Linux device driver to adjust the brightness of the screen based on mouse events. A left click decreases brightness and a right click increases brightness.

## Introduction:
Devices are generally represented by their driver files in the */dev* directory. Here, we've created a custom driver for our mouse. 

Device files are created using the ```mknod``` system call.

```
mknod path type major minor
```

- *Path* - Path where the file is to be created.
- *Type* - 'c' or 'b' to indicate whether the device is a character device or a block device.
- *Major, Minor* - The major and minor number of the device. Major number identifies the device driver and minor number is used to identify the specific instance of the device. ```ls – l``` is used to find these numbers.

These files are plain files which are associated with our module with their major number. They can be deleted like regular files with

```
rm path
```


Device drivers can be built either as part of the kernel or separately as loadable modules. ```lsmod``` or ```cat /proc/modules``` prints the contents of the ```/proc/modules``` file, which shows the loadable kernel modules are currently loaded.

Modules can be loaded using the ```insmod``` command, by giving the name of the object file (*.ko*) to be loaded.

```
insmod module_name
```
Modules can be unloaded using the ```rmmod``` command.

```
rmmod module_name
```


## Working:
The driver read mouse input using kernel level functions from /dev/input/mice which will not work with PS controllers and other kinds of input devices. The brightness is changed by writing to /sys/class/backlight/amdgpu_bl1/brightness.

## Compile the driver by running the following commands:
```
1) sudo su
2) make mknd 
3) make
4) insmod BM_Driver.ko
5) make test
```

## For recompiling the driver, follow these steps to remove previous installation
```
1) sudo su
2) make clean
3) rmmod BM_Driver
4) rm /dev/BMDriver
```
After this, compile the driver as normal

## Debug
Debug messages are added to the kernel ring buffer and can be read using the dmesg command. To limit the number of messages to a reasonable amount, say 50, use dmesg | tail -50
