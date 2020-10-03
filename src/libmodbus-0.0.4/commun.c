#include <modbus.h>
#include <fcntl.h>

/************************************************************************************
		Mb_test_crc : check the crc of a packet
*************************************************************************************
input :
-------
trame  : packet with is crc
n      : lenght of the packet without tht crc
                              ^^^^^^^
answer :
--------
1 = crc fealure
0 = crc ok
************************************************************************************/
int Mb_test_crc(byte trame[],int n)
{
	unsigned int crc,i,j,carry_flag,a;
	crc=0xffff;
	for (i=0;i<n;i++)
	{
		crc=crc^trame[i];
		for (j=0;j<8;j++)
		{
			a=crc;
			carry_flag=a&0x0001;
			crc=crc>>1;
			if (carry_flag==1)
				crc=crc^0xa001;
		}
	}
   if (Mb_verbose)
      printf("test crc %0x %0x\n",(crc&255),(crc>>8));
	if ((trame[n+1]!=(crc>>8)) || (trame[n]!=(crc&255)))
      return 1;
   else
      return 0;
}

/************************************************************************************
		Mb_calcul_crc : compute the crc of a packet and put it at the end
*************************************************************************************
input :
-------
trame  : packet with is crc
n      : lenght of the packet without tht crc
                              ^^^^^^^
answer :
--------
crc
************************************************************************************/
int Mb_calcul_crc(byte trame[],int n)
{
	unsigned int crc,i,j,carry_flag,a;
	crc=0xffff;
	for (i=0;i<n;i++)
	{
		crc=crc^trame[i];
		for (j=0;j<8;j++)
		{
			a=crc;
			carry_flag=a&0x0001;
			crc=crc>>1;
			if (carry_flag==1)
				crc=crc^0xa001;
		}
	}
	trame[n+1]=crc>>8;
	trame[n]=crc&255;
	return crc;
}
/************************************************************************************
		Mb_close_device : Close the device
*************************************************************************************
input :
-------
Mb_device : device descriptor

no output
************************************************************************************/
void Mb_close_device(int Mb_device)
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
int Mb_open_device(char Mbc_port[20], int Mbc_speed, int Mbc_parity, int Mbc_bit_l, int Mbc_bit_s)
{
  int fd;

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

  /* settings port */
  bzero(&Mb_tio,sizeof(Mb_tio));
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
  switch (Mbc_bit_l)
  {
     case 7:
        Mb_tio.c_cflag |= CS7;
        break;
     case 8:
     default:
        Mb_tio.c_cflag |= CS8;
        break;
  }
  switch (Mbc_parity)
  {
     case 1:
        Mb_tio.c_cflag = Mb_tio.c_cflag | PARENB;
//        Mb_tio.c_iflag = ICRNL;
        break;
     case -1:
        Mb_tio.c_cflag = Mb_tio.c_cflag | PARENB | PARODD;
//        Mb_tio.c_iflag = ICRNL;
        break;
     case 0:
     default:
//        Mb_tio.c_iflag = IGNPAR | ICRNL;
        Mb_tio.c_iflag = IGNPAR;
//        Mb_tio.c_iflag &= ~ICRNL;
        break;
  }
  Mb_tio.c_iflag &= ~ICRNL;

  if (Mbc_bit_s==2)
     Mb_tio.c_cflag = Mb_tio.c_cflag | CSTOPB;
     
  Mb_tio.c_cflag = Mb_tio.c_cflag | CLOCAL | CREAD;
  Mb_tio.c_oflag = 0;
  Mb_tio.c_lflag = 0; /*ICANON;*/
  Mb_tio.c_cc[VMIN]=1;
  Mb_tio.c_cc[VTIME]=0;

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

/************************************************************************************
		Mb_rcv_print : print a character
This function can be use with slave or master to print a character when it receive one
*************************************************************************************
input :
-------
c : character

no output
************************************************************************************/
void Mb_rcv_print(unsigned char c)
{
   printf("-> receiving byte :0x%x %d \n",c,c);
}

/************************************************************************************
		Mb_snd_print : print a character
This function can be use with slave or master to print a character when it send one
*************************************************************************************
input :
-------
c : character

no output
************************************************************************************/
void Mb_snd_print(unsigned char c)
{
   printf("<- sending byte :0x%x %d \n",c,c);
}

char *Mb_version(void)
{
   return VERSION;
}
