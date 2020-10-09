#include "mbtcp/modbus.h"
#include <diagnostic_msgs/DiagnosticStatus.h>
#include "fx5u_hardware/fx5u_hardware.h"
#include <ros/ros.h>
using namespace std;

#define RED 1
#define GREEN 2
#define BULE 5
#define YELLOW 3 
#define NO_CL 0


void drivermotor_cmdcallback(const diagnostic_msgs::DiagnosticStatus& status)
{

}

void mlse_cmdcallback(const diagnostic_msgs::DiagnosticStatus& status)
{

}

diagnostic_msgs::DiagnosticStatus plc_msg;
void diagnomic_reg()
{
    plc_msg.name = "PLC-Fx5UC";
    plc_msg.hardware_id = "192.168.1.51:502";


}
int main(int argc, char **argv)
{
    /* create a modbus object */
    modbus *fx5uc = new modbus("192.168.1.51", 502);
    /* set slave id */
    fx5uc->modbus_set_slave_id(1);
    /* connect with the server */
    fx5uc->modbus_connect();  
    /* create hardware */
    FX5U_series device;
    /* create ros ndoe */
    ros::init(argc, argv, "PLC_control");
    ros::NodeHandle nh;
    ros::Rate loop_rate(60);
    /* Publisher */
    ros::Publisher cmd_PLC;
     cmd_PLC = nh.advertise<diagnostic_msgs::DiagnosticStatus>("PLC_infomation", 1000);
    uint16_t bitM[10];
    while(ros::ok())
    {   
        fx5uc->modbus_read_holding_registers(0, 1,bitM)     
        if(bitM[0] == ON)// Nếu M0 on 
        {
            if(bitM[5])device.D[1] = NO_CL;
            else if(bitM[6]){
                device.D[1] = RED;
                
            }
            else if(bitM[7])device.D[1] = BULE;
            fx5uc->modbus_write_register(0, device.D[1]); 
        }  
        diagnomic_reg();
        cmd_PLC.publish(plc_msg);                                           
        loop_rate.sleep();
        ros::spinOnce();
    }
    //close connection and free the memory
    fx5uc->modbus_close();  
    delete(fx5uc);
    return 0;
}  