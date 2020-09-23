#include "modbus_lib/modbus.h"
#include "fx5u_hardware/fx5u_hardware.h"
#include <ros/ros.h>
using namespace std;

int main(int argc, char **argv)
{
    // create struct fx5u IO
    fx5u_hardware *fx5u_hw;
    // // create a modbus object
    modbus *fx5uc = new modbus("192.168.10.11", 502);
    // set slave id
    fx5uc->modbus_set_slave_id(1);
    // connect with the server
    fx5uc->modbus_connect();

    while(1)
    {
        // // read input bits (x0 -- x7)
        // fx5uc->modbus_read_input_bits(PORT0, 7, fx5u_hw -> device.x0);   
        // // read input bits (x10 -- x17)
        // fx5uc->modbus_read_input_bits(PORT1, 7, fx5u_hw -> device.x1); 
        // //for(int i = 0; i < 16 ; i++) cout << read_bits[i];
        // // read holding registers           function 0x03
        // fx5uc->modbus_read_holding_registers(0, 1, fx5u_hw -> device.D);
        // // // write single coil                function 0x05
        // fx5uc->modbus_write_coil(PORT0 + 1, ON);
        // //write single reg                 function 0x06
        // fx5uc->modbus_write_register(0, 123);
        // // write multiple coils             function 0x0F
        // fx5u_hw -> device.y0 = {true, true, true, true, true, true, true}; 
        // fx5uc->modbus_write_coils(PORT0,7,fx5u_hw -> device.y0);
        // // write multiple regs              function 0x10
        // uint16_t write_regs[4] = {123, 123, 123};
        // fx5uc->modbus_write_registers(0, 4, write_regs);
    }

    //close connection and free the memory
    fx5uc->modbus_close();
    
    delete(fx5uc);
    delete(fx5u_hw);

    return 0;
}