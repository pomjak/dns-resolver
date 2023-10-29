#pragma once

#include <iostream>
#include <cstdint>
#include <map>
#include <string>

extern std::map<uint16_t, std::string> qclassMap;
extern std::map<uint16_t, std::string> qtypeMap;
extern std::string remapQClass(uint16_t qclass);
extern std::string remapQType(uint16_t qtype);
