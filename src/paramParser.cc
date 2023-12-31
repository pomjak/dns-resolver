/**
 * @file paramParser.cc
 * @author Pomsar Jakub (xpomsa00@stud.fit.vutbr.cz)
 * @brief definition of functions  of class paramParser
 * @version 0.1
 * @date 2023-11-14
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "paramParser.h"

enum class Option
{
    Recursion = 'r',
    Reverse = 'x',
    IPv6 = '6',
    Server = 's',
    Port = 'p',
};

void paramParser::processParams(int argc, char **argv)
{
    int opt;

    if (argc < 2)
    {
        std::cerr << "USAGE: dns [-r] [-x] [-6] -s server [-p port] address" << std::endl;
        throw std::invalid_argument("bad number of args");
    }

    while ((opt = getopt(argc, argv, "rx6s:p:")) != -1)
    {
        switch (static_cast<Option>(opt))
        {
        case Option::Recursion:
            setRecursion(true);
            break;

        case Option::Reverse:
            setReverse(true);
            break;

        case Option::IPv6:
            setIPv6(true);
            break;

        case Option::Server:
            setServer(optarg);
            break;

        case Option::Port:
            setPort(std::stoi(optarg));
            break;

        default:
            std::cerr << "Unknown option: " << static_cast<char>(opt) << std::endl;
            std::cerr << "USAGE: dns [-r] [-x] [-6] -s server [-p port] address" << std::endl;
        }
    }

    if (optind < argc)
        setAddress(argv[optind]);
    else
        throw std::invalid_argument("Address must be set");

    if (getServer().empty())
        throw std::invalid_argument("Server address must be set");
}
