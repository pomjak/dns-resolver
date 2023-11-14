/**
 * @file Dns.cc
 * @author Pomsar Jakub (xpomsa00@stud.fit.vutbr.cz)
 * @brief DNS client main program
 * @version 0.1
 * @date 2023-11-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <iostream>
#include <exception>
#include "paramParser.h"
#include "socketHandler.h"
#include "DnsMsg.h"

int main(int argc, char **argv)
{
    paramParser param;
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
        return (EXIT_FAILURE);
    }

    catch (const std::exception &e)
    {
        std::cerr << "Generic Exception: " << e.what() << std::endl;
        return (EXIT_FAILURE);
    }

    catch (...)
    {
        std::cerr << "Generic Exception" << std::endl;

        return (EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
