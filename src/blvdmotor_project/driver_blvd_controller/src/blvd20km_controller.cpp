#include <ros/ros.h>
#include "mbrtu/modbusrtu.h"
#include <boost/thread/thread.hpp>

#define MOTOR_ADDRESS1 0X01
#define MOTOR_ADDRESS2 0X02
#define POLY                0xA001



int main(int argc, char **argv)
{
	/* onpen comport */
	int fd = Mb_open_device("/dev/ttyUSB0",115200,1,8,1); /*even , 8 bit, 1 stop_bit*/
	/*create ros node*/
	ros::init(argc, argv, "Driver_motor");
	ros::NodeHandle nh;

	while(ros::ok())
	{	
		writeRegister(fd,MOTOR_ADDRESS1,ADDR_SPEED0_L, 500);
		writeRegister(fd,MOTOR_ADDRESS1,ADDR_MOTOR_CONTROL, createMotorControl16bit(MOTOR_DIRECTOIN_FORWARD,true,true,0));
 		ros::spinOnce();
	}

	Mb_close_device(fd);
	return 0;
}