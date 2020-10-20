#include <ros/ros.h>
#include "flex_lbr/flexiSoft.h"

int main(int argc, char **argv)
{

	/* create a object */
	flexiSoft *flex = new flexiSoft("192.168.1.21", 9102);
	/* connect with the server */
	flex->flexiSoft_connect();
    /* create ros ndoe */
    ros::init(argc, argv, "FlexiSoft_Interface");
    ros::NodeHandle nh;
    ros::Rate loop_rate(20);
    
    while(ros::ok())
    { 

    }
     //close connection and free the memory
    flex->flexiSoft_close();   
    delete(flex);
	return 0;
}