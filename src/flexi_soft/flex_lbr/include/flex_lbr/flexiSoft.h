/*
** Created by Hiep on 10/20/2017.  (Women day)
*/

#ifndef FLEXISOFT_H
#define FLEXISOFT_H
#pragma once 

#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_MSG_LENGTH 260

class flexiSoft {

private: 
	bool _connected{};
    uint16_t PORT{};
    int _socket{};
    uint _msg_id{};
    int _slaveid{};
    std::string HOST;

    struct sockaddr_in _server{};

public:
	bool err{};
    int err_no{};
    std::string error_msg;

	flexiSoft(std::string host, uint16_t port);
	~flexiSoft();
	
	bool flexiSoft_connect(void);
	void flexiSoft_close(void) const;
};

#endif //FLEXISOFT_H