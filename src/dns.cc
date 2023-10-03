#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "param_parser.h"

#define UDP_LIMIT 255

int main(int argc, char **argv)
{
    param_parser param;

    param.process_params(argc, argv);
    int resolver_socket;

    if ((resolver_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        std::cerr << "socket fail";

    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(param.get_port());

    if (inet_pton(AF_INET, param.get_server().c_str(), &server_addr.sin_addr) <= 0)
        std::cerr << "socket fail";

    int bytes_tx = sendto(resolver_socket, "msg", strlen("msg"), MSG_CONFIRM, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bytes_tx < 0)
        std::cerr << "send to FAILED";

    socklen_t len;
    char srv_response[UDP_LIMIT];

    int bytes_rx = recvfrom(resolver_socket, (char *)srv_response, UDP_LIMIT, MSG_WAITALL, (struct sockaddr *)&server_addr, &len);

    if (bytes_rx < 0)
        std::cerr << "recv to FAILED";

    close(resolver_socket);

    return EXIT_SUCCESS;
}
