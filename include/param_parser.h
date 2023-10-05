#pragma once

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <getopt.h>

class param_parser
{
private:
        bool recursion;
        bool reverse;
        bool ipv6;
        std::string server;
        int port;
        std::string address;
    
    void set_recursion(bool r) { this->recursion = r; }

    void set_reverse(bool x) { this->reverse = x; }

    void set_ipv6(bool ip) { this->ipv6 = ip; }

    void set_server(std::string s) { this->server = s; }

    void set_port(int p) { this->port = p; }

    void set_address(std::string a) { this->address = a; }

public:
    param_parser()
    {
        this->recursion = false;
        this->reverse = false;
        this->ipv6 = false;
        this->server = "";
        this->port = 53;
        this->address = "";
    }

    void process_params(int argc, char **argv);

    bool get_recursion(void) { return this->recursion; }

    bool get_reverse(void) { return this->reverse; }

    bool get_ipv6(void) { return this->ipv6; }

    std::string get_server(void) { return this->server; }

    int get_port(void) { return this->port; }

    std::string get_address(void) { return this->address; }
};
