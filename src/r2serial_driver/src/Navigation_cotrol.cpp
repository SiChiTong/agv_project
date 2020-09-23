#include "ros/ros.h"
#include "std_msgs/String.h"
#include <geometry_msgs/Twist.h>
#include "r2serial_driver/speed_wheel.h"
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>

#define RMID 183
#define TMID 184

uint8_t GetcheckSum( short nPacketSize, uint8_t *byArray);
uint8_t * Run(uint8_t id, int16_t data_Ctrl);
uint8_t * Stop_Free(uint8_t id);
uint8_t * Break(uint8_t id);
uint8_t * Main_BC_ON(uint8_t id);

#define Pi 3.1415926535
#define rad_rpm 9.5492965964254
#define L  0.255 // wheelbase (in meters per radian)
#define R  0.075 //wheel radius (in meters per radian)
#define v_max  100 // speed maximum of moter behind gear
#define v_min  2 // speed maximum of moter behind gear

int16_t W_l, W_r; // speed befor gear 
float k_v = 1; // percent speed %
float V_max ; // speed max when percent speed = 100%  (m/s)
int8_t mode;

void teleop_keyCallback(const geometry_msgs::Twist& msg)
{

  V_max = msg.linear.x;
  mode = int8_t(msg.linear.y);
  //ROS_INFO("%f",V_max);

}//teleop_keyCallback

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

  /* Kiem  tra van toc */
  if(fabs(v_r) > v_max) v_r = v_max;
  if(fabs(v_l) > v_max) v_l = v_max;

  if(fabs(v_r) < v_min) v_r = 0;
  if(fabs(v_l) < v_min) v_l = 0;


  /* van toc truoc hop so */

  W_r = (v_r/v_max)*3000; 
  W_l =  (v_l/v_max)*3000;

  //ROS_INFO("Wheel left: %d  Wheel right: %d", Wheel_left, Wheel_right);

} //cmd_velCallback


int main(int argc, char **argv)
{
  /**
   Khoi tao Node 
   */
  r2serial_driver::speed_wheel robot;
  robot.start = false;
  
  ros::init(argc, argv, "Navigation_control");
  ros::NodeHandle n;
  ros::Rate loop_rate(60);

  /* Publisher */
  ros::Publisher Navigation_control;
  Navigation_control = n.advertise<r2serial_driver::speed_wheel>("Navigation_control_cmd", 1000);


  /* Subscriber */
  ros::Subscriber cmd_vel;
  ros::Subscriber teleop_key;

  cmd_vel = n.subscribe("cmd_vel", 10,cmd_velCallback);
  teleop_key = n.subscribe("cmd_vel", 10,teleop_keyCallback);

  
  while (ros::ok())
  {
    
  /*
  * This is a message object. You stuff it with data, and then publish it.
  */

  robot.start = true;
  if(mode == 2 || mode == 1) robot.runon = true;
  else robot.runon = false;

  if(robot.runon == true)
  {

    robot.wheel_letf = -W_l;
    robot.wheel_right = W_r;
    //ROS_INFO("From Navigation_control: %d  %d", robot.wheel_letf, robot.wheel_right);

  }else{
    robot.wheel_letf = 0;
    robot.wheel_right = 0;
  }
    


  Navigation_control.publish(robot);
  loop_rate.sleep();

  ros::spinOnce();
  }
   
   return 0;
}


// From the input array, return the checksum
uint8_t GetcheckSum( short nPacketSize, uint8_t *byArray)
{

  uint8_t byTmp = 0;
  for(short i = 0; i < nPacketSize; i++ ) byTmp += *(byArray + i);
  return (~byTmp +1);

} //GetcheckSum
 
// Make data to send to driver 
uint8_t * Run(uint8_t id, int16_t data_Ctrl) 
{
  
  static uint8_t data_send[8] = {0,0,0,0,0,0,0,0}; 

  data_send[0] = RMID;
  data_send[1] = TMID;
  data_send[2] = id;
  data_send[3] = 130; 
  data_send[4] = 2;
  data_send[5] = data_Ctrl;
  data_send[6] = data_Ctrl >> 8;
  data_send[7] = GetcheckSum(7,data_send);
  return data_send;

}//Run

uint8_t * Stop_Free(uint8_t id) 
{
  static uint8_t data_send[8] = {0,0,0,0,0,0,0,0}; 

  data_send[0] = RMID;
  data_send[1] = TMID;
  data_send[2] = id;
  data_send[3] = 5; 
  data_send[4] = 1;
  data_send[5] = 1;
  data_send[6] = GetcheckSum(6,data_send); 
  return data_send;

}//Stop_Free

uint8_t * Break(uint8_t id) 
{
  static uint8_t data_send[8] = {0,0,0,0,0,0,0,0}; 

  data_send[0] = RMID;
  data_send[1] = TMID;
  data_send[2] = id;
  data_send[3] = 6; 
  data_send[4] = 1;
  data_send[5] = 1;
  data_send[6] = GetcheckSum(6,data_send); 
  return data_send;

}//Break

uint8_t * Main_BC_ON(uint8_t id)
{
  static uint8_t data_send[8] = {0,0,0,0,0,0,0,0}; 

  data_send[0] = RMID;
  data_send[1] = TMID;
  data_send[2] = id;
  data_send[3] = 10; 
  data_send[4] = 1;
  data_send[5] = 5;
  data_send[6] = GetcheckSum(6,data_send); 
  return data_send;
  
}//Main_BC_ON
