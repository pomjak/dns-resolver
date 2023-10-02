#include <iostream>
#include "param_parser.h"


int main(int argc, char **argv)
{
    param_parser param;
    
    param.process_params(argc,argv);

    std::cout << "add: " << param.get_address() << std::endl;
    std::cout << "ser: " << param.get_server() << std::endl;
    std::cout << "rec: " << param.get_recursion() << std::endl;
    std::cout << "rev: " << param.get_reverse() << std::endl;
    std::cout << "ip6: " << param.get_ipv6() << std::endl;

    return EXIT_SUCCESS;
}
