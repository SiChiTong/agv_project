#include "mbtcp/modbus.h"
#include "plc_fx5uc/plc_cmd.h"
#include "fx5u_hardware/fx5u_hardware.h"
#include <ros/ros.h>
using namespace std;

#define RED 1
#define GREEN 2
#define BULE 5
#define YELLOW 3 
#define NO_CL 0

struct IO_msgs
{
    bool error;
    bool warning;
    bool normal;
};
// Global value
IO_msgs IO_msgs;

void plc_cmdcallback(const plc_fx5uc::plc_cmd& msg)
{
    IO_msgs.error = msg.error;
    IO_msgs.warning = msg.warning;
    IO_msgs.normal = msg.normal;
}

int main(int argc, char **argv)
{
    /* create a modbus object */
    modbus *fx5uc = new modbus("192.168.10.11", 502);
    /* set slave id */
    fx5uc->modbus_set_slave_id(1);
    /* connect with the server */
    fx5uc->modbus_connect();  

    /* create hardware */
    fx5u_hardware *hardware;

    /* create ros ndoe */
    ros::init(argc, argv, "PLC_control");
    ros::NodeHandle nh;
    ros::Rate loop_rate(60);
    /* Subscriber */
    ros::Subscriber fx5uc_cmd;
    fx5uc_cmd = nh.subscribe("fx5uc_cmd", 10,plc_cmdcallback);

    while(ros::ok())
    {
        
        if (IO_msgs.error)  hardware -> device.D[1] = RED;
            else if(IO_msgs.warning) hardware -> device.D[1] = YELLOW;
            else if(IO_msgs.normal)  hardware -> device.D[1] = BULE;
        fx5uc->modbus_write_register(1, hardware -> device.D[1]);

        loop_rate.sleep();
        ros::spinOnce();
    }

    //close connection and free the memory
    fx5uc->modbus_close();  
    delete(fx5uc);
    delete(hardware);
    return 0;
}  