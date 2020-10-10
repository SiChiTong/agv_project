#include <ros/ros.h>
#include "mbrtu/modbusrtu.h"
#include <geometry_msgs/Twist.h>
#include <driver_blvd_controller/speed_wheel.h>
// #include <boost/thread/thread.hpp>

#define DEFAULT_BAUDRATE 115200
#define DEFAULT_SERIALPORT "/dev/ttyUSB0"

#define MOTOR_ADDRESS1 0X01
#define MOTOR_ADDRESS2 0X02
#define POLY           0xA001

#define Pi 			   3.1415926535
#define rad_rpm        9.5492965964254
#define L              0.5 // wheelbase (in meters per radian)
#define R  			   0.085 //wheel radius (in meters per radian)

static int speed[2];
static uint16_t alarm_status[2], encoder_value[2];
//Process ROS receive from navigation message, send to uController
void navigationCallback(const driver_blvd_controller::speed_wheel& robot)
{
	speed[0] = robot.wheel_letf;
    speed[1] = robot.wheel_right;
    for(int i= 0; i<4; i++) ROS_INFO("  ");
    ROS_INFO("Wheel left: %d  Wheel right: %d", speed[0], speed[1]);
	ROS_INFO("encoder_value[0] = %d encoder_value[1] = %d",(int16_t)encoder_value[0],(int16_t)encoder_value[1]);
	ROS_INFO("alarm_status[0] = %x alarm_status[1] = %x",alarm_status[0],alarm_status[1]);
	for(int i= 0; i<40; i++) ROS_INFO("  ");
}

int main(int argc, char **argv)
{
	driver_blvd_controller::speed_wheel encoder;
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
	ros::Rate loop_rate(20);
	/* Subscriber */
    ros::Subscriber navigation =  nh.subscribe("Navigation_control_cmd", 10,navigationCallback); 
    /* Publisher */
    ros::Publisher speed_wheel = nh.advertise<driver_blvd_controller::speed_wheel>("wheel_encoder", 100);
	for (uint8_t i = 1; i < 3; i++)
	{	
		writeSpeedControlMode(i,BLVD02KM_SPEED_MODE_USE_DIGITALS);
		writeAcceleration(i,3);
		writeDeceleration(i,1);
		writeSpeed(i,BLVD20KM_SPEED_MIN);
		writeStop(i);
	}

	while(ros::ok())
	{
		for(uint8_t i = 1; i < 3; i++)
		{
			if(speed[i-1] > 0){
				writeForward(i);
			}else if(speed[i-1] < 0){
				writeReverse(i); 
			}
			writeSpeed(i,abs(speed[i-1]));
			readAlarm(i,&alarm_status[i-1]);
			feedbackSpeed(i,&encoder_value[i-1]);
		}
		encoder.wheel_letf = (double)(encoder_value[0]/30);
		encoder.wheel_right = (double)(encoder_value[1]/30);
		speed_wheel.publish(encoder);
		loop_rate.sleep();
 		ros::spinOnce();
	}

	Mb_close_device();

	return 0;
}

