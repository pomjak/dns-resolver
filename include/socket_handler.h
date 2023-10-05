#ifndef SOCKET_HANDLER
#define SOCKET_HANDLER

#include <arpa/inet.h>
#include <sys/socket.h>
#include"param_parser.h"


class communicate {
    private:
        int resolver_socket;
        struct sockaddr_in server_addr;

    public:
        communicate()
        {
            this->resolver_socket=-1;
        }
        void start(param_parser *param);
        void send(void);
        void recv(void);
        void end(void);


};

#endif