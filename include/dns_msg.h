#pragma once

#include <random>
#include <vector>
#include <arpa/inet.h>
#include <sstream>
#include <iomanip>
#include "param_parser.h"

// HEADER
//  0-15b    ID      [SET]   RAND
//  16b      QR      [SET]   0 QUERY; 1 RESPONSE
//  17-20b   OPCODE  [SET]   0 STANDARD; 1 INVERSE
//  21b      AA      [READ]  AUTH ANS
//  22b      TC      [READ]  TRUNCATED
//  23b      RD      [SET]   1 RECURS DESIRED; 0 NOT RECURS
//  24b      RA      [READ]  RECURS AVAILABLE
//  25-27    Z       [SET]   000
//  28-31    RCODE   [READ]  ERR CODE

struct Header
{
    uint16_t id;
    uint16_t rd : 1;
    uint16_t tc : 1;
    uint16_t aa : 1;
    uint16_t opcode : 4;
    uint16_t qr : 1;
    uint16_t rcode : 4;
    uint16_t z : 3;
    uint16_t ra : 1;
    uint16_t qCount;
    uint16_t ansCount;
    uint16_t authCount;
    uint16_t addCount;
};

struct Question
{
    uint16_t qType;
    uint16_t qClass;
};
#pragma pack(1) // getting rid of unused padding
struct ResourceRecord
{
    uint16_t type;
    uint16_t rClass;
    uint32_t ttl;
    uint16_t length;
};
#pragma pack()

class DnsMessage
{
private:
    Header header{};
    std::vector<uint8_t> qname;
    Question question{};
    ResourceRecord answer{};

    void setHeaderId();
    void setRecursion(bool enable);
    void setClassAndType(bool ipv6, bool inverse);
    void directAddress(const std::string &address);
    void reverseAddress(std::string address);
    void reverseAddressV6(const std::string &addrV6);
    void printName(const std::vector<uint8_t> &response, uint16_t *offset);
    void printSOA(const std::vector<uint8_t> &response, uint16_t *offset);
    void printAddress(const std::vector<uint8_t> &response, uint16_t *offset, uint8_t type);
    void printHeader(const std::vector<uint8_t> &response, uint16_t *offset);
    void printQuestion(const std::vector<uint8_t> &response, uint16_t *offset);
    void printRR(const std::vector<uint8_t> &response, uint16_t *offset, uint16_t cnt);

public:
    DnsMessage() = default;

    void constructMsg(param_parser *param);

    std::vector<uint8_t> handover() const;

    void printMsg(const std::vector<uint8_t> &response);
};