#pragma once 

#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include <cerrno>
#include <vector>

#include"param_parser.h"


class communicate {
    private:
        int resolver_socket;
        struct sockaddr_in server_addr;

    public:
        communicate()
        {
            this->resolver_socket=-1;
        }
        void start(param_parser *param);
        void send(std::vector<uint8_t> msg);
        std::vector<uint8_t> recv(void);
        void end(void);


};
