/**
 * @file Maps.h
 * @author Pomsar Jakub (xpomsa00@stud.fit.vutbr.cz)
 * @brief extern declarations of maps used for remapping class, type and err code of dns msg
 * @version 0.1
 * @date 2023-11-14
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <iostream>
#include <cstdint>
#include <map>
#include <optional>
#include <string>

extern std::map<uint16_t, std::string> rcodeMap;
extern std::map<uint16_t, std::string> qclassMap;
extern std::map<uint16_t, std::string> qtypeMap;

extern std::string remapRcode(uint16_t rcode);
extern std::string remapQClass(uint16_t qclass);
extern std::string remapQType(uint16_t qtype);
