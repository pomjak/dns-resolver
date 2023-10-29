#include "dns_msg.h"
#include "qmaps.h"

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

    std::string suffix = "in-addr.arpa";
    std::stringstream sss(suffix);

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

void DnsMessage::printName(std::vector<uint8_t> response, uint16_t *offset)
{
    uint8_t i;
    while (response[(*offset)] != 0x00) // end of name is 0x00
    {
        if (response[(*offset)] == 0xc0) // Message compression detected
        {
            uint16_t pos = static_cast<uint16_t>(response[(*offset)]) |
                           (static_cast<uint16_t>(response[(*offset) + 1]) << 8); // concatenating 2xuint8_t to uint16_t

            *offset += 2;
            pos = ntohs(pos & 0xFF30); // getting rid of compress flag
            printName(response, &pos);
            return;
        }

        else
        {
            for (i = 1; i <= static_cast<uint8_t>(response[(*offset)]); i++)
                std::cout << response[(*offset) + i];
            std::cout << '.';
            *offset += i;
        }
    }
    (*offset)++;
}

void DnsMessage::printHeader(std::vector<uint8_t> response, uint16_t *offset)
{
    memset(&header, 0, sizeof(Header));
    memcpy(&header, response.data(), sizeof(Header));
    (*offset) += sizeof(Header);

    std::cout << "Authoritative: " << (header.aa ? "yes" : "no") << std::endl;
    std::cout << "Recursive: " << ((header.ra && header.rd) ? "yes" : "no") << std::endl;
    std::cout << "Truncated: " << (header.tc ? "yes" : "no") << std::endl;
    if (header.rcode)
        std::cout << "ERROR: " << remapRcode(header.rcode) << std::endl;
}

void DnsMessage::printQuestion(std::vector<uint8_t> response, uint16_t *offset)
{
    std::cout << "Question(" << ntohs(header.q_count) << ")" << std::endl;

    printName(response, offset);

    memset(&question, 0, sizeof(Question));
    memcpy(&question, response.data() + (*offset), sizeof(Question));
    (*offset) += sizeof(Question);

    std::cout << "," << remapQType(ntohs(question.qtype)) << "," << remapQClass(ntohs(question.qclass)) << std::endl;
}

void DnsMessage::printAnswer(std::vector<uint8_t> response, uint16_t *offset)
{
    std::cout << "Answer(" << ntohs(header.ans_count) << ")" << std::endl;

    printName(response, offset);

    memset(&answer, 0, sizeof(ResourceRecord));
    memcpy(&answer, response.data() + (*offset), sizeof(ResourceRecord));
    (*offset) += sizeof(ResourceRecord);

    std::cout << "," << remapQType(ntohs(answer.type));
    std::cout << "," << remapQClass(ntohs(answer.Rclass));
    std::cout << "," << ntohl(answer.ttl) << ",";

    if (remapQType(ntohs(answer.type)) == "CNAME")
        printName(response, offset);
    else
        std::cout << "addr";

    std::cout << std::endl;
}

void DnsMessage::printMsg(std::vector<uint8_t> response)
{
    uint16_t offset = 0;

    printHeader(response, &offset);
    std::cout << std::endl;

    printQuestion(response, &offset);
    printAnswer(response, &offset);

    std::cout
        << "Authority(" << ntohs(header.auth_count) << ")" << std::endl;
    std::cout
        << "Additional(" << ntohs(header.add_count) << ")" << std::endl;
}