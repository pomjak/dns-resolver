#include"socket_handler.h"

void communicate::start(int port)
{
    if ((this->resolver_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        throw ("creating new socket failed");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET,"server", &server_addr.sin_addr) <= 0)
        throw ("converting to network fromat failed");
}
void send(void){}
void recv(void){}
void end(void){}