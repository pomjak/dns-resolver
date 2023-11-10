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
    param_parser() : recursion(false), reverse(false), ipv6(false), port(53) {}

    void processParams(int argc, char **argv);

    bool getRecursion(void) const { return this->recursion; }

    bool getReverse(void) const { return this->reverse; }

    bool getIPv6(void) const { return this->ipv6; }

    std::string getServer(void) const { return this->server; }

    int getPort(void) const { return this->port; }

    std::string getAddress(void) const { return this->address; }
};
