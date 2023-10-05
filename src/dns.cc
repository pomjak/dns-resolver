#include <iostream>
#include <exception>
#include "param_parser.h"
#include "socket_handler.h"

int main(int argc, char **argv)
{
    param_parser param;
    communicate sckt;

    try
    {
        param.process_params(argc, argv);
        sckt.start(&param); 
        sckt.send();
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
