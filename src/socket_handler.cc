#include "socket_handler.h"
#include "param_parser.h"
#include "dns_msg.h"

#define UDP_LIMIT 512

void communicate::start(param_parser *param)
{
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(param->getPort());

    host_info = gethostbyname(param->getServer().c_str());

    if (host_info == NULL || host_info->h_addr_list[0] == NULL)
        throw std::runtime_error("No IP address found for the server");

    for (int i = 0; host_info->h_addr_list[i]; i++)
    {
        memcpy(&addr, host_info->h_addr_list[i], sizeof(struct in_addr));

        if (inet_pton(AF_INET, inet_ntoa(addr), &serverAddr.sin_addr) <= 0)
        {
            if (host_info->h_addr_list[i + 1] == nullptr)
                throw std::runtime_error("Converting IP address to network format failed");
            else
                continue;
        }
        else
            break;
    }

    if ((this->resolverSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        throw std::runtime_error("Creating new socket failed: " + std::string(strerror(errno)));

    len = sizeof(serverAddr);
    fromlen = sizeof(from);
}

void communicate::sendQuery(std::vector<uint8_t> msg)
{
    if (connect(resolverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        throw std::runtime_error("Connect failed: " + std::string(strerror(errno)));

    auto bytesTx = send(this->resolverSocket, msg.data(), msg.size(), MSG_CONFIRM);
    if (bytesTx < 0)
        throw std::runtime_error("Sending message failed: " + std::string(strerror(errno)));

    if (getsockname(resolverSocket, (struct sockaddr *)&from, &len) == -1)
        throw std::runtime_error("getsockname() failed: " + std::string(strerror(errno)));
}

std::vector<uint8_t> communicate::recvResponse(void)
{
    char response[UDP_LIMIT];
    auto bytesRead = recv(resolverSocket, response, UDP_LIMIT, MSG_WAITALL);

    if (bytesRead > 0)
    {
        getpeername(resolverSocket, (struct sockaddr *)&from, &fromlen);
        return std::vector<uint8_t>(response, response + bytesRead);
    }

    throw std::runtime_error("recv() failed: " + std::string(strerror(errno)));
}

void communicate::end(void)
{
    if (resolverSocket != -1)
    {
        close(resolverSocket);
        resolverSocket = -1; // Mark socket as closed
    }
}
