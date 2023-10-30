#include "qmaps.h"

std::map<uint16_t, std::string> rcodeMap =
    {
        {0, "No error condition"},
        {1, "Format error"},
        {2, "Server failure"},
        {3, "Name Error [No such name]"},
        {4, "Not Implemented"},
        {5, "Refused"}};

std::map<uint16_t, std::string> qclassMap =
    {
        {1, "IN"},
        {2, "CS"},
        {3, "CH"},
        {4, "HS"}};

std::map<uint16_t, std::string> qtypeMap =
    {
        {1, "A"},
        {1, "NS"},
        {3, "MD"},
        {4, "MF"},
        {5, "CNAME"},
        {6, "SOA"},
        {11, "WKS"},
        {12, "PTR"},
        {13, "HINFO"},
        {14, "MINFO"},
        {15, "MX"},
        {16, "TXT"},
        {28, "AAAA"}};

std::string remapQClass(uint16_t qclass)
{
    auto it = qclassMap.find(qclass);
    if (it != qclassMap.end())
        return it->second;

    else
        return "Unknown";
}

std::string remapQType(uint16_t qclass)
{
    auto it = qtypeMap.find(qclass);
    if (it != qtypeMap.end())
        return it->second;

    else
        return "Unknown";
}

std::string remapRcode(uint16_t rcode)
{
    auto it = rcodeMap.find(rcode);
    if (it != rcodeMap.end())
        return it->second;

    else
        return "Unknown";
}