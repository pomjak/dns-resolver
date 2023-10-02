#include <iostream>
#include <cstring>
#include <unistd.h>
#include <getopt.h>

void get_args(int argc, char **argv)
{
    int opt;
    std::string server;
    std::string port;
    std::string address;
    while ((opt = getopt(argc, argv, "rx6s:p:")) != -1)
    {
        switch(opt)
        {
          case 'r':
            std::cout << "reverse" << std::endl;
          break;

          case 'x':
            std::cout << "turncated" << std::endl;
          break;

          case '6':
            std::cout << "ip6" << std::endl;
          break;

          case 's':
            server = optarg;
            std::cout << "server: " << server << std::endl;
          break;

          case 'p':
            port = optarg;
            std::cout << "port" << port <<std::endl;
          break;

          default:
            std::cout << "usage" << std::endl;
          break;
        }
    }
    if (optind < argc) {
        address = argv[optind];
        std::cout << address << std::endl;
    } else {
        std::cerr << "Address is a required parameter." << std::endl;
    }
}

int main(int argc, char **argv)
{
    get_args(argc,argv);
    return 0;
}
