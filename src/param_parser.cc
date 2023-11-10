#include "param_parser.h"

void param_parser::processParams(int argc, char **argv)
{
    int opt;

    if (argc < 2)
    {
        std::cerr << "USAGE: dns [-r] [-x] [-6] -s server [-p port] address" << std::endl;
        throw std::invalid_argument("bad number of args");
    }

    while ((opt = getopt(argc, argv, "rx6s:p:")) != -1)
    {
        switch (opt)
        {
        case 'r':
            this->setRecursion(true);
            break;

        case 'x':
            this->setReverse(true);
            break;

        case '6':
            this->setIPv6(true);
            break;

        case 's':
            this->setServer(optarg);
            break;

        case 'p':
            this->setPort(std::stoi(optarg));
            break;

        default:
            std::cerr << "USAGE: dns [-r] [-x] [-6] -s server [-p port] address" << std::endl;
            break;
        }
    }

    if (optind < argc)
        this->setAddress(argv[optind]);
    else
        throw std::invalid_argument("Address must be set");

    if (getServer() == "")
        throw std::invalid_argument("Server address must be set");
}