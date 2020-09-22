#include "ros/ros.h"
#include "std_msgs/String.h"
#include "r2serial_driver/MLS_Measurement.h"
#include <geometry_msgs/Twist.h>
#include "r2serial_driver/speed_wheel.h" 
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#define DEFAULT_DIRECTION "forward" 

bool string_compare(char *str1, const char *str2);

/* Include funtion for Sick line */
uint8_t Getbit(uint8_t x, uint8_t number);
uint8_t * Extern_data(uint8_t data);
bool Islinegood(uint8_t data);
bool IsSensorflipped(uint8_t data);
bool CheckPolarity(uint8_t data);
bool Checkreadingcode(uint8_t data);
void Gacceleration(const float step);
void Deceleration(const float step);

/* CallBack Subcriber */
#define Pi 3.1415926535
#define rad_rpm 9.5492965964254
// #define L 0.255 // khoang cach 2 banh
// #define Lm 0.355 // Khoang cach  tu tam truc banh den tam do line 
// #define R 0.075 //wheel radius (in meters per radian)
// #define v_max_wheel 100 // speed maximum of moter behind gear
// #define v_min_wheel 0 // speed maximum of moter behind gear
// #define Lt 0.09

//Global data

float present_speed; // percent speed %
float present_speed_setting; //percent speed setting
float speed_setting; // speed max when percent speed = 100%  (m/s)
float L; // khoang cach 2 banh
float Lm; // Khoang cach  tu tam truc banh den tam do line 
float R; //wheel radius (in meters per radian)
float v_max_wheel; // speed maximum of moter behind gear
float v_min_wheel; // speed maximum of moter behind gear
float Lt;

int ucIndex; //ucontroller index number
bool line_good;
uint8_t track_level;
uint8_t error_register;
int16_t W_l, W_r; // speed befor gear]
int8_t dir = 1;
int8_t mode;
float buff_v = present_speed; 

void teleop_keyCallback(const geometry_msgs::Twist& msg)
{

	speed_setting = msg.linear.x;
	mode = int8_t(msg.linear.y);
	//ROS_INFO("%f",V_max);

}//teleop_keyCallback

void mlsCallback(const r2serial_driver::MLS_Measurement& msg)
{

	uint8_t *status;   uint8_t *lcp; 

	lcp = Extern_data(msg.lcp);
	/* 
	* #LCP
	* The following assignment applies (see "Outputof line center points", page 40):
	* 0 => No track found
	* 2 => One track found
	* 3 => Two tracks found: Left diverter
	* 6 => Two tracks found: Left diverter
	* 7 => Three tracks found or 90 °C intersection2
	*/
	uint8_t lcp_nummber = lcp[0] | lcp[1]<<1 | lcp[2]<<2;

	/* 
	* Marker 
	* Bit 0 is the introductory character bit
	* Bit 1...4 present code 1...15
	*/
	uint8_t Marker = lcp[3] | lcp[4]<<1 | lcp[5]<<2 | lcp[6]<<3 | lcp[7]<<4;


	status = Extern_data(msg.status);
	/* 
	* True is  Sufficiently strong track detected
	* Fasle is  No track or track too weak
	*/
	line_good = Islinegood(status[0]);

	/*
	* Display of magnetic field strength in accor‐dance
	*/
	track_level = status[0] | status[1]<<1 | status[2]<<2;
	/*
	* Indicates whether or not the measuring rangehas been inverted
	* False => Negative positions on cable outlet side
	* True => Positive positions on cable outlet side
	*/
	bool Sensor_fipped = IsSensorflipped(status[4]);

	/*
	* Indicates whether the upper surface of themagnetic tape is magnetized to the north orsouth pole
	* False => North pole
	* True => South pole
	*/
	bool Prolarity = CheckPolarity(status[5]);

	/* 
	* False => No code present to read
	* True => Sensor is reading code
	*/
	bool Reading_code = Checkreadingcode(status[6]);

	/* Error register */
	error_register = msg.error;


	float V;  // forward velocity (ie meters per second)
	float speed;
	float angle_error; // goc lech 
	float W;  // angular velocity (ie radians per second)
	float v_r; // clockwise angular velocity of right wheel (ie radians per second)
	float v_l; // counter-clockwise angular velocity of left wheel (ie radians per second)

	// if(msg.position[0] == 0 && msg.position[2] == 0) 
	// {
	// 	present_speed_setting = 1 - fabs(msg.position[1])*6;
	// 	if(present_speed_setting < 0.3) present_speed_setting = 0.3;

	// }else if(msg.position[0] != 0 && msg.position[2] != 0)
	// 	{
	// 		present_speed_setting = (fabs(msg.position[0]) - fabs(msg.position[1]))*6; 
	// 		if(present_speed_setting > 1 ) present_speed_setting =1;
	// 		else if(present_speed_setting < 0.3) present_speed_setting = 0.3;
	// 	} 
		
	V = fabs(speed_setting*present_speed); // V cai dat 
	
	angle_error = atan(msg.position[1]/Lm);
	
	v_l = ((1 - (L*angle_error)/(2*Lt)) * (V/R)) * rad_rpm;
	if(fabs(v_l) > v_max_wheel) v_l = v_max_wheel;
	if(fabs(v_l) < v_min_wheel) v_l = 0;

	v_r = ((1 + (L*angle_error)/(2*Lt)) * (V/R)) * rad_rpm;
	if(fabs(v_r) > v_max_wheel) v_r = v_max_wheel;
	if(fabs(v_r) < v_min_wheel) v_r = 0;

	W_l = (v_l/v_max_wheel)*3000;
	W_r = (v_r/v_max_wheel)*3000;


} //echo_line_previousCallback

/* MAIN */
int main(int argc, char **argv)
{
	char direction[20]; 
	char str[20];
 	char topicSubscribe[20];
    char topicPublish[20];
    char param[70];

    /***Create Node */
	ros::init(argc, argv, "MLS_Line_Sick_Forward");
	ros::NodeHandle n;	
	ros::Rate loop_rate(60);
	r2serial_driver::speed_wheel robot;
    robot.start = false;

	//Open and initialize the serial port to the uController
  	if (argc > 1) 
  	{
	    if(sscanf(argv[1],"%d", &ucIndex)==1) 
	    {
			sprintf(topicSubscribe, "mls%d",ucIndex);
			sprintf(topicPublish, "speedwheel%d",ucIndex);
			
	    }else 
	    	{
			    ROS_ERROR("Not connect mls%d !!",ucIndex);
			    return 1;
	  		}
    }else 
     	{
		    strcpy(topicSubscribe, "mls0");
		    strcpy(topicPublish, "speedwheel0");
  		}

  	sprintf(param,"/consept_mls%d/present_speed_setting",ucIndex);
  	n.getParam(param,present_speed_setting);
  	sprintf(param,"/consept_mls%d/L",ucIndex);
  	n.getParam(param,L);
  	sprintf(param,"/consept_mls%d/Lm",ucIndex);
  	n.getParam(param,Lm);
	sprintf(param,"/consept_mls%d/R",ucIndex);
  	n.getParam(param,R);
  	sprintf(param,"/consept_mls%d/Lt",ucIndex);
  	n.getParam(param,Lt);
  	sprintf(param,"/consept_mls%d/v_max_wheel",ucIndex);
  	n.getParam(param,v_max_wheel);
  	sprintf(param,"/consept_mls%d/v_min_wheel",ucIndex);
  	n.getParam(param,v_min_wheel);

  	strcpy(direction, DEFAULT_DIRECTION);
  	if (argc > 2) strcpy(direction, argv[2]);
  	
  	if(string_compare(direction,"forward") == true) dir = 1; 
  		else if(string_compare(direction,"backward") == true) dir = -1;
 			else
			{
				ROS_ERROR("Robot drirection invalid");
				return 1;
 			}

 	ROS_INFO("MLS line sick %d starting",ucIndex);	

	/* Publisher */
	ros::Publisher speedwheel;
	speedwheel = n.advertise<r2serial_driver::speed_wheel>(topicPublish, 1000);

	/* Subscriber position line */
	ros::Subscriber mls;
	ros::Subscriber teleop_key;

	mls = n.subscribe(topicSubscribe, 10,mlsCallback);  
	teleop_key = n.subscribe("cmd_vel", 10,teleop_keyCallback);
	
	clock_t begin_time = clock();
	clock_t end_time;

	while (ros::ok())
	{
		/* This is a message object. You stuff it with data, and then publish it. */
		end_time = clock();
		if(float(end_time-begin_time)/CLOCKS_PER_SEC*1000  >= 0.1) // 10 ms
		{
			if(present_speed < present_speed_setting) Gacceleration(0.01);
			else if(present_speed > present_speed_setting) Deceleration(0.02);
			else present_speed = present_speed_setting;
			
			begin_time = clock();
		}

	    robot.error = error_register;
	    robot.start = true;
	
	    if(int8_t(speed_setting/fabs(speed_setting)) == dir && speed_setting !=0 && mode == 3)
		{
			robot.runon = true;
			if(line_good == true)
			{

				robot.wheel_letf = W_l*dir*(-1);
				robot.wheel_right = W_r*dir;
				if(track_level <= 3 && track_level > 0) ROS_WARN("From MLS%d: track too weak!!",ucIndex);

			}else 
				{
				  	ROS_ERROR("From MLS%d: no track!!", ucIndex);
				    robot.wheel_letf = 0;
				    robot.wheel_right = 0;
				}

		}else robot.runon = false;

		speedwheel.publish(robot);
		loop_rate.sleep();
		ros::spinOnce();
	}
	


	return 0;
}


/* Include funtion for Sick line */
// Get value bit thu n 
uint8_t Getbit(uint8_t x, uint8_t number)
{
	return (x >> number) & 0b00000001;
}//Getbit

uint8_t * Extern_data(uint8_t data)
{
	static uint8_t element[8] = {0,0,0,0,0,0,0,0}; 
	for(int i = 0; i<8;i++)
	{
	element[i] = Getbit(data,i);
	}
	return element;
  
}//Extern_data

bool Islinegood(uint8_t data)
{
	if(data == 1) return true;
	else return false;
}//Islinegood

bool IsSensorflipped(uint8_t data)
{
	if(data == 1) return true;
	else return false;
}//IsSensorflipped

bool CheckPolarity(uint8_t data)
{
	if(data == 1) return true;
	else return false;
}//CheckPolarity

bool Checkreadingcode(uint8_t data)
{
	if(data == 1) return true;
	else return false;
}//Checkreadingcode

bool string_compare(char *str1, const char *str2)
{
	int count = 0;

	if((unsigned)strlen(str1) == (unsigned)strlen(str2))
	{
		for(int i=0; i < (unsigned)strlen(str2); i++)
		{
			*(str1 + i) = *(str2 + i);
			count ++;
		}
		if(count == (unsigned)strlen(str2)) return true;
		else return false;
		
	}else return false;
	
}

void Gacceleration(const float step)
{
	if(present_speed >= present_speed_setting) present_speed = present_speed_setting;
	else present_speed += step;
}

void Deceleration(const float step)
{
	if(present_speed >= present_speed_setting) present_speed = present_speed_setting;
	else present_speed -= step;
}