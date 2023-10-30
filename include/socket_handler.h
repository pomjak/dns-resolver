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
        struct sockaddr_in server_addr,from;
        socklen_t len, fromlen;

    public:
        communicate()
        {
            this->resolver_socket=-1;
        }
        void start(param_parser *param);
        void sendQuery(std::vector<uint8_t> msg);
        std::vector<uint8_t> recvResponse(void);
        void end(void);


};
