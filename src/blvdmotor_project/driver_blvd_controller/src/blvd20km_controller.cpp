#include <ros/ros.h>
#include "mbrtu/modbusrtu.h"
#include <geometry_msgs/Twist.h>
#include <driver_blvd_controller/speed_wheel.h>
// #include <boost/thread/thread.hpp>

#define MOTOR_ADDRESS1 0X01
#define MOTOR_ADDRESS2 0X02
#define POLY           0xA001

/* Globle value */
static int speed[2];
static uint16_t alarm_status[2], encoder_value[2], warning_status[2];
static char port[30];    //port name
static int baud;     	 //baud rate 

//Process ROS receive from navigation message, send to uController
void navigationCallback(const driver_blvd_controller::speed_wheel& robot)
{
	speed[0] = robot.wheel_letf;
    speed[1] = robot.wheel_right;

    if(stat(DEFAULT_SERIALPORT, &sb) == 0)
    {
    	ROS_INFO("Connected ");
	    for(int i= 0; i<2; i++) ROS_INFO("  ");
	    ROS_INFO("                 Wheel left      Wheel right");
	    ROS_INFO("Command speed :  %d              %d", speed[0], speed[1]);
		ROS_INFO("Feedback speed:  %d              %d",(int16_t)encoder_value[0],(int16_t)encoder_value[1]);
		ROS_INFO("Warning record:  %x              %x",warning_status[0],warning_status[1]);
		ROS_INFO("Alarm record  :  %x              %x",alarm_status[0],alarm_status[1]);
		for(int i= 0; i<45; i++) ROS_INFO("  ");
    } 	    
}

int main(int argc, char **argv)
{
  	baud = DEFAULT_BAUDRATE;
	if (argc > 1) {
		if(sscanf(argv[1],"%d", &baud)!=1) {
		  ROS_ERROR("ucontroller baud rate parameter invalid");
		  return 1;
		}
	}

	strcpy(port, DEFAULT_SERIALPORT);
	if (argc > 2)
	strcpy(port, argv[2]);

	/*create ros node*/
	ros::init(argc, argv, "Driver_motor");
	ros::NodeHandle nh;
	ros::Rate loop_rate(20);
	
	/* Subscriber */
    ros::Subscriber navigation =  nh.subscribe("Navigation_control_cmd", 10,navigationCallback); 
    
    /* Publisher */
    driver_blvd_controller::speed_wheel encoder;
    ros::Publisher speed_wheel = nh.advertise<driver_blvd_controller::speed_wheel>("wheel_encoder", 100);
	
	while(ros::ok())
	{
		/* onpen comport */
		ROS_INFO("connection initializing (%s) at %d baud", port, baud);
		Mb_open_device(port,baud,1,8,1); /*even , 8 bit, 1 stop_bit*/
		
		if(stat(DEFAULT_SERIALPORT, &sb) == 0)
		{
			for (uint8_t i = 1; i < 3; i++)
			{	
				writeResetAlarm(i);
				sleep(1); 
				writeSpeedControlMode(i,BLVD02KM_SPEED_MODE_USE_DIGITALS);
				writeAcceleration(i,2);
				writeDeceleration(i,1);
				writeSpeed(i,BLVD20KM_SPEED_MIN);
				writeStop(i);
				clearAlarmRecords(i);
				clearWarningRecords(i);
			}
		}
			
		while(ros::ok())
		{
			if(stat(DEFAULT_SERIALPORT, &sb) != 0){
				for(int i= 0; i<51; i++) ROS_INFO("  ");
				ROS_INFO("Disconnected");
				break;
			} 

			for(uint8_t i = 1; i < 3; i++)
			{
				if(speed[i-1] > 0){
					writeForward(i);
				}else if(speed[i-1] < 0){
					writeReverse(i); 
				}
				writeSpeed(i,abs(speed[i-1]));
				feedbackSpeed(i,&encoder_value[i-1]);
				readAlarm(i,&alarm_status[i-1]);
				readWarning(i,&warning_status[i-1]);
			}

			encoder.wheel_letf = (double)(encoder_value[0]/30);
			encoder.wheel_right = (double)(encoder_value[1]/30);
			speed_wheel.publish(encoder);
			loop_rate.sleep();
			ros::spinOnce();
		}

		Mb_close_device();
		sleep(2);
	}

	return 0;
}

