#include "blvd20km_lib/BLVD20KM.h"
#define MOTOR_ADDRESS1 0X01
#define MOTOR_ADDRESS2 0X02


uint16_t alarmState;

int main(int argc, char **argv)
{

	/*Rs485 driver blvd20km*/
	BLVD20KM_hieplm *left_motor = new BLVD20KM_hieplm(MOTOR_ADDRESS1, (char *)DEFAULT_SERIALPORT,DEFAULT_BAUDRATE); 
	if(!left_motor -> begin()) return 1; 
	
	BLVD20KM_hieplm *right_motor = new BLVD20KM_hieplm(MOTOR_ADDRESS2, (char *)DEFAULT_SERIALPORT,DEFAULT_BAUDRATE); 
	if(!right_motor -> begin()) return 1; 
	
	/*create ros node*/
	ros::init(argc, argv, "Driver_motor");
	ros::NodeHandle nh;
	ros::Rate loop_rate(60);

	while(ros::ok())
	{	

		left_motor -> writeSpeed(100);
		right_motor -> writeSpeed(100);
		// if (left_motor -> readAlarm(&alarmState) == 0) {
		// 	ROS_INFO("Current alarm: ");
		// } else {
		//     ROS_INFO("Cannot read alarm");
		// }
		
		loop_rate.sleep();
   		ros::spinOnce();
	}

	left_motor -> closemotor();
	right_motor -> closemotor();

	delete(left_motor);
	delete(right_motor);
	return 0;
}