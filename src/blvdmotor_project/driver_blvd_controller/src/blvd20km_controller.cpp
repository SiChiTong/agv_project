#include "blvd20km_lib/BLVD20KM.h"
#include "mbrtu/modbusrtu.h"
#define MOTOR_ADDRESS1 0X01
#define MOTOR_ADDRESS2 0X02


uint16_t alarmState;

int main(int argc, char **argv)
{
	/* onpen comport */
	modbusrtu mb;
	int fd = mb.Mb_open_device("/dev/ttyUSB0",115200,1,8,1);
	
	/* Connect to comport */
 	BLVD20KM_hieplm *left_motor = new BLVD20KM_hieplm(fd,0x01);
	
	// /*create ros node*/
	// ros::init(argc, argv, "Driver_motor");
	// ros::NodeHandle nh;
	// ros::Rate loop_rate(60);

	// while(ros::ok())
	// {	

	// 	left_motor -> writeSpeed(100);
	// 	right_motor -> writeSpeed(100);
	// 	// if (left_motor -> readAlarm(&alarmState) == 0) {
	// 	// 	ROS_INFO("Current alarm: ");
	// 	// } else {
	// 	//     ROS_INFO("Cannot read alarm");
	// 	// }
		
	// 	loop_rate.sleep();
 //   		ros::spinOnce();
	// }

	// left_motor -> closemotor();
	// right_motor -> closemotor();

	// delete(left_motor);
	// delete(right_motor);
	mb.Mb_close_device(fd);
	return 0;
}