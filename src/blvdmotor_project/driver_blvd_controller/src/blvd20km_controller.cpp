#include <ros/ros.h>
#include "mbrtu/modbusrtu.h"
#include <geometry_msgs/Twist.h>
#include <r2serial_driver/speed_wheel.h>
#include "std_msgs/String.h"


#define DEFAULT_BAUDRATE 115200
#define DEFAULT_SERIALPORT "/dev/ttyUSB0"
//Function Code
#define     READ_COILS        0x01
#define     READ_INPUT_BITS   0x02
#define     READ_REGS         0x03
#define     READ_INPUT_REGS   0x04
#define     WRITE_COIL        0x05
#define     WRITE_REG         0x06
#define     WRITE_COILS       0x0F
#define     WRITE_REGS        0x10
// #include <boost/thread/thread.hpp>

#define MOTOR_ADDRESS1 0X01
#define MOTOR_ADDRESS2 0X02
#define POLY           0xA001

#define Pi 			   3.1415926535
#define rad_rpm        9.5492965964254
#define L              0.255 // wheelbase (in meters per radian)
#define R  			   0.075 //wheel radius (in meters per radian)

static int speed[2];
//Process ROS receive from navigation message, send to uController
void navigationCallback(const r2serial_driver::speed_wheel& robot)
{
	speed[0] = robot.wheel_letf;
    speed[1] = robot.wheel_right;
    ROS_INFO("Wheel left: %d  Wheel right: %d", speed[0], speed[1]);
}

int main(int argc, char **argv)
{
	char port[30];    //port name
  	int baud;     	  //baud rate 

	strcpy(port, DEFAULT_SERIALPORT);
	if (argc > 1)
	 strcpy(port, argv[1]);

	baud = DEFAULT_BAUDRATE;
	if (argc > 2) {
		if(sscanf(argv[2],"%d", &baud)!=1) {
		  ROS_ERROR("ucontroller baud rate parameter invalid");
		  return 1;
		}
	}
	ROS_INFO("connection initializing (%s) at %d baud", port, baud);
	/* onpen comport */
	Mb_open_device(port,baud,1,8,1); /*even , 8 bit, 1 stop_bit*/
	/*create ros node*/
	ros::init(argc, argv, "Driver_motor");
	ros::NodeHandle nh;
	/* Subscriber */
    ros::Subscriber navigation;
	navigation =  nh.subscribe("Navigation_control_cmd", 10,navigationCallback); 

	for (uint8_t i = 1; i < 3; i++)
	{	
		writeSpeedControlMode(i,BLVD02KM_SPEED_MODE_USE_DIGITALS);
		writeSpeed(i,BLVD20KM_SPEED_MIN);
		writeResetAlarm(i);
		writeStop(i);
	}
	uint8_t tick;
	while(ros::ok())
	{
		//uint16_t encoder[2];
		for(uint8_t i = 1; i < 3; i++)
		{
			if(speed[i-1] > 0){
				writeForward(i);
			}else if(speed[i-1] < 0){
				writeReverse(i); 
			}else{
				writeStop(i);
			}

			writeSpeed(i,abs(speed[i-1]));
		}
		//ROS_INFO("%d , %d",(int)encoder[0],(int)encoder[1]);	
 		ros::spinOnce();
	}

	Mb_close_device();
	return 0;
}

