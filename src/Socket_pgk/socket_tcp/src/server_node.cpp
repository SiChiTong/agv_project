#include "server_lib/Server.h"
#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>
#include <stdio.h>


int main(int argc, char **argv)
{
  Server *Sr = new Server(8888);
  ros::init(argc, argv, "Ethernet_Controller");
  ros::NodeHandle nh;
  ros::Publisher client_pub[Sr -> max_clients];
  
  while(ros::ok())
  {
  	Sr -> socketloop();
    for(int i = 0; i < Sr -> max_clients; i++)
    {
      int count = 0;
      while(Sr -> client_class[i].ip_address[count] != '\0')
      {
        count++;
      }
      if(count!= 0) 
      {
        cout << Sr -> client_class[i].ip_address << " --- "<< Sr -> client_class[i].databuffer << endl;
        client_pub[i] = nh.advertise<std_msgs::String>(Sr -> client_class[i].ip_address, 100);
      }
    }
  }
  ros::spinOnce();
  delete Sr; 
  return 0;
}



