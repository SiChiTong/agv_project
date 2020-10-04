#include "mbrtu/modbusrtu.h"

/************************************************************************************
		Mb_close_device : Close the device
*************************************************************************************
input :
-------
Mb_device : device descriptor

no output
************************************************************************************/
void modbusrtu::Mb_close_device(int Mb_device)
{
  if (tcsetattr (Mb_device,TCSANOW,&saved_tty_parameters) < 0)
    perror("Can't restore terminal parameters ");
  close(Mb_device);
}

/************************************************************************************
		Mb_open_device : open the device
*************************************************************************************
input :
-------
Mbc_port   : string with the device to open (/dev/ttyS0, /dev/ttyS1,...)
Mbc_speed  : speed (baudrate)
Mbc_parity : 0=don't use parity, 1=use parity EVEN, -1 use parity ODD
Mbc_bit_l  : number of data bits : 7 or 8 	USE EVERY TIME 8 DATA BITS
Mbc_bit_s  : number of stop bits : 1 or 2    ^^^^^^^^^^^^^^^^^^^^^^^^^^

answer  :
---------
device descriptor
************************************************************************************/
int modbusrtu::Mb_open_device(const char *Mbc_port, int Mbc_speed, int Mbc_parity, int Mbc_bit_l, int Mbc_bit_s)
{
	int fd;	/* File descriptor for the port */
	/* open port */
	fd = open(Mbc_port,O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY) ;
	if(fd<0)
	{
		perror("Open device failure\n") ;
		exit(-1) ;
	} else   fcntl(fd, F_SETFL, 0);

	/* save olds settings port */
	if (tcgetattr (fd,&saved_tty_parameters) < 0)
	{
		perror("Can't get terminal parameters ");
		return -1 ;
	}
	
	/*
    * Enable the receiver and set local mode...
    */
    bzero(&Mb_tio,sizeof(Mb_tio));

    /*
   	* Set size data
   	*/
    switch (Mbc_bit_l)
	{
		case 7:
			Mb_tio.c_cflag |= CS7;
			break;
		case 8:
			Mb_tio.c_cflag |= CS8;
			break;
		default:
			Mb_tio.c_cflag |= CS8;
	}
	/*
    * Set the parity mode */
	switch (Mbc_parity)
	{
	    case 1:
	        Mb_tio.c_cflag = Mb_tio.c_cflag | PARENB;
	        break;
	    case -1:
	        Mb_tio.c_cflag = Mb_tio.c_cflag | PARENB | PARODD;;
	        break;
	    case 0:
	     	Mb_tio.c_iflag = IGNPAR;
	        break;
	    default:
	        Mb_tio.c_iflag = IGNPAR;
	}

	if (Mbc_bit_s==2)
    	Mb_tio.c_cflag = Mb_tio.c_cflag | CSTOPB;

	Mb_tio.c_iflag &= ~ICRNL;
	Mb_tio.c_cflag = Mb_tio.c_cflag | CLOCAL | CREAD;
	Mb_tio.c_oflag = 0;
	Mb_tio.c_lflag = 0; 	/*ICANON;*/
	Mb_tio.c_cc[VMIN]=1;
	Mb_tio.c_cc[VTIME]=0;
    /*
    * Set the baud rates 
    */
    speed_t BAUD;  
    switch (Mbc_speed)
	{
		case 0:
			BAUD = B0;
			break;
		case 50:
			BAUD = B50;
			break;
		case 75:
			BAUD = B75;
			break;
		case 110:
			BAUD = B110;
			break;
		case 134:
			BAUD = B134;
			break;
		case 150:
			BAUD = B150;
			break;
		case 200:
		BAUD = B200;
			break;
			case 300:
		BAUD = B300;
			break;
			case 600:
		BAUD = B600;
			break;
			case 1200:
		BAUD = B1200;
			break;
			case 1800:
		BAUD = B1800;
			break;
			case 2400:
		BAUD = B2400;
			break;
			case 4800:
		BAUD = B4800;
			break;
			case 9600:
		BAUD = B9600;
			break;
			case 19200:
		BAUD = B19200;
			break;
			case 38400:
		BAUD = B38400;
			break;
			case 57600:
		BAUD = B57600;
			break;
			case 115200:
		BAUD = B115200;
			break;
		case 230400:
			BAUD = B230400;
			break;
		default:
			BAUD = B9600;
	}
	cfsetispeed(&Mb_tio, BAUD);
	cfsetospeed(&Mb_tio, BAUD);
	/* clean port */
  	tcflush(fd, TCIFLUSH);

  	  /* activate the settings port */
	if (tcsetattr(fd,TCSANOW,&Mb_tio) <0)
	{
		perror("Can't set terminal parameters ");
		return -1 ;
	}

	return fd ;
}
modbusrtu::modbusrtu(){}
modbusrtu::~modbusrtu(){}