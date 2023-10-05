#pragma once

#include <random>
#include <vector>
#include <arpa/inet.h>
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

class DnsMessage
{

private:
    Header header;
    Question question;

    void set_header_id(void);
    void set_recursion(bool _);
    void set_opcode(bool _);
    void set_address(std::string _);

public:
    DnsMessage()
    {
        memset(&header, 0, sizeof(struct Header));
        memset(&question, 0, sizeof(struct Question));
    }
    void construct_msg(param_parser *param);

    std::vector<uint8_t> handover(void);
};