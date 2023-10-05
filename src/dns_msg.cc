#include "dns_msg.h"

void DnsMessage::set_header_id(void)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<uint16_t> distribution(0, std::numeric_limits<uint16_t>::max());

    header.id = (unsigned short) htons(distribution(gen));

    std::cout << header.id << std::endl;
}

void DnsMessage::set_recursion(bool recursion)
{
    if (recursion)
        header.rd = 1;
}

void DnsMessage::set_opcode(bool inverse)
{
    if (inverse)
        header.opcode = htons(1);
}

void DnsMessage::construct_msg(param_parser *param)
{
    this->set_header_id();

    if (param->get_recursion())
        this->set_recursion(true);

    if (param->get_reverse())
        this->set_opcode(true);

    // if(param->get_port() != 53);

    // param->get_server();
    // param->get_address();
    
    header.q_count = htons(1);

}

std::vector<uint8_t> DnsMessage::handover(void)
{
    std::vector<uint8_t> buf(65536);
    std::memcpy(buf.data(), &header, sizeof(Header));
    return buf;
}