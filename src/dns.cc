#include <iostream>
#include <exception>
#include "param_parser.h"
#include "socket_handler.h"
#include "dns_msg.h"

int main(int argc, char **argv)
{
    param_parser param;
    communicate socket;
    DnsMessage msg;

    try
    {
        param.processParams(argc, argv);

        socket.start(&param);

        msg.constructMsg(&param);

        socket.sendQuery(msg.handover());

        msg.printMsg(socket.recvResponse());

        socket.end();
    }

    catch (const std::invalid_argument &e)
    {
        std::cerr << "Invalid Argument Exception: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    catch (const std::exception &e)
    {
        std::cerr << "Generic Exception: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
