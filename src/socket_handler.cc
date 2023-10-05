#include "socket_handler.h"
#include "param_parser.h"
#include "dns_msg.h"

#define UDP_LIMIT 255

void communicate::start(param_parser *param)
{
    if ((this->resolver_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        throw std::runtime_error("creating new socket failed" + std::string(strerror(errno)));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(param->get_port());

    if (inet_pton(AF_INET, param->get_server().c_str(), &server_addr.sin_addr) <= 0)
        throw std::runtime_error("converting to network fromat failed");
}
void communicate::send(std::vector<uint8_t> msg)
{
    int bytes_tx = sendto(this->resolver_socket, msg.data(), sizeof(msg.data()), MSG_CONFIRM, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bytes_tx < 0)
        throw std::runtime_error("sending msg failed" + std::string(strerror(errno)));
}
void communicate::recv(void)
{
    socklen_t len;
    char srv_response[UDP_LIMIT];
    int bytes_rx = recvfrom(resolver_socket, (char *)srv_response, UDP_LIMIT, MSG_WAITALL, (struct sockaddr *)&server_addr, &len);

    if (bytes_rx < 0)
        throw std::runtime_error("recv of response failed" + std::string(strerror(errno)));
}
void communicate::end(void)
{
    close(resolver_socket);
}