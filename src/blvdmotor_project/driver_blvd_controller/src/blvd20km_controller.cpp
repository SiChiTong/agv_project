#include "blvd20km_lib/BLVD20KM.h"
#include "mbrtu/modbusrtu.h"
#include <boost/thread/thread.hpp>

#define MOTOR_ADDRESS1 0X01
#define MOTOR_ADDRESS2 0X02

// void do_stuff(int* fd, termios *Mb_tio)
// {
// 	BLVD20KM_hieplm *right_motor = new BLVD20KM_hieplm(*fd,*Mb_tio,MOTOR_ADDRESS2);
// 	uint16_t right_speed;
// 	while (ros::ok())
// 	{
// 		right_motor -> writeSpeed(500); 
// 		if(right_motor -> readSpeed(&right_speed) == 0)
// 		printf("%d \n", right_speed); 

// 	}
// 	delete(right_motor);
// }

int main(int argc, char **argv)
{
	/* onpen comport */
	int fd = Mb_open_device("/dev/ttyUSB0",115200,1,8,1); /*even , 8 bit, 1 stop_bit*/
	/* Connect to comport */
 	BLVD20KM_hieplm *left_motor = new BLVD20KM_hieplm(fd,Mb_tio,MOTOR_ADDRESS1);
 	uint16_t left_speed = 0;
	/*create ros node*/
	ros::init(argc, argv, "Driver_motor");
	ros::NodeHandle nh;

	//boost::thread thread_b(do_stuff, &fd, &Mb_tio);

	while(ros::ok())
	{			
		left_motor -> writeSpeed(500); 
		if(left_motor -> readSpeed(&left_speed) == 0)
			ROS_INFO("%d", left_speed); 
 		ros::spinOnce();
	}

	// wait the second thread to finish
  	//thread_b.join();
	delete(left_motor);
	Mb_close_device(fd);
	return 0;
}