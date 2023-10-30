#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include <cerrno>
#include <vector>
#include <netdb.h>

#include "param_parser.h"

class communicate
{
private:
    int resolver_socket;
    struct sockaddr_in server, from;
    struct addrinfo DomainName;
    socklen_t len, fromlen;
    struct addrinfo *res;

public:
    communicate()
    {
        this->resolver_socket = -1;
        memset(&DomainName, 0, sizeof(DomainName));
        memset(&server, 0, sizeof(server));
        memset(&from, 0, sizeof(from));
    }
    void start(param_parser *param);
    void sendQuery(std::vector<uint8_t> msg);
    std::vector<uint8_t> recvResponse(void);
    void end(void);
};
