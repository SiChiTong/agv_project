#ifndef BLVD20KM_H
#define BLVD20KM_H

#pragma once // tránh tình trạng đụng độ thư viện - trong mọi tình huống chỉ có 1 thư viện được tạo ra ở tất cả các file
#include<iostream>
using namespace std;


class BLVD20KM
{
public:
	/*Phương thức tính toán CRC-16 kiểm tra dữ liệu*/
	unsigned short crc16(char *data_p, unsigned short length);  
	/*Phương thức truy vấn dữ liệu đến thiết bị đệ tử */                  
	uint8_t * Query(uint8_t slaveAddress, uint8_t function_code, uint8_t *data); 
	/*Phương thức nhận phản dữ liệu từ thiết bị đệ tử */ 
	uint8_t * Respon(uint8_t slaveAddress, uint8_t function_code, uint8_t *data); 
	BLVD20KM();
	~BLVD20KM();
	
};

class Unicastmd : public BLVD20KM
{
public:
	unsigned short crc16(char *data_p, unsigned short length);    
	uint8_t * readreg(uint8_t slaveAddress, uint8_t *data);
	uint8_t * writereg(uint8_t slaveAddress, uint8_t *data);
	uint8_t * performdiag(uint8_t slaveAddress, uint8_t *data);
	uint8_t * wirteMultiplereg(uint8_t slaveAddress, uint8_t *data);
	Unicastmd();
	~Unicastmd();
	
};

// class broadcastmd : public BLVD20KM
// {
// public:
// 	unsigned short crc16(char *data_p, unsigned short length);    
// 	uint8_t * Querymessage(uint8_t function_code, uint8_t *data); 
// 	broadcastmd();
// 	~broadcastmd();
	
// };

/* Function code */
#define read 0x03
#define write 0x06
#define perform 0x08
#define writemultiple 0x10

/* Register address */

#endif

