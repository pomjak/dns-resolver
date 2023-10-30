#include "socket_handler.h"
#include "param_parser.h"
#include "dns_msg.h"

#define UDP_LIMIT 512

void communicate::start(param_parser *param)
{

    server.sin_family = AF_UNSPEC;
    server.sin_port = htons(param->get_port());

    int status = getaddrinfo(param->get_server().c_str(), (std::to_string(param->get_port()).c_str()), &DomainName, &res);

    if (status != 0)
    {
        throw std::runtime_error("resolving ip addr of dns server failed");
    }

    // if (inet_pton(AF_INET, param->get_server().c_str(), &server.sin_addr) <= 0)
    //     throw std::runtime_error("converting to network format failed");

    if ((this->resolver_socket = socket(result->ai_addr,result->ai_socktype ,result->ai_protocol)) < 0)
        throw std::runtime_error("creating new socket failed" + std::string(strerror(errno)));
    len = sizeof(server);
    fromlen = sizeof(from);
}

void communicate::sendQuery(std::vector<uint8_t> msg)
{
    if (connect(resolver_socket, (struct sockaddr *)&server, sizeof(server)) == -1)
        throw std::runtime_error("connect failed" + std::string(strerror(errno)));

    int bytesTx = send(this->resolver_socket, msg.data(), msg.size(), MSG_CONFIRM);
    if (bytesTx < 0)
        throw std::runtime_error("sending msg failed" + std::string(strerror(errno)));

    if (getsockname(resolver_socket, (struct sockaddr *)&from, &len) == -1)
        throw std::runtime_error("getsockname() failed" + std::string(strerror(errno)));
}

std::vector<uint8_t> communicate::recvResponse(void)
{
    char response[UDP_LIMIT];
    size_t bytesRead;

    while ((bytesRead = recv(resolver_socket, response, UDP_LIMIT, MSG_WAITALL)) > 0)
    {
        getpeername(resolver_socket, (struct sockaddr *)&from, &fromlen);
        std::vector<uint8_t> receivedData(response, response + bytesRead);
        return receivedData;
    }
     throw std::runtime_error("recv() failed" + std::string(strerror(errno)));
}

void communicate::end(void)
{
    close(resolver_socket);
}