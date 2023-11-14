/**
 * @file Maps.cc
 * @author Pomsar Jakub (xpomsa00@stud.fit.vutbr.cz)
 * @brief Definition of maps and functions for remapping DNS class, type, and response code
 * @version 0.1
 * @date 2023-11-14
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "Maps.h"

std::map<uint16_t, std::string> rcodeMap =
    {
        {0, "No error condition "},
        {1, "Format error "},
        {2, "Server failure "},
        {3, "Name Error [No such name] "},
        {4, "Not Implemented "},
        {5, "Refused "}};

std::map<uint16_t, std::string> qclassMap =
    {
        {1, "IN"},
        {2, "CS"},
        {3, "CH"},
        {4, "HS"}};

std::map<uint16_t, std::string> qtypeMap =
    {
        {1, "A"},
        {2, "NS"},
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
    try
    {
        return qclassMap.at(qclass);
    }
    catch (const std::out_of_range &)
    {
        return "Unknown";
    }
}

std::string remapQType(uint16_t qtype)
{
    try
    {
        return qtypeMap.at(qtype);
    }
    catch (const std::out_of_range &)
    {
        return "Unknown";
    }
}

std::string remapRcode(uint16_t rcode)
{
    try
    {
        return rcodeMap.at(rcode);
    }
    catch (const std::out_of_range &)
    {
        return "Unknown";
    }
}
