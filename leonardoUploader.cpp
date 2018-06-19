#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <iostream>
#include <sys/ioctl.h>

using namespace std;

int fd;
int retSystem;
struct termios termAttr;

int main(int argc, char *argv[])
{
	if(argv[1] == NULL || !strcmp(argv[1], "--help")) {
		cout << "Usage: " << argv[0] << " serialport [hexfile]" << endl;
		exit(1);
	}
	
	//open serial
	fd = open(argv[1], O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1)
	{
		perror("open_port: Unable to open serial port\n");
		exit(1);
	}
	cout << "Open Serial...." << endl;

	//set baud to 1200 to reset Arduino Leonardo
	tcgetattr(fd,&termAttr);
	cfsetispeed(&termAttr,B1200);
	cfsetospeed(&termAttr,B1200);
	tcsetattr(fd,TCSANOW,&termAttr);

	int RTS_flag, DTR_flag;
	RTS_flag = TIOCM_RTS;
	DTR_flag = TIOCM_DTR;
	ioctl(fd,TIOCMBIS,&RTS_flag);//Set RTS pin
	ioctl(fd,TIOCMBIC,&DTR_flag);//Clear DTR pin

	close(fd);
	cout << "Reset Arduino Leonardo...." << endl;

	if(argv[2] == NULL) {
		cout << "No hex file given." << endl;
		exit(0);
	}

	//delay 2 seconds after reseting
	sleep(2);

	char cmd[512];
	snprintf(cmd, sizeof(cmd), "avrdude -pm32u4 -cavr109 -D -P %s -Uflash:w:%s", argv[1], argv[2]);
	
	cout << "avrdude cmd: " << cmd << endl;
	retSystem = system(cmd);

	exit(retSystem);
	return 0;
}
