#include <ros/ros.h>
#include "mbrtu/modbusrtu.h"
#include <geometry_msgs/Twist.h>
#include "r2serial_driver/speed_wheel.h"
#include "std_msgs/String.h"
// #include <boost/thread/thread.hpp>

#define MOTOR_ADDRESS1 0X01
#define MOTOR_ADDRESS2 0X02
#define POLY           0xA001

#define Pi 			   3.1415926535
#define rad_rpm        9.5492965964254
#define L              0.255 // wheelbase (in meters per radian)
#define R  			   0.075 //wheel radius (in meters per radian)
#define w_max  		   4000 // speed maximum of moter befor gear
#define w_min  		   80 // speed maximum of moter befor gear

int16_t W_l, W_r; // speed befor gear 
float k_v = 1; 	  // percent speed %
float V_max ;     // speed max when percent speed = 100%  (m/s)
float K = 30;          // He so chuyen
//Process ROS receive from navigation message, send to uController
void cmd_velCallback(const geometry_msgs::Twist& msg)
{
	float V;  // forward velocity (ie meters per second)
	float W;  // angular velocity (ie radians per second)
	float v_r; // clockwise angular velocity of right wheel (ie radians per second)
	float v_l; // counter-clockwise angular velocity of left wheel (ie radians per second)
	float w_r, w_l; // speed rad/s of one

	V_max = msg.linear.x;  W = msg.angular.z;
	V = V_max*k_v;

	/* Van toc goc 2 banh */
	w_r = ((2 * V) + (W * L)) / (2 * R);   //(rad/s)
	w_l = ((2 * V) - (W * L)) / (2 * R);   //(rad/s)

	/* Van toc 2 banh */
	v_r = w_r*rad_rpm;  // (rpm)  
	v_l = w_l*rad_rpm;  // (rpm) 

	/* van toc truoc hop so */
	W_r = v_r*K; 
	W_l = v_l*K;
	/* Kiem  tra van toc */
	if(fabs(W_r) > w_max) W_r = w_max;
	if(fabs(W_l) > w_max) W_l = w_max;

	if(fabs(W_r) < w_min) W_r = 0;
	if(fabs(W_l) < w_min) W_l = 0;

	ROS_INFO("Wheel left: %d  Wheel right: %d", W_l, W_r);
} //cmd_velCallback


int main(int argc, char **argv)
{
	/* onpen comport */
	int fd = Mb_open_device("/dev/ttyUSB0",115200,1,8,1); /*even , 8 bit, 1 stop_bit*/
	/*create ros node*/
	ros::init(argc, argv, "Driver_motor");
	ros::NodeHandle nh;
	ros::Rate loop_rate(60);
	/* Subscriber */
    ros::Subscriber cmd_vel;
	cmd_vel = nh.subscribe("cmd_vel", 10,cmd_velCallback);
	while(ros::ok())
	{	
		// writeRegister(fd,MOTOR_ADDRESS1,ADDR_SPEED0_L, 500);
		// writeRegister(fd,MOTOR_ADDRESS1,ADDR_MOTOR_CONTROL, 
		// 				createMotorControl16bit(MOTOR_DIRECTOIN_FORWARD,true,true,0));
 		loop_rate.sleep();
 		ros::spinOnce();
	}

	Mb_close_device(fd);
	return 0;
}