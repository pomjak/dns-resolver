#ifndef PARAM_PARSER
#define PARAM_PARSER

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <getopt.h>

class param_parser
{
private:
    struct flags{
        bool recursion;
        bool reverse;
        bool ipv6;
        std::string server;
        int port;
        std::string address;
    };

    flags flag;
    
    void set_recursion(bool r) { this->flag.recursion = r; }

    void set_reverse(bool x) { this->flag.reverse = x; }

    void set_ipv6(bool ip) { this->flag.ipv6 = ip; }

    void set_server(std::string s) { this->flag.server = s; }

    void set_port(int p) { this->flag.port = p; }

    void set_address(std::string a) { this->flag.address = a; }

public:
    param_parser()
    {
        this->flag.recursion = false;
        this->flag.reverse = false;
        this->flag.ipv6 = false;
        this->flag.server = "";
        this->flag.port = -1;
        this->flag.address = "";
    }

    void process_params(int argc, char **argv);

    bool get_recursion(void) { return this->flag.recursion; }

    bool get_reverse(void) { return this->flag.reverse; }

    bool get_ipv6(void) { return this->flag.ipv6; }

    std::string get_server(void) { return this->flag.server; }

    int get_port(void) { return this->flag.port; }

    std::string get_address(void) { return this->flag.address; }
};

#endif