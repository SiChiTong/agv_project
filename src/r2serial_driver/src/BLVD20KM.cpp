#include "r2serial_driver/BLVD20KM.h"
#define POLY 0xA001

unsigned short BLVD20KM::crc16(char *data_p, unsigned short length)
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

	crc = ~crc;
	data = crc;
	crc = (crc << 8) | (data >> 8 & 0xff);

	return (crc);
}   

uint8_t * BLVD20KM::Query(uint8_t slaveAddress, uint8_t function_code, uint8_t *data); 
{
	uint8_t * p_data;
	*p_data = *data;
	*(p_data + 1) = (data + 1);
	for(uint8_t = 2, i < (unsigned)strlen(data_p) + 2; i++)
	{
		*(p_data + i) = *(data + i);
	}
}

BLVD20KM::BLVD20KM();
BLVD20KM::~BLVD20KM();