#include "modbus_lib/modbus.h"
#include "plc_fx5uc/plc_cmd.h"
#include <ros/ros.h>

using namespace std;

struct cmd
{
    bool error
    bool warning
    bool normal

    
};

void plc_cmdcallback(const plc_fx5uc::plc_cmd& msg)
{

}

int main(int argc, char **argv)
{
    // // create a modbus object
    modbus *fx5uc = new modbus("192.168.10.11", 502);
    // set slave id
    fx5uc->modbus_set_slave_id(1);
    // connect with the server
    fx5uc->modbus_connect();
    // create ros ndoe
    ros::init(argc, argv, "PLC_control");
    ros::NodeHandle n;
    ros::Rate loop_rate(60);

    while(1)
    {
        
    }

    //close connection and free the memory
    fx5uc->modbus_close();  
    delete(fx5uc);

    return 0;
}  