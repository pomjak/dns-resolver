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
    
    void setRecursion(bool r) { this->recursion = r; }

    void setReverse(bool x) { this->reverse = x; }

    void setIPv6(bool ip) { this->ipv6 = ip; }

    void setServer(std::string s) { this->server = s; }

    void setPort(int p) { this->port = p; }

    void setAddress(std::string a) { this->address = a; }

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

    void processParams(int argc, char **argv);

    bool getRecursion(void) { return this->recursion; }

    bool getReverse(void) { return this->reverse; }

    bool getIPv6(void) { return this->ipv6; }

    std::string getServer(void) { return this->server; }

    int getPort(void) { return this->port; }

    std::string getAddress(void) { return this->address; }
};
