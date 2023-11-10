#pragma once

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <getopt.h>

class paramParser
{
private:
    bool recursion;
    bool reverse;
    bool ipv6;
    std::string server;
    int port;
    std::string address;

    void setRecursion(bool r) { recursion = r; }

    void setReverse(bool x) { reverse = x; }

    void setIPv6(bool ip) { ipv6 = ip; }

    void setServer(std::string s) { server = s; }

    void setPort(int p) { port = p; }

    void setAddress(std::string a) { address = a; }

public:
    paramParser() : recursion(false), reverse(false), ipv6(false), port(53) {}

    void processParams(int argc, char **argv);

    bool getRecursion(void) const { return recursion; }

    bool getReverse(void) const { return reverse; }

    bool getIPv6(void) const { return ipv6; }

    std::string getServer(void) const { return server; }

    int getPort(void) const { return port; }

    std::string getAddress(void) const { return address; }
};
