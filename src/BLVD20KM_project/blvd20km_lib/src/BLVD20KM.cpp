#include "blvd20km_lib/BLVD20KM.h"
#define POLY 0xA001

uint16_t BLVD20KM_hieplm::getcrc16(uint8_t const *data_p, uint16_t length)
/*
EX: Ban đầu CRC = 1111 1111 1111 1111 chuyển sang Hex là FFFF
	Chọn data_p là 54 hay 0101 0100(1 byte) là số cần tính. 
	Chọn số poly =  A001h hay 1010 000 000 0001 
	(Poly là một số mà bạn sử dụng làm phép tính số CRC cơ sở của mình.)

	+ Bước 1: Dịch CRC và data_p sang phải 1 bit 
	  data_p = 54, là 0101 0100 trở thành 0010 1010 
	  CRC = 1111 1111 1111 1111 trở thành 0111 1111 1111 1111
	
	+ Bước 2: Kiểm tra BIT ngoài cùng bên phải của Dữ liệu và so sánh nó với một trong các CRC
      NẾU chúng bằng nhau, dịch chuyển CRC sang phải 1 bit 
      NẾU chúng không phải, dịch chuyển CRC sang phải 1 bit VÀ cộng thêm số Poly một lần nữa.
	  Thực hiện bước 2 đúng 8 lần vì 1 byte có 8 bit.

	+Bước 3: Bước 1 và 2 sẽ được lăp lại theo số lượng data_p.
*/
{
	unsigned char i;
	unsigned int data;
	unsigned int crc = 0xffff;

	if (length == 0)
	    return (~crc);
	do
	{
	    for (i=0, data=(unsigned int)0xff & *data_p++; 
	    	 i < 8; 
	    	 i++, data >>= 1)
	    {
	        if ((crc & 0x0001) ^ (data & 0x0001))
	            crc = (crc >> 1) ^ POLY;
	        else  crc >>= 1;
	    }
	} while (--length);

	return (crc);
}   

FILE *serialInit(char * port, int baud)
{
	int BAUD = 0;
	int fd = -1;
	struct termios newtio;
	FILE *fp = NULL;
	 
	//Open the serial port as a file descriptor for low level configuration
	// read/write, not controlling terminal for process,
	fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY );
	if ( fd<0 )
	{

	    ROS_ERROR("serialInit: Could not open serial device %s",port);
	    return fp;
	    
	}
	 
	// set up new settings
	memset(&newtio, 0,sizeof(newtio));
	newtio.c_cflag =  CS8 | CLOCAL | CREAD;  //no parity, 1 stop bit
	 
	newtio.c_iflag = IGNCR;    //ignore CR, other options off
	newtio.c_iflag |= IGNBRK;  //ignore break condition
	 
	newtio.c_oflag = 0;        //all options off
	 
	newtio.c_lflag = ICANON;     //process input as lines
	 
	  // activate new settings
	tcflush(fd, TCIFLUSH);
	  //Look up appropriate baud rate constant
	switch (baud)
	{
	    case 38400:
	    default:
	        BAUD = B38400;
	        break;
	    case 19200:
	        BAUD  = B19200;
	        break;
	    case 9600:
	        BAUD  = B9600;
	        break;
	    case 4800:
	        BAUD  = B4800;
	        break;
	    case 2400:
	        BAUD  = B2400;
	        break;
	    case 1800:
	        BAUD  = B1800;
	        break;
	    case 1200:
	        BAUD  = B1200;
	        break;
	}  //end of switch baud_rate
	
	if (cfsetispeed(&newtio, BAUD) < 0 || cfsetospeed(&newtio, BAUD) < 0)
	{
	    ROS_ERROR("serialInit: Failed to set serial baud rate: %d", baud);
	    close(fd);
	    return NULL;
	}
	tcsetattr(fd, TCSANOW, &newtio);
	tcflush(fd, TCIOFLUSH);
	 
	//Open file as a standard I/O stream
	fp = fdopen(fd, "r+");
	if (!fp) {
	    ROS_ERROR("serialInit: Failed to open serial stream %s", port);
	    fp = NULL;
	}
	return fp;
}
BLVD20KM_hieplm::BLVD20KM_hieplm(char * port, int baud)
{
	fpSerial = serialInit(port, baud);
}


