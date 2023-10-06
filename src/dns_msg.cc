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
    std::cout << "opcode settin" << std::endl;
    if (inverse)
    {
        std::cout << "inverse" << std::endl;

        header.opcode = 0x1;
    }
}

void DnsMessage::set_class_type(bool ipv6, bool inverse)
{
    if (inverse)
        question.qtype = htons(12); // PTR
    else if (ipv6)
        question.qtype = htons(28); // AAAA
    else
        question.qtype = htons(1); // A

    question.qclass = htons(1); // INternet class
}

void DnsMessage::direct_address(std::string addr)
{
    std::stringstream ss(addr);

    std::string label;

    while (std::getline(ss, label, '.'))
    {
        qname.push_back(static_cast<uint8_t>(label.length()));

        for (char c : label)
            qname.push_back(static_cast<uint8_t>(c));
    }

    qname.push_back(0);
}

void DnsMessage::reverse_address(std::string addr)
{
    std::string sufix = "in-addr.arpa";

    std::stringstream sa(addr);
    std::stringstream ss(sufix);

    std::stack<uint8_t> stack;
    std::string octet[4];
    std::string temp;

    for (int i = 0; std::getline(sa, octet[i], '.'); i++);

    for (int i = 3; i >= 0; i--)
    {
        qname.push_back(static_cast<uint8_t>(octet[i].length()));

        for (char c : octet[i])
            qname.push_back(static_cast<uint8_t>(c));
    }

    while (std::getline(ss, temp, '.'))
    {
        qname.push_back(static_cast<uint8_t>(temp.length()));

        for (char c : temp)
            qname.push_back(static_cast<uint8_t>(c));
    }

    qname.push_back(0);
}

void DnsMessage::construct_msg(param_parser *param)
{
    this->set_header_id();

    if (param->get_recursion())
        this->set_recursion(true);

    this->set_class_type(param->get_ipv6(), param->get_reverse());

    if (!(param->get_reverse()))
        this->direct_address(param->get_address());
    else
        this->reverse_address(param->get_address());

    header.q_count = htons(1);
}

std::vector<uint8_t> DnsMessage::handover(void)
{
    std::vector<uint8_t> buf;
    buf.resize(sizeof(Header));
    memcpy(buf.data(), &header, sizeof(Header));

    buf.insert(buf.end(), qname.begin(), qname.end());

    std::vector<uint8_t> buf_q;
    buf_q.resize(sizeof(Question));
    memcpy(buf_q.data(), &question, sizeof(Question));

    buf.insert(buf.end(), buf_q.begin(), buf_q.end());

    return buf;
}