#include "dns_msg.h"

void DnsMessage::set_header_id(void)
{
    std::random_device rd;

    std::mt19937 gen(rd());

    std::uniform_int_distribution<uint16_t> distribution(0, std::numeric_limits<uint16_t>::max());

    header.id = (unsigned short)htons(distribution(gen));

    std::cout << std::hex << header.id << std::endl;
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

void DnsMessage::set_class_type(bool ipv6)
{
    if (!ipv6)
        question.qtype = htons(1);
    else
        question.qtype = htons(28);

    question.qclass = htons(1); // INternet class
}

void DnsMessage::convert_address(std::string addr)
{

    std::stringstream ss(addr);
    std::string label;

    while (std::getline(ss, label, '.'))
    {
        question.qname.push_back(static_cast<uint8_t>(label.length()));

        for (char c : label)
            question.qname.push_back(static_cast<uint8_t>(c));
    }

    question.qname.push_back(0);
}

void DnsMessage::construct_msg(param_parser *param)
{
    this->set_header_id();

    if (param->get_recursion())
        this->set_recursion(true);

    if (param->get_reverse())
        this->set_opcode(true);

    this->set_class_type(param->get_ipv6());

    this->convert_address(param->get_address());

    header.q_count = htons(1);
}

std::vector<uint8_t> DnsMessage::handover(void)
{
    std::vector<uint8_t> buf;
    buf.resize(sizeof(Header));
    memcpy(buf.data(), &header, sizeof(header));

    std::vector<uint8_t> buf_q;
    buf_q.resize(sizeof(Question));
    memcpy(buf_q.data(), &question, sizeof(question));

    buf.insert(buf.end(), buf_q.begin(), buf_q.end());

    return buf;
}