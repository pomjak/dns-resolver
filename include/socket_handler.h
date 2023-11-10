#pragma once 

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <cstring>
#include <cerrno>
#include <vector>

#include"param_parser.h"


class communicate {
    private:
        int resolverSocket;
        struct sockaddr_in serverAddr,from;
        socklen_t len, fromlen;
        struct hostent *host_info;
        struct in_addr addr;

    public:
        communicate()
        {
            this->resolverSocket=-1;
        }
        void start(param_parser *param);
        void sendQuery(std::vector<uint8_t> msg);
        std::vector<uint8_t> recvResponse(void);
        void end(void);


};
