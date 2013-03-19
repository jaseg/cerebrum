#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/usbdevice_fs.h>
#include <unistd.h>

//Resets an USB device (akin to disconnecting and re-connecting it)
//See http://www.roman10.net/how-to-reset-usb-device-in-linux/
//Compile with gcc -Wall -o usbreset usbreset.c

int main(int argc, char **argv){
	int fd = open(argv[1], O_WRONLY);
	if(fd == -1){
		fprintf(stderr, "Cannot open the given USB device: %s\n", strerror(errno));
		return 1;
	}
	if(ioctl(fd, USBDEVFS_RESET, 0)){
		fprintf(stderr, "Error while invoking the ioctl on the given device: %s\n", strerror(errno));
		close(fd);
		return 2;
	}else{
		close(fd);
		return 0;
	}
}
