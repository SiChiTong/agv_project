#ifndef BLVD20KM_H
#define BLVD20KM_H

#pragma once // tránh tình trạng đụng độ thư viện - trong mọi tình huống chỉ có 1 thư viện được tạo ra ở tất cả các file

// C library headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdlib.h>
#include <ctime>
#include "std_msgs/String.h"
#include "ros/ros.h"
using namespace std;

#define DEFAULT_BAUDRATE 115200
#define DEFAULT_SERIALPORT "/dev/ttyUSB0"

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

		BLVD20KM_hieplm(uint8_t address, char * port, int baud);
		bool begin(void);
		void closemotor(void);
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
		struct termios tty;
	    void serialInit(char * port, int baud);
		uint16_t getCRC16(uint8_t const *data_p, uint16_t length);
		uint8_t address;
	    uint8_t queryBuffer[BLVD20KM_QUERY_MAX_LEN];
	    uint8_t readUint32t(uint16_t readStartAddress, uint32_t *value);
	    uint8_t readQuery(uint8_t fnCode, uint8_t* data, uint16_t dataLen);
	    uint8_t readRegisters(uint16_t readStartAddress, uint16_t dataLen, uint16_t* registerData);
	    uint8_t writeConfigTrigger();
	    uint8_t writeRegister(uint16_t writeAddress, uint16_t data16bit);
	    void writeQuery(uint8_t fnCode, uint8_t* data, uint16_t dataLen);

	    uint16_t uint16Buffer[8];
	    uint8_t uint8Buffer[41];
	    
};

#endif

