#include "modbus_lib/modbus.h"
#include "fx5u_hardware/fx5u_hardware.h"
#include <ros/ros.h>
using namespace std;

int main(int argc, char **argv)
{
    // create struct fx5u IO
    fx5u_hardware *fx5u_hw;
    // create a modbus object
    modbus *fx5uc = new modbus("192.168.10.11", 502);
    // set slave id
    fx5uc->modbus_set_slave_id(1);
    // connect with the server
    fx5uc->modbus_connect();

    while(1)
    {
        cout << fx5u_hw -> device.portx[0];
        // read coil                        function 0x01
        bool read_coil[16];
        fx5uc->modbus_read_coils(fx5u_hw -> device.portx[0], 16, read_coil);
        //for(int i = 0; i < 16 ; i++) cout << read_coil[i];
        // read input bits(discrete input)  function 0x02
        bool read_bits[16];
        fx5uc->modbus_read_input_bits(fx5u_hw -> device.portx[0], 16, read_bits);   
        //for(int i = 0; i < 16 ; i++) cout << read_bits[i];
        // read holding registers           function 0x03
        uint16_t read_holding_regs[1];
        fx5uc->modbus_read_holding_registers(0, 1, read_holding_regs);
        cout << " " << read_holding_regs[0] << endl;
        // // write single coil                function 0x05
        fx5uc->modbus_write_coil(8, true);
        //write single reg                 function 0x06
        fx5uc->modbus_write_register(0, 123);
        // write multiple coils             function 0x0F
        bool write_cols[16] = {true, true, true, true, true, true, true, true,true,
                                         true, true, true, true, true, true, true}; 
        fx5uc->modbus_write_coils(1,1,write_cols);
        // write multiple regs              function 0x10
        uint16_t write_regs[4] = {123, 123, 123};
        fx5uc->modbus_write_registers(0, 4, write_regs);
    }

    // close connection and free the memory
    fx5uc->modbus_close();
    
    delete(fx5uc);
    delete(fx5u_hw);

    return 0;
}