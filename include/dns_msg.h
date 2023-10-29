#pragma once

#include <random>
#include <vector>
#include <arpa/inet.h>
#include <sstream>
#include "param_parser.h"

// HEADER
//  0-15b    ID      [SET]   RAND
//  16b      QR      [SET]   0 QUERY; 1 RESPONSE
//  17-20b   OPCODE  [SET]   0 STANDARD; 1 INVERSE
//  21b      AA      [READ]  AUTH ANS
//  22b      TC      [READ]  TURNCATED
//  23b      RD      [SET]   1 RECURS DESIRED; 0 NOT RECURS
//  24b      RA      [READ]  RECURS AVAI
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
    uint16_t q_count;
    uint16_t ans_count;
    uint16_t auth_count;
    uint16_t add_count;
};

struct Question
{
    uint16_t qtype;
    uint16_t qclass;
};
#pragma pack(1) // getting rid of unused padding
struct ResourceRecord
{
    uint16_t type;
    uint16_t Rclass;
    uint32_t ttl;
    uint16_t length;
};
#pragma pack()
class DnsMessage
{

private:
    Header header;
    std::vector<uint8_t> qname;
    Question question;

    ResourceRecord answer;

    ResourceRecord authority;

    ResourceRecord additional;

    void setHeaderId(void);
    void setRecursion(bool _);
    void setClassAndType(bool ipv6, bool inverse);
    void directAddress(std::string _);
    void reverseAddress(std::string _);
    void printName(std::vector<uint8_t> response, uint16_t *pos);
    void printAddress(std::vector<uint8_t> response, uint16_t *offset, uint8_t type);
    void printHeader(std::vector<uint8_t> response, uint16_t *offset);
    void printQuestion(std::vector<uint8_t> response, uint16_t *offset);
    void printRR(std::vector<uint8_t> response, uint16_t *offset, uint16_t cnt);

public:
    DnsMessage()
    {
        memset(&header, 0, sizeof(struct Header));
        memset(&question, 0, sizeof(struct Question));
        memset(&answer, 0, sizeof(struct ResourceRecord));
        memset(&authority, 0, sizeof(struct ResourceRecord));
        memset(&additional, 0, sizeof(struct ResourceRecord));

        qname.clear();
    }

    void constructMsg(param_parser *param);

    std::vector<uint8_t> handover(void);

    void printMsg(std::vector<uint8_t> response);
};