#include "blvd20km_lib/BLVD20KM.h"
#define MOTOR_ADDRESS 0X01

uint16_t alarmState;

int main(int argc, char **argv)
{

	/*Rs485 driver blvd20km*/
	BLVD20KM_hieplm *left_motor = new BLVD20KM_hieplm(MOTOR_ADDRESS, (char *)DEFAULT_SERIALPORT,DEFAULT_BAUDRATE); 
	if(!left_motor -> begin()) return 1; 
	
	/*create ros node*/
	ros::init(argc, argv, "Driver_motor_controller");
	ros::NodeHandle nh;
	ros::Rate loop_rate(50);

	while(ros::ok())
	{	
		left_motor -> writeSpeed(500);
		if (left_motor -> readAlarm(&alarmState) == 0) {
			printf("Current alarm: ");
		} else {
		    printf("Cannot read alarm");
		}
		loop_rate.sleep();
   		ros::spinOnce();
	}

	left_motor -> closemotor();
	delete(left_motor);
	return 0;
}