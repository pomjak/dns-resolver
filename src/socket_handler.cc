#include"socket_handler.h"
#include"param_parser.h"

#define UDP_LIMIT 255

void communicate::start(param_parser *param)
{
    if ((this->resolver_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        throw ("creating new socket failed");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(param->get_port());

    if (inet_pton(AF_INET,param->get_server().c_str(), &server_addr.sin_addr) <= 0)
        throw ("converting to network fromat failed");
}
void communicate::send(void)
{
    int bytes_tx = sendto(this->resolver_socket, "msg", strlen("msg"), MSG_CONFIRM, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bytes_tx < 0)
        throw ("sending query failed");

}
void communicate::recv(void)
{
    socklen_t len;
    char srv_response[UDP_LIMIT];
    int bytes_rx = recvfrom(resolver_socket, (char *)srv_response, UDP_LIMIT, MSG_WAITALL, (struct sockaddr *)&server_addr, &len);

    if (bytes_rx < 0)
        throw ("recv of response failed");
}
void communicate::end(void)
{
    close(resolver_socket);
}