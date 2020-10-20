/*
** Created by Hiep on 10/20/2017.  (Women day)
*/
#include "flex_lbr/flexiSoft.h"

/**
 * Main Constructor of Modbus Connector Object
 * @param host IP Address of Host
 * @param port Port for the TCP Connection
 * @return     A Modbus Connector Object
 */

 flexiSoft::flexiSoft(std::string host, uint16_t port=502) {
    HOST = host;
    PORT = port;
    _slaveid = 1;
    _msg_id = 1;
    _connected = false;
    err = false;
    err_no = 0;
    error_msg = "";
}

/**
 * Destructor of Modbus Connector Object
 */

 flexiSoft::~flexiSoft(void) = default;


/**
 * Build up a Modbus/TCP Connection
 * @return   If A Connection Is Successfully Built
 */
bool flexiSoft::flexiSoft_connect() {
    if(HOST.empty() || PORT == 0) {
        std::cout << "Missing Host and Port" << std::endl;
        return false;
    } else {
        std::cout << "Found Proper Host "<< HOST << " and Port " <<PORT <<std::endl;
    }

    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if(_socket == -1) {
        std::cout <<"Error Opening Socket" <<std::endl;
        return false;
    } else {
        std::cout <<"Socket Opened Successfully" << std::endl;
    }

    struct timeval timeout{};
    timeout.tv_sec  = 20;  // after 20 seconds connect() will timeout
    timeout.tv_usec = 0;
    setsockopt(_socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    _server.sin_family = AF_INET;
    _server.sin_addr.s_addr = inet_addr(HOST.c_str());
    _server.sin_port = htons(PORT);

    if (connect(_socket, (struct sockaddr*)&_server, sizeof(_server)) < 0) {
        std::cout<< "Connection Error" << std::endl;
        return false;
    }

    std::cout<< "Connected" <<std::endl;
    _connected = true;
    return true;
}

/**
 * Close the Modbus/TCP Connection
 */
void flexiSoft::flexiSoft_close() const {
    close(_socket);
    std::cout <<"Socket Closed" <<std::endl;
}
