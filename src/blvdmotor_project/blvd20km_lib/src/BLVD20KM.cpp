#include "blvd20km_lib/BLVD20KM.h"

#define POLY                0xA001
#define FN_CODE_READ        0x03
#define FN_CODE_WRITE       0x06
#define FN_CODE_DIAGNOSIS   0x08
#define FN_CODE_WRITE_MULTI 0x10

#define ADDR_ALARM_H         0x0080
#define ADDR_ALARM_L         0x0081
#define ADDR_RESET_ALARM_H   0x0180
#define ADDR_RESET_ALARM_L   0x0181
#define ADDR_SPEED0_H        0x0480
#define ADDR_SPEED0_L        0x0481
#define ADDR_TORQUE_H        0x0700
#define ADDR_TORQUE_L        0x0701
#define ADDR_TORQUE_LIMIT0_H 0x0700
#define ADDR_TORQUE_LIMIT0_L 0x0701
#define ADDR_MOTOR_CONTROL   0x007d
#define ADDR_ANALOG_MODE_L   0x10e3
#define ADDR_CONFIG_H        0x018c
#define ADDR_CONFIG_L        0x018d
#define MOTOR_DIRECTOIN_STOP    0
#define MOTOR_DIRECTOIN_FORWARD 1
#define MOTOR_DIRECTOIN_REVERSE 2

#define MOTOR_FORWARD_BIT      0b00100000
#define MOTOR_REVERSE_BIT      0b00000000
#define MOTOR_SLOW_CHANGE_BIT  0b00100000
#define MOTOR_FREE_ON_STOP_BIT 0b10000000

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
uint16_t BLVD20KM_hieplm::getCRC16(uint8_t const *data_p, uint16_t length, uint16_t poly)
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

/*###############################################*/
void BLVD20KM_hieplm::serialInit(int serial_port, struct termios Mb_tio, uint8_t address)
{


	if(serial_port >0)
	{
		if (tcsetattr(serial_port,TCSANOW,&Mb_tio) <0)
		{
			perror("Can't set terminal parameters ");
		}else
			ROS_INFO("Device 0x%x is connected ",address);
	}else {
		ROS_INFO("Device 0x%x can not connect to port(%d)",address,serial_port);
		perror("Open device failure\n") ;
		exit(-1) ;
	}
		
}

/*###############################################*/
BLVD20KM_hieplm::BLVD20KM_hieplm(int serial_port, struct termios Mb_tio ,uint8_t address)
{
	this->serial_port = serial_port;
	this->address = address;
	this->Mb_tio = Mb_tio;
	ROS_INFO("Device 0x%x is connecting port(%d)", address, serial_port);
	serialInit(serial_port,Mb_tio,address);
}


/*###############################################*/
uint8_t BLVD20KM_hieplm::writeSpeedControlMode(uint16_t mode) 
{
	uint8_t result;
	result = writeRegister(ADDR_ANALOG_MODE_L, mode);
	if (result != 0) 
	{
		return result;
	}
	return writeConfigTrigger(); // trigger after setting ADDR_ANALOG_MODE
}

/*###############################################*/
uint8_t BLVD20KM_hieplm::readSpeedControlMode(uint16_t *mode) 
{
	return readRegisters(ADDR_ANALOG_MODE_L, 1, mode);
}

/*###############################################*/
uint8_t BLVD20KM_hieplm::writeConfigTrigger() 
{
	uint8_t result = writeStop();
	if (result != 0) return result;
	return writeRegister(ADDR_CONFIG_L, 1);
}

/*###############################################*/
uint16_t createMotorControl16bit(uint8_t motorDirection, bool freeLockOnStop = true, bool slowChange = true, uint8_t motorDataNum = 0) {
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
uint8_t BLVD20KM_hieplm::writeForward() 
{
	return writeRegister(ADDR_MOTOR_CONTROL, createMotorControl16bit(MOTOR_DIRECTOIN_FORWARD));
}

/*###############################################*/
uint8_t BLVD20KM_hieplm::writeLock() 
{
	return writeRegister(ADDR_MOTOR_CONTROL, createMotorControl16bit(MOTOR_DIRECTOIN_STOP, false));
}

/*###############################################*/
uint8_t BLVD20KM_hieplm::writeStop() 
{
	return writeRegister(ADDR_MOTOR_CONTROL, createMotorControl16bit(MOTOR_DIRECTOIN_STOP));
}

/*###############################################*/
uint8_t BLVD20KM_hieplm::writeReverse() 
{
	return writeRegister(ADDR_MOTOR_CONTROL, createMotorControl16bit(MOTOR_DIRECTOIN_REVERSE));
}

/*###############################################*/
uint8_t BLVD20KM_hieplm::writeSpeed(uint16_t speed) 
{
	return writeRegister(ADDR_SPEED0_L, speed);
}

/*###############################################*/
uint8_t BLVD20KM_hieplm::writeTorqueLimit(uint16_t torque) 
{
	return writeRegister(ADDR_TORQUE_L, torque);
}

/*###############################################*/
uint8_t BLVD20KM_hieplm::writeDiagnosis() 
{
	uint8_t result;
	uint8_t data[41];
	data[0] = 0;
	data[1] = 0;
	data[2] = 1;
	data[3] = 2;
	writeQuery(FN_CODE_DIAGNOSIS, data, sizeof(data));
	for (uint8_t i = 0; i < 41; ++i) 
	{
		data[i] = 0;
	}
	result = readQuery(FN_CODE_DIAGNOSIS, data, sizeof(data));
	if (result != 0) { return result; }
	if (data[2] != 1 || data[3] != 2) 
	{
		return BLVD20KM_ERROR_DIAGNOSIS_DATA_INVALID;
	}

	return 0;
}

/*###############################################*/
uint8_t BLVD20KM_hieplm::writeResetAlarm() 
{
	return writeRegister(ADDR_RESET_ALARM_L, 1);
}

/*###############################################*/
uint8_t BLVD20KM_hieplm::readAlarm(uint16_t *alarm) 
{
	return readRegisters(ADDR_ALARM_L, 1, alarm);
}

/*###############################################*/
uint8_t BLVD20KM_hieplm::readDirection(bool *forwarding, bool *reversing, bool *freeLockOnStop) 
{
	uint16_t data;
	uint8_t result;
	result = readRegisters(ADDR_MOTOR_CONTROL, 1, &data);
	if (result != 0) 
	{
		return result;
	}
	*forwarding = (MOTOR_FORWARD_BIT & data) != 0x00;
	*reversing = (MOTOR_REVERSE_BIT & data) != 0x00;
	*freeLockOnStop = (MOTOR_FREE_ON_STOP_BIT & data) != 0x00;
	return 0;
}

/*###############################################*/
uint16_t uint8tsToUint16t(uint8_t *chars) 
{
	return ((uint16_t) chars[0]) << 8 | (uint16_t) chars[1];
}

/*###############################################*/
uint32_t uint16tsToUint32t(uint16_t *shorts) 
{
	return ((uint32_t) shorts[0]) << 16 | (uint32_t) shorts[1];
}

/*###############################################*/
uint8_t BLVD20KM_hieplm::readUint32t(uint16_t readStartAddress, uint32_t *value) 
{
	uint8_t result;
	result = readRegisters(readStartAddress, 2, uint16Buffer);
	if (result != 0) 
	{
		return result;
	}
	*value = uint16tsToUint32t(uint16Buffer);
	return 0;
}

/*###############################################*/
uint8_t BLVD20KM_hieplm::readTorque(uint16_t *torque) 
{
	return readRegisters(ADDR_TORQUE_L, 1, torque);
}

/*###############################################*/
uint8_t BLVD20KM_hieplm::readTorqueLimit(uint16_t *torque) 
{
	return readRegisters(ADDR_TORQUE_LIMIT0_L, 1, torque);
}

/*###############################################*/
uint8_t BLVD20KM_hieplm::readSpeed(uint16_t *speed) 
{
	return readRegisters(ADDR_SPEED0_L, 1, speed);
}

/*###############################################*/
uint8_t BLVD20KM_hieplm::writeRegister(uint16_t writeAddress, uint16_t data16bit) 
{
	uint8_t data[] = {
						(uint8_t)(writeAddress >> 8),
						(uint8_t)(writeAddress & 0xFF),
						(uint8_t)(data16bit >> 8),
						(uint8_t)(data16bit & 0xFF)
					 };
	
	writeQuery(FN_CODE_WRITE, data, sizeof(data));
	return readQuery(FN_CODE_WRITE, data, sizeof(data));
}

/*###############################################*/
uint8_t BLVD20KM_hieplm::readRegisters(uint16_t readStartAddress, uint16_t dataLen, uint16_t* registerData) 
{
	uint8_t result;
	uint8_t data[] = {
						(uint8_t)(readStartAddress >> 8),
						(uint8_t)(readStartAddress & 0xFF),
						(uint8_t)(dataLen >> 8),
						(uint8_t)(dataLen & 0xFF)
					 };
	
	writeQuery(FN_CODE_READ, data, sizeof(data));
	result = readQuery(FN_CODE_READ, uint8Buffer, dataLen * 2 + 1);
	if (result != 0) { return result; }
	for (uint16_t i = 0; i < dataLen; ++i) 
		registerData[i] = uint8tsToUint16t(&uint8Buffer[i * 2 + 1]); // + 1 to skip data length byte
	return 0;
}

/*###############################################*/
void BLVD20KM_hieplm::cleanbuff()
{
	uint8_t read_buf [256]; 
	memset(&read_buf, '\0', sizeof(read_buf));
	read(serial_port, &read_buf, sizeof(read_buf));
}
/*###############################################*/
void BLVD20KM_hieplm::writeQuery(uint8_t fnCode, uint8_t* data, uint16_t dataLen)
{
	uint16_t queryLen = 4 + dataLen;
	uint16_t i;
	queryBuffer[0] = address;
	queryBuffer[1] = fnCode;
	for (i = 0; i < dataLen; ++i)  queryBuffer[i+2] = data[i];
	uint16_t crc16 = getCRC16(queryBuffer, queryLen - 2, POLY);
	queryBuffer[queryLen - 2] = crc16 >>8;
	queryBuffer[queryLen - 1] = crc16 & 0xFF;
	// remove received buffer before sending
	cleanbuff();
	uint8_t msg[queryLen];
	for (i = 0; i < queryLen; ++i) 
		msg[i] = queryBuffer[i];

  	write(serial_port,msg,(size_t)queryLen);
	usleep(C3_5_time); //delay ms
}

/*###############################################*/
uint8_t BLVD20KM_hieplm::readQuery(uint8_t fnCode, uint8_t* data, uint16_t dataLen)
{
	uint16_t queryLen = 0;
	std::clock_t    start;
	unsigned long waitFrom = std::clock();
	const unsigned long timeoutMs = 10;

	uint8_t read_buf [256]; 
	memset(&read_buf, '\0', sizeof(read_buf));
	
    while((std::clock() - waitFrom)/(double)(CLOCKS_PER_SEC / 1000) < timeoutMs)
    {
    	//queryLen = read(serial_port, &read_buf, sizeof(read_buf));
    	if(queryLen)
    	{
    		cout << "done" << endl;
    		break;  		
    	} 
  	}
  	
	if (queryLen == 0) 
		return BLVD20KM_ERROR_NO_RESPONSE;

	uint16_t crc = getCRC16(queryBuffer, queryLen - 2, POLY);
	if ((uint8_t)(crc & 0xFF) != queryBuffer[queryLen - 1] || (uint8_t)(crc >> 8) != queryBuffer[queryLen - 2]) 
		return BLVD20KM_ERROR_UNMATCH_CRC;
	
	if (queryBuffer[0] != address) 
		return BLVD20KM_ERROR_UNMATCH_ADDRESS;

	if (queryBuffer[1] != fnCode) 
	{
		if (queryBuffer[1] == (fnCode + 0x80)) 
			return queryBuffer[2]; // ERROR_CODE
		else 
			return BLVD20KM_ERROR_UNMATCH_FN_CODE;
	}

	if (dataLen != queryLen - 4) 
		return BLVD20KM_ERROR_UNMATCH_DATA_LEN;
	
	for (uint16_t i = 0; i < dataLen; ++i) 
		data[i] = queryBuffer[i + 2];

	return 0;
}
