#ifndef BLVD20KM_H
#define BLVD20KM_H

#pragma once // tránh tình trạng đụng độ thư viện - trong mọi tình huống chỉ có 1 thư viện được tạo ra ở tất cả các file

// C library headers
#include <iostream>  /* std::cout */
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <time.h>	 /* delay */
#include <cstdint>	 /* uin8_t */


#include "std_msgs/String.h"
#include "ros/ros.h"
using namespace std;

#define DEFAULT_BAUDRATE 115200
#define DEFAULT_SERIALPORT "/dev/ttyUSB0"
#define C3_5_time 2000

#define BLVD20KM_QUERY_MAX_LEN 41
#define BLVD20KM_ERROR_CODE_INVALID_FN        0x01
#define BLVD20KM_ERROR_CODE_INVALID_ADDR      0x02
#define BLVD20KM_ERROR_CODE_INVALID_DATA      0x03
#define BLVD20KM_ERROR_CODE_SLAVE_ERROR       0x04
#define BLVD20KM_ERROR_NO_RESPONSE            0x10
#define BLVD20KM_ERROR_UNMATCH_CRC            0x11
#define BLVD20KM_ERROR_UNMATCH_ADDRESS        0x12
#define BLVD20KM_ERROR_UNMATCH_FN_CODE        0x13
#define BLVD20KM_ERROR_UNMATCH_DATA_LEN       0x14
#define BLVD20KM_ERROR_OVER_QUERY_MAX_LEN     0x15
#define BLVD20KM_ERROR_DIAGNOSIS_DATA_INVALID 0x16

#define BLVD20KM_SPEED_MIN 80
#define BLVD20KM_SPEED_MAX 4000
#define BLVD20KM_TORQUE_MAX 200

#define BLVD02KM_SPEED_MODE_USE_DIGITALS 0x0001

class BLVD20KM_hieplm
{
	public:

		uint8_t address;
		BLVD20KM_hieplm(int serial_port, termios Mb_tio ,uint8_t address);
		uint8_t writeForward();
		uint8_t writeLock();
		uint8_t writeStop();
		uint8_t writeReverse();

		uint8_t readDirection(bool *forwarding, bool *reversing, bool *freeLockOnStop);
		uint8_t readSpeed(uint16_t *speed);
		uint8_t readSpeedControlMode(uint16_t *mode);
		uint8_t readTorque(uint16_t *torque);
		uint8_t readTorqueLimit(uint16_t *torque);
		uint8_t writeSpeed(uint16_t speed);
		uint8_t writeSpeedControlMode(uint16_t mode);
		uint8_t writeTorqueLimit(uint16_t torque);
		uint8_t writeDiagnosis();
		uint8_t readAlarm(uint16_t *alarm);
		uint8_t writeResetAlarm();

	private:
		/*test*/
		int serial_port;
		struct termios Mb_tio;
	    uint8_t queryBuffer[BLVD20KM_QUERY_MAX_LEN];
	    void serialInit(int serial_port, termios Mb_tio,uint8_t address);
	    /* check the crc of a message packet*/									
		uint16_t getCRC16(uint8_t const *data_p, uint16_t length, uint16_t POLY);
	    uint8_t readUint32t(uint16_t readStartAddress, uint32_t *value);
	    uint8_t readQuery(uint8_t fnCode, uint8_t* data, uint16_t dataLen);
	    uint8_t readRegisters(uint16_t readStartAddress, uint16_t dataLen, uint16_t* registerData);
	    uint8_t writeConfigTrigger();
	    uint8_t writeRegister(uint16_t writeAddress, uint16_t data16bit);
	    void writeQuery(uint8_t fnCode, uint8_t* data, uint16_t dataLen);
	    void cleanbuff();

	    uint16_t uint16Buffer[8];
	    uint8_t uint8Buffer[41];
	    
};

#endif

