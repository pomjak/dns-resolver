#include <iostream>
#include <exception>
#include "param_parser.h"
#include "socket_handler.h"
#include "dns_msg.h"

int main(int argc, char **argv)
{
    param_parser param;
    communicate sckt;
    DnsMessage msg;

    try
    {
        param.process_params(argc, argv);

        sckt.start(&param); 

        msg.constructMsg(&param);

        sckt.send(msg.handover());

        msg.recvMsg(sckt.recv());

        msg.deconstructMsg();
        
        sckt.end();
    }

    catch (const std::invalid_argument &e)
    {
        std::cerr << "Invalid Argument Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    catch (const std::exception &e)
    {
        std::cerr << "Generic Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
