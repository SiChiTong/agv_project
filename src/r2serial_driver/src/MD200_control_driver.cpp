#include "ros/ros.h"
#include "std_msgs/String.h"
#include "r2serial_driver/MLS_Measurement.h"
#include "r2serial_driver/speed_wheel.h" 
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define RMID 183
#define TMID 184

/* Include funtion for Driver */
uint8_t GetcheckSum( short nPacketSize, uint8_t *byArray);
uint8_t * Run(uint8_t id, int16_t data_Ctrl);
uint8_t * Stop_Free(uint8_t id);
uint8_t * Break(uint8_t id);
uint8_t * Main_BC_ON(uint8_t id);

int16_t W_l = 0, W_r = 0; // speed befor gear 
bool runon_mls0, runon_mls1, runon_navigation;
bool start_mls0, start_mls1, start_navigation;
void speedwheel0Callback(const r2serial_driver::speed_wheel& robot)
{

  runon_mls0 = robot.runon;
  start_mls0 = robot.start;

  if(runon_mls0 == true && start_mls0 == true)
  {
    if(robot.error == 0)
    {
      W_l = robot.wheel_letf;
      W_r = robot.wheel_right;
    }else
        {
          W_l = 2;
          W_r = 2;
        }
    ROS_INFO("From MLS0: %d  %d", W_l, W_r);
  }
    
} //speedwheel0Callback

void speedwheel1Callback(const r2serial_driver::speed_wheel& robot)
{

  runon_mls1 = robot.runon;
  start_mls1 = robot.start;

  if(runon_mls1 == true && start_mls1 == true)
  {
    if(robot.error == 0)
    {
      W_l = robot.wheel_letf;
      W_r = robot.wheel_right;
    }else
        {
          W_l = 2;
          W_r = 2;
        }
    ROS_INFO("From MLS: %d  %d", W_l, W_r);
  }
    
} //speedwheel1Callback

void navigationCallback(const r2serial_driver::speed_wheel& robot)
{

  runon_navigation = robot.runon;
  start_navigation = robot.start;
  
  if(runon_navigation == true && start_navigation == true)
  {
    if(robot.error == 0)
    {

      W_l = robot.wheel_letf;
      W_r = robot.wheel_right;

    }else
        {
          W_l = 2;
          W_r = 2;
        }
    ROS_INFO("From Navigation: %d  %d", W_l, W_r);
  }
    
} //navigationCallback

int main(int argc, char **argv)
{
  /**
   Khoi tao Node 
   */
  
  ros::init(argc, argv, "MD200_control_driver");
  ros::NodeHandle n;
  ros::Rate loop_rate(60);
  ROS_INFO("MD200_control_driver starting");

  /* Publisher */
  ros::Publisher uc0Command;
  uc0Command = n.advertise<std_msgs::String>("uc0Command", 1000);
  
  /* Subscriber position line */
  ros::Subscriber speedwheel0;
  ros::Subscriber speedwheel1;
  ros::Subscriber navigation;

  speedwheel0 = n.subscribe("speedwheel0", 10,speedwheel0Callback);  
  speedwheel1 = n.subscribe("speedwheel1", 10,speedwheel1Callback); 
  navigation =  n.subscribe("Navigation_control_cmd", 10,navigationCallback); 

  /* Config USB to RS485 */
  for(uint8_t i  = 1;i<3;i++)
  {
    std_msgs::String msg_cfg;
    std::stringstream ss_cfg;
    uint8_t *a;
    a = Main_BC_ON(i);

    for(uint8_t j = 0; j < 7 ; j++) 
    {
      ss_cfg << *(a + j);
      ROS_INFO("%d",*(a + j));
    }

    msg_cfg.data = ss_cfg.str();
    uc0Command.publish(msg_cfg);
    usleep(30);
  }

  while (ros::ok())
  {

  /*
  * This is a message object. You stuff it with data, and then publish it.
  */

    if(start_mls0 == true && start_mls1 == true && start_navigation == true)
    {
      if(runon_mls0 != true && runon_mls1 != true && runon_navigation != true)
      {
        ROS_WARN("Not controler!!");
        W_l = 0;
        W_r = 0;
      }
    }
    
    for(uint8_t i = 1 ; i<3;i++)
    {

      int16_t V_buff;
      std_msgs::String msg;
      std::stringstream ss;
      uint8_t *a;

      if(i == 1) V_buff = W_l;
      else V_buff = W_r;
      
      if(fabs(V_buff) > 3)
      {

        a = Run(i,V_buff);
        for(uint8_t j = 0; j < 8 ; j++) ss << *(a + j);   
      } 

      else if(fabs(V_buff) < 3 && fabs(V_buff) >0)
      {

        a = Break(i);
        for(uint8_t j = 0; j < 7 ; j++) ss << *(a + j);   
      }

      else 
      {
        a = Stop_Free(i);
        for(uint8_t j = 0; j < 7 ; j++) ss << *(a + j);   
      }   
      
      msg.data = ss.str();
      uc0Command.publish(msg);
      loop_rate.sleep();

    }
   
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