#include "socket_handler.h"
#include "param_parser.h"
#include "dns_msg.h"

#define UDP_LIMIT 512

void communicate::start(param_parser *param)
{
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(param->getPort());

    if (inet_pton(AF_INET, param->getServer().c_str(), &serverAddr.sin_addr) <= 0)
        throw std::runtime_error("converting to network format failed : ");

    if ((this->resolverSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        throw std::runtime_error("creating new socket failed :" + std::string(strerror(errno)));

    len = sizeof(serverAddr);
    fromlen = sizeof(from);
}

void communicate::sendQuery(std::vector<uint8_t> msg)
{
    if (connect(resolverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        throw std::runtime_error("connect failed :" + std::string(strerror(errno)));

    int bytesTx = send(this->resolverSocket, msg.data(), msg.size(), MSG_CONFIRM);
    if (bytesTx < 0)
        throw std::runtime_error("sending msg failed :" + std::string(strerror(errno)));

    if (getsockname(resolverSocket, (struct sockaddr *)&from, &len) == -1)
        throw std::runtime_error("getsockname() failed :" + std::string(strerror(errno)));
}

std::vector<uint8_t> communicate::recvResponse(void)
{
    char response[UDP_LIMIT];
    size_t bytesRead;

    while ((bytesRead = recv(resolverSocket, response, UDP_LIMIT, MSG_WAITALL)) > 0)
    {
        getpeername(resolverSocket, (struct sockaddr *)&from, &fromlen);
        std::vector<uint8_t> receivedData(response, response + bytesRead);
        return receivedData;
    }
    throw std::runtime_error("recv() failed" + std::string(strerror(errno)));
}

void communicate::end(void)
{
    close(resolverSocket);
}