#include "mbrtu/modbusrtu.h"

#define POLY   0xA001
int Mb_device;				/* device tu use */

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
uint16_t getCRC16(uint8_t data_p[], uint16_t length, uint16_t poly)
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
	            crc = (crc >> 1) ^ poly;
	        else  crc >>= 1;
	    }
	} while (--length);

	return (crc);
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
int Mb_open_device(const char Mbc_port[], int Mbc_speed, int Mbc_parity, int Mbc_bit_l, int Mbc_bit_s)
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


/*###############################################*/
uint16_t createMotorControl16bit(uint8_t motorDirection, bool freeLockOnStop = true, bool slowChange = true, uint8_t motorDataNum = 0) 
{
	// MB-FREE, -, STOP-MODE, REV, FWD, M1, M2, M0
	uint16_t bits = 0x0000;
	switch (motorDirection) 
	{
	case MOTOR_DIRECTOIN_REVERSE:
		bits |= MOTOR_REVERSE_BIT;
		break;
	case MOTOR_DIRECTOIN_FORWARD:
		bits |= MOTOR_FORWARD_BIT;
		break;
	}
	if (freeLockOnStop) 
	{
		bits |= MOTOR_FREE_ON_STOP_BIT;
	}
	if (slowChange) 
	{
		bits |= MOTOR_SLOW_CHANGE_BIT;
	}
	if (motorDataNum != 0 && motorDataNum < 0b1000) 
	{
		bits |= motorDataNum;
	}
	return bits;
}

/*###############################################*/
uint16_t uint8tsToUint16t(uint8_t chars[]) 
{
	return ((uint16_t) chars[0]) << 8 | (uint16_t) chars[1];
}

/*########################################################################*/
uint8_t writeRegister(int fd, uint8_t address, uint16_t writeAddress, uint16_t data16bit) 
{
	uint8_t data[] = {
						(uint8_t)(writeAddress >> 8),
						(uint8_t)(writeAddress & 0xFF),
						(uint8_t)(data16bit >> 8),
						(uint8_t)(data16bit & 0xFF)
					 };

	writeQuery(fd, address, FN_CODE_WRITE, data, sizeof(data));
	return readQuery(fd, address, FN_CODE_WRITE, data, sizeof(data));
}

/*#########################################################################*/
void writeQuery(int fd, uint8_t address,uint8_t fnCode, uint8_t data[], uint16_t dataLen)
{

	uint16_t queryLen = 4 + dataLen;
	uint16_t i;
	uint8_t queryBuffer[BLVD20KM_QUERY_MAX_LEN];
	queryBuffer[0] = address;
	queryBuffer[1] = fnCode;
	for (i = 0; i < dataLen; ++i)  queryBuffer[i+2] = data[i];
	uint16_t crc16 = getCRC16(queryBuffer, queryLen - 2, POLY);
	queryBuffer[queryLen - 1] = crc16 >>8;
	queryBuffer[queryLen - 2] = crc16 & 0xFF;
	// remove received buffer before sending

	uint8_t msg[queryLen];
	for (i = 0; i < queryLen; ++i) 
		msg[i] = queryBuffer[i];
	/* clean port */
	tcflush(fd, TCIFLUSH);
  	ssize_t st = write(fd,msg,(size_t)queryLen);
	usleep(C3_5_time); //delay ms
}

/*##############################################################3##################*/
uint8_t readRegisters(int fd, uint8_t address, uint16_t readStartAddress, uint16_t dataLen, uint16_t registerData[]) 
{
	uint8_t result;
	uint8_t data[] = {
						(uint8_t)(readStartAddress >> 8),
						(uint8_t)(readStartAddress & 0xFF),
						(uint8_t)(dataLen >> 8),
						(uint8_t)(dataLen & 0xFF)
					 };
	
	writeQuery(fd, address, FN_CODE_READ, data, sizeof(data));
	result = readQuery(fd, address, FN_CODE_READ, uint8Buffer, dataLen * 2 + 1);
	if (result != 0) { return result; }
	for (uint16_t i = 0; i < dataLen; ++i) 
		registerData[i] = uint8tsToUint16t(&uint8Buffer[i * 2 + 1]); // + 1 to skip data length byte
	return 0;
}

/*####################################################################################*/
uint8_t readQuery(int fd, uint8_t address, uint8_t fnCode, uint8_t data[], uint16_t dataLen)
{
	uint16_t queryLen = 0;
	clock_t  start = clock();
	const unsigned long timeoutMs = 10;

	uint8_t read_buf [BLVD20KM_QUERY_MAX_LEN];
	memset(&read_buf, '\0', sizeof(read_buf));
	
    while(true)
    {
    	queryLen = read(fd, &read_buf, sizeof(read_buf));
    	if(queryLen||(clock() - start)/(double)(CLOCKS_PER_SEC / 1000) < timeoutMs)
    		break;  		
  	}
  
	if (queryLen == 0) 
		return BLVD20KM_ERROR_NO_RESPONSE;

	uint16_t crc = getCRC16(read_buf, queryLen - 2, POLY);
	if ((uint8_t)(crc & 0xFF) != read_buf[queryLen - 1] || (uint8_t)(crc >> 8) != read_buf[queryLen - 2]) 
		return BLVD20KM_ERROR_UNMATCH_CRC;
	
	if (read_buf[0] != address) 
		return BLVD20KM_ERROR_UNMATCH_ADDRESS;

	if (read_buf[1] != fnCode) 
	{
		if (read_buf[1] == (fnCode + 0x80)) 
			return read_buf[2]; // ERROR_CODE
		else 
			return BLVD20KM_ERROR_UNMATCH_FN_CODE;
	}

	if (dataLen != queryLen - 4) 
		return BLVD20KM_ERROR_UNMATCH_DATA_LEN;
	
	for (uint16_t i = 0; i < dataLen; ++i) 
		data[i] = read_buf[i + 2];

	return 0;
}