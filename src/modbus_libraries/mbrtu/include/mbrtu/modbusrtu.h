//
// Created by Hiep  on 10/2/2020.
//

#ifndef MODBUSPPMODBUSRTU_H
#define MODBUSPPMODBUSRTU_H
#pragma once 

#include <iostream>  /* std::cout */
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <time.h>	 /* delay */
#include <cstdint>	 /* uin8_t */

///Function Code
#define     READ_COILS        0x01
#define     READ_INPUT_BITS   0x02
#define     READ_REGS         0x03
#define     READ_INPUT_REGS   0x04
#define     WRITE_COIL        0x05
#define     WRITE_REG         0x06
#define     WRITE_COILS       0x0F
#define     WRITE_REGS        0x10

/* master structure */
typedef struct {
   int device;												/* modbus device (serial port: /dev/ttyS0 ...) */
   int slave; 												/* number of the slave to call*/
   int function; 											/* modbus function to emit*/
   int address;												/* slave address */
   int length;												/* data length */
   int timeout;												/* timeout in ms */
} Mbm_trame;

class modbusrtu
{

	private:
		struct termios saved_tty_parameters;			/* old serial port setting (restored on close) */
		struct termios Mb_tio;						/* new serail port setting */
	public:
		int Mb_open_device(const char *Mbc_port, int Mbc_speed,/* open device and configure it */	
							int Mbc_parity, int Mbc_bit_l,
							 int Mbc_bit_s);	
		void Mb_close_device(int Mb_device);				/* close device*/	
		modbusrtu();
		~modbusrtu();
		
};


#endif //MODBUSPP_MODBUS_H