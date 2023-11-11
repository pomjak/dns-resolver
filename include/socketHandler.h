#pragma once

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>
#include <cerrno>
#include <vector>

#include "paramParser.h"

class communicate
{
private:
    int resolverSocket;
    struct sockaddr_in serverAddr, from;
    socklen_t len, fromlen;
    struct hostent *host_info;
    struct in_addr addr;

public:
    communicate() : resolverSocket(-1), len(sizeof(from)), fromlen(sizeof(from)) {}

    ~communicate()
    {
        if (resolverSocket != -1)
        {
            close(resolverSocket);
        }
    }

    void start(paramParser *param);
    void sendQuery(std::vector<uint8_t> msg);
    std::vector<uint8_t> recvResponse(void);
    void end(void);
};
