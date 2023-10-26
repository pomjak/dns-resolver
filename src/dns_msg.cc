#include "dns_msg.h"

void DnsMessage::setHeaderId(void)
{
    std::random_device rd;

    std::mt19937 gen(rd());

    std::uniform_int_distribution<uint16_t> distribution(0, std::numeric_limits<uint16_t>::max());

    header.id = (unsigned short)htons(distribution(gen));
}

void DnsMessage::setRecursion(bool recursion)
{
    if (recursion)
        header.rd = 1;
}

void DnsMessage::setClassAndType(bool ipv6, bool inverse)
{
    if (inverse)
        question.qtype = htons(12); // PTR
    else if (ipv6)
        question.qtype = htons(28); // AAAA
    else
        question.qtype = htons(1); // A

    question.qclass = htons(1); // INternet class
}

void DnsMessage::directAddress(std::string addr)
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

void DnsMessage::reverseAddress(std::string addr)
{
    std::stringstream ssa(addr);

    std::string octet[4];
    std::string temp;

    for (int i = 0; std::getline(ssa, octet[i], '.'); i++)
        ;

    for (int i = 3; i >= 0; i--)
    {
        qname.push_back(static_cast<uint8_t>(octet[i].length()));

        for (char c : octet[i])
            qname.push_back(static_cast<uint8_t>(c));
    }

    std::string sufix = "in-addr.arpa";
    std::stringstream sss(sufix);

    while (std::getline(sss, temp, '.'))
    {
        qname.push_back(static_cast<uint8_t>(temp.length()));

        for (char c : temp)
            qname.push_back(static_cast<uint8_t>(c));
    }

    qname.push_back(0);
}

void DnsMessage::constructMsg(param_parser *param)
{
    this->setHeaderId();

    if (param->get_recursion())
        this->setRecursion(true);

    this->setClassAndType(param->get_ipv6(), param->get_reverse());

    if (!(param->get_reverse()))
        this->directAddress(param->get_address());
    else
        this->reverseAddress(param->get_address());

    header.q_count = htons(1);
}

void DnsMessage::deconstructMsg(void)
{

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

void DnsMessage::recvMsg(std::vector<uint8_t> response)
{   
    //Header

    //qname
    //question
    
    //ans_name
    memcpy(&header, response.data(), sizeof(Header));

    memcpy(&question, response.data()+sizeof(Header)+qname.size(), sizeof(Question));

    



}