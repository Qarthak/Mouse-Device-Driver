# Mouse-Device-Driver
A Linux device driver to adjust the brightness of the screen based on mouse events

## Working:
The driver read mouse input using kernel level functions from /dev/input/mice which will not work with PS controllers and other kinds of input devices. The brightness is changed by writing to /sys/class/backlight/amdgpu_bl1/brightness. These two files must exist for the driver to work

## Compile the driver by running the following commands:
1) sudo su
No need to do this if the device file already exists
2) make mknd 
3) make
4) insmod BM_Driver.ko
5) make test

## For recompiling the driver, follow these steps to remove previous installation
1) sudo su
2) make clean
3) rmmod BM_Driver
Do this step only if there's some error with the device file, like an incorrect Major number
4) rm /dev/BMDriver
After this, compile the driver as normal
