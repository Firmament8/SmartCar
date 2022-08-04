#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>

using namespace std;

class serialPort {
private:
	int fd = -1;
	struct termios Opt;
	int speed_arr[14] = {B38400, B19200, B9600, B4800, B2400, B1200, B300,
					  B38400, B19200, B9600, B4800, B2400, B1200, B300,};
	int name_arr[14] = {38400,  19200,  9600,  4800,  2400,  1200,  300, 38400,
					  19200,  9600, 4800, 2400, 1200,  300,};

	int ret;
	char value[4];
	fd_set readfd;
	struct timeval timeout;

public:
	serialPort() {
		fd = -1;
	}

	bool OpenPort(const char *dev) {

		char *_dev = new char[32];
		strcpy(_dev, dev);
		fd = open(_dev, O_RDWR);
		if(-1 == fd) {
			perror("Can't Open Serial Port");
			return false;
		}

		int DTR_flag;
		DTR_flag = TIOCM_DTR;
		ioctl(fd, TIOCMBIS, &DTR_flag);
		return true;
	}

	int setup(int speed, int flow_ctrl, int databits, int stopbits, int parity) {

		int   i;
		int   status;
		struct termios options;

		if(tcgetattr(fd, &options) != 0) {
			perror("SetupSerial 1");
			return(false);
		}

		for(i = 0; i < sizeof(speed_arr) / sizeof(int); i++) {
			if(speed == name_arr[i]) {
				cfsetispeed(&options, speed_arr[i]);
				cfsetospeed(&options, speed_arr[i]);
			}
		}

		options.c_cflag |= CLOCAL;
		options.c_cflag |= CREAD;

		switch(flow_ctrl) {

			case 0:
				options.c_cflag &= ~CRTSCTS;
				break;

			case 1:
				options.c_cflag |= CRTSCTS;
				break;
			case 2:
				options.c_cflag |= IXON | IXOFF | IXANY;
				break;
		}

		options.c_cflag &= ~CSIZE;
		switch(databits) {
			case 5:
				options.c_cflag |= CS5;
				break;
			case 6:
				options.c_cflag |= CS6;
				break;
			case 7:
				options.c_cflag |= CS7;
				break;
			case 8:
				options.c_cflag |= CS8;
				break;
			default:
				fprintf(stderr, "Unsupported data size\n");
				return (false);
		}

		switch(parity) {
			case 'n':
			case 'N':
				options.c_cflag &= ~PARENB;
				options.c_iflag &= ~INPCK;
				break;
			case 'o':
			case 'O':
				options.c_cflag |= (PARODD | PARENB);
				options.c_iflag |= INPCK;
				break;
			case 'e':
			case 'E':
				options.c_cflag |= PARENB;
				options.c_cflag &= ~PARODD;
				options.c_iflag |= INPCK;
				break;
			case 's':
			case 'S':
				options.c_cflag &= ~PARENB;
				options.c_cflag &= ~CSTOPB;
				break;
			default:
				fprintf(stderr, "Unsupported parity\n");
				return (false);
		}

		switch(stopbits) {
			case 1:
				options.c_cflag &= ~CSTOPB; break;
			case 2:
				options.c_cflag |= CSTOPB; break;
			default:
				fprintf(stderr, "Unsupported stop bits\n");
				return (false);
		}


		options.c_oflag &= ~OPOST;

		options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
		//options.c_lflag &= ~(ISIG | ICANON);  

		options.c_cc[VTIME] = 1;
		options.c_cc[VMIN] = 1;

		tcflush(fd, TCIFLUSH);

		if(tcsetattr(fd, TCSANOW, &options) != 0) {
			perror("com set error!\n");
			return (false);
		}
		return (true);
	}

	int sreadBuffer(char *buffer, int size) {
		return read(fd, buffer, size);
	}

	void writeBuffer(char *buffer, int size) {
		write(fd, buffer, size);
	}

	void ClosePort() {
		close(fd);
	}

	char serial(char *buffer, int size) {

		timeout.tv_sec = 0;
		timeout.tv_usec = 0;

		FD_ZERO(&readfd);
		FD_SET(fd, &readfd);

		ret = select(fd + 1, &readfd, NULL, NULL, &timeout);
		if(FD_ISSET(fd, &readfd)) {
			read(fd, value, 4);
			if(value[0] == 0xee && value[2] == 0x0a && value[3] == 0x0a) {
				return value[1];
			}
		}
		write(fd, buffer, size);
		return 0x00;
	}
};