#include "param_parser.h"

void param_parser::process_params(int argc, char **argv)
{
    int opt;

    if (argc < 2)
        std::cerr << "USAGE: dns [-r] [-x] [-6] -s server [-p port] address" << std::endl;

    while ((opt = getopt(argc, argv, "rx6s:p:")) != -1)
    {
        switch (opt)
        {
        case 'r':
            this->set_recursion(true);
            break;

        case 'x':
            this->set_reverse(true);
            break;

        case '6':
            this->set_ipv6(true);
            break;

        case 's':
            this->set_server(optarg);
            break;

        case 'p':
            this->set_port(std::stoi(optarg));
            break;

        default:
            std::cerr << "USAGE: dns [-r] [-x] [-6] -s server [-p port] address" << std::endl;
            break;
        }
    }

    if (optind < argc)
        this->set_address(argv[optind]);
    else
        throw std::invalid_argument("Address must be set");

    if(get_server() == "")
        throw std::invalid_argument("Server address must be set");
}