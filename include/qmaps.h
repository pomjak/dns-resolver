#pragma once

#include <iostream>
#include <cstdint>
#include <map>
#include <string>

extern std::map<uint16_t, std::string> rcodeMap;
extern std::map<uint16_t, std::string> qtypeMap;
extern std::map<uint16_t, std::string> qtypeMap;

extern std::string remapRcode(uint16_t rcode);
extern std::string remapQClass(uint16_t qclass);
extern std::string remapQType(uint16_t qtype);
