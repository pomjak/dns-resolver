#include "dns_msg.h"
#include "qmaps.h"

/**
 * @brief generate random id of query
 *
 */
void DnsMessage::setHeaderId(void)
{
    std::random_device rd;

    std::mt19937 gen(rd());

    std::uniform_int_distribution<uint16_t> distribution(0, std::numeric_limits<uint16_t>::max());

    header.id = (unsigned short)htons(distribution(gen));
}

/**
 * @brief sets corresponding header recursion flag
 *
 */
void DnsMessage::setRecursion(bool recursion)
{
    if (recursion)
        header.rd = 1;
}

/**
 * @brief sets the DNS question class and type based on the given parameters.
 *
 * @param ipv6 indicating whether the query is for an IPv6 address.
 * @param inverse indicating whether to use the inverse query type (PTR).
 */
void DnsMessage::setClassAndType(bool ipv6, bool inverse)
{
    question.qClass = htons(1); // INternet class
    if (inverse)
        question.qType = htons(12); // PTR
    else
        question.qType = ipv6 ? htons(28) : htons(1); // AAAA if ipv6, A otherwise
}

/**
 * @brief encodes a direct address into the qname vector.
 *
 * @param addr
 */
void DnsMessage::directAddress(const std::string &addr)
{
    std::stringstream ss(addr);

    std::string label;

    while (std::getline(ss, label, '.')) // parsing string with delimeter .
    {
        qname.push_back(static_cast<uint8_t>(label.length())); // storing length of label before storing label

        for (char c : label)
            qname.push_back(static_cast<uint8_t>(c)); // storing label by one char
    }

    qname.push_back(0); // null byte indicating end of label
}

/**
 * @brief encodes a reverse address into the qname vector.
 *
 * @param addr IPv4 address to be encoded in reverse DNS format.
 */
void DnsMessage::reverseAddress(std::string addr)
{
    std::stringstream ssa(addr);

    std::string octet[4];
    std::string temp;

    // tokenize IPv4 address into octets
    for (int i = 0; std::getline(ssa, octet[i], '.'); i++)
        ;

    // traverse octets in reverse order
    for (int i = 3; i >= 0; i--)
    {
        qname.push_back(static_cast<uint8_t>(octet[i].length()));

        for (char c : octet[i])
            qname.push_back(static_cast<uint8_t>(c));
    }

    // append 'in-addr.arpa' suffix
    std::string suffix = "in-addr.arpa";
    std::stringstream sss(suffix);

    while (std::getline(sss, temp, '.'))
    {
        qname.push_back(static_cast<uint8_t>(temp.length()));

        for (char c : temp)
            qname.push_back(static_cast<uint8_t>(c));
    }

    // null byte indicating end of addr
    qname.push_back(0);
}

/**
 * @brief encodes a reverse IPv6 address into the qname vector.
 *
 * @param addrV6 IPv6 address traversed in reverse DNS format.
 */
void DnsMessage::reverseAddressV6(const std::string &addrV6)
{
    uint8_t NoQuartets = 38; // number of quartets + ':'

    for (int i = NoQuartets; i >= 0; i--)
    {
        if (addrV6[i] != ':')
        {
            qname.push_back(static_cast<uint8_t>(1));         // length is always 1
            qname.push_back(static_cast<uint8_t>(addrV6[i])); // storing addr per byte
        }
    }
    // append 'ip6.arpa' suffix
    std::string suffix = "ip6.arpa";
    std::stringstream sss(suffix);
    std::string temp;

    while (std::getline(sss, temp, '.'))
    {
        qname.push_back(static_cast<uint8_t>(temp.length()));

        for (char c : temp)
            qname.push_back(static_cast<uint8_t>(c));
    }

    qname.push_back(0);
}
/**
 * @brief constructs a DNS message based on the provided parameters.
 *
 * @param param param_parser object containing parameters for constructing the DNS message.
 */
void DnsMessage::constructMsg(param_parser *param)
{
    setHeaderId();

    if (param->getRecursion())
        setRecursion(true);

    setClassAndType(param->getIPv6(), param->getReverse());

    if (!(param->getReverse()))
        directAddress(param->getAddress()); // store normal DA
    else
    {
        if (param->getIPv6())
            reverseAddressV6(param->getAddress());
        else
            reverseAddress(param->getAddress()); // store reversed ip addr
    }

    header.qCount = htons(1);
}

/**
 * @brief prepares and returns a vector containing the DNS message for transmission.
 *
 * @return std::vector<uint8_t> vector containing the DNS message.
 */
std::vector<uint8_t> DnsMessage::handover() const
{
    std::vector<uint8_t> buf(sizeof(Header) + qname.size() + sizeof(Question));

    // copy the header into the buffer
    memcpy(buf.data(), &header, sizeof(Header));

    // append the qname to the buffer
    memcpy(buf.data() + sizeof(Header), qname.data(), qname.size());

    // copy the question into the buffer
    memcpy(buf.data() + sizeof(Header) + qname.size(), &question, sizeof(Question));

    return buf;
}

void DnsMessage::printName(std::vector<uint8_t> response, uint16_t *offset)
{
    uint8_t i;
    while (response[(*offset)]) // end of name is 0x00
    {
        if ((response[*offset] & 0xC0) == 0xC0) // Message compression detected
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
            for (i = 1; i <= static_cast<uint8_t>(response[(*offset)]); i++) // number of chars is stored at response[offset]
                std::cout << response[(*offset) + i];
            std::cout << '.';
            *offset += i;
        }
    }
    (*offset)++;
}

void DnsMessage::printAddress(std::vector<uint8_t> response, uint16_t *offset, uint8_t type)
{
    uint16_t len = static_cast<uint16_t>(ntohs(answer.length));
    uint16_t i = 0;

    if (type == 6)
    {
        for (i = 0; i < len; i += 2)
        {
            std::cout << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<int>(response[(*offset) + i]);
            std::cout << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<int>(response[(*offset) + i + 1]);
            if (i != (len - 2))
                std::cout << ":";
        }
    }

    if (type == 4)
    {
        for (i = 0; i < len; i++)
        {
            std::cout << static_cast<int>(response[(*offset) + i]);
            if (i != (len - 1))
                std::cout << ".";
        }
    }

    *offset += i;
}

void DnsMessage::printSOA(std::vector<uint8_t> response, uint16_t *offset)
{
    printName(response, offset);
    std::cout << ", ";
    printName(response, offset);

    uint32_t soaData;

    for (int i = 0; i < 5; i++)
    {
        std::cout << ", ";
        memcpy(&soaData, response.data() + (*offset), sizeof(soaData));
        (*offset) += sizeof(soaData);
        std::cout << ntohl(soaData);
    }
}

void DnsMessage::printHeader(std::vector<uint8_t> response, uint16_t *offset)
{
    uint16_t id = header.id;
    memset(&header, 0, sizeof(Header));
    memcpy(&header, response.data(), sizeof(Header));

    (*offset) += sizeof(Header);
    if (id != header.id)
    {
        std::cout << "Header ID does not match" << std::endl;
        return;
    }

    std::cout << "Authoritative: " << (header.aa ? "Yes, " : "No, ");
    std::cout << "Recursive: " << ((header.ra && header.rd) ? "Yes, " : "No, ");
    std::cout << "Truncated: " << (header.tc ? "Yes" : "No");
    if (header.rcode)
        std::cout << ", ERROR: " << remapRcode(header.rcode);
}

void DnsMessage::printQuestion(std::vector<uint8_t> response, uint16_t *offset)
{
    std::cout << "Question section (" << ntohs(header.qCount) << ")" << std::endl;
    for (uint8_t i = 0; i < ntohs(header.qCount); i++)
    {
        std::cout << "  ";
        printName(response, offset);

        memset(&question, 0, sizeof(Question));
        memcpy(&question, response.data() + (*offset), sizeof(Question));
        (*offset) += sizeof(Question);

        std::cout << ", " << remapQType(ntohs(question.qType)) << ", "
                  << remapQClass(ntohs(question.qClass)) << std::endl;
    }
}

void DnsMessage::printRR(std::vector<uint8_t> response, uint16_t *offset, uint16_t cnt)
{

    for (uint8_t i = 0; i < ntohs(cnt); i++)
    {
        std::cout << "  ";
        printName(response, offset);

        memset(&answer, 0, sizeof(ResourceRecord));
        memcpy(&answer, response.data() + (*offset), sizeof(ResourceRecord));
        (*offset) += sizeof(ResourceRecord);

        std::cout << ", " << remapQType(ntohs(answer.type));
        std::cout << ", " << remapQClass(ntohs(answer.rClass));
        std::cout << ", " << std::dec << static_cast<uint32_t>(ntohl(answer.ttl)) << ", ";

        if (remapQType(ntohs(answer.type)) == "A")
            printAddress(response, offset, 4);
        if (remapQType(ntohs(answer.type)) == "AAAA")
            printAddress(response, offset, 6);
        if (remapQType(ntohs(answer.type)) == "CNAME")
            printName(response, offset);
        if (remapQType(ntohs(answer.type)) == "PTR")
            printName(response, offset);
        if ((remapQType(ntohs(answer.type)) == "SOA"))
            printSOA(response, offset);
        if (remapQType(ntohs(answer.type)) == "NS")
            printName(response, offset);

        std::cout
            << std::endl;
    }
}

void DnsMessage::printMsg(std::vector<uint8_t> response)
{
    uint16_t offset = 0;

    printHeader(response, &offset);
    std::cout << std::endl;

    printQuestion(response, &offset);
    std::cout
        << "Answer section (" << ntohs(header.ansCount) << ")" << std::endl;
    printRR(response, &offset, header.ansCount);

    std::cout
        << "Authority section(" << ntohs(header.authCount) << ")" << std::endl;
    printRR(response, &offset, header.authCount);
    std::cout
        << "Additional section(" << ntohs(header.addCount) << ")" << std::endl;
    printRR(response, &offset, header.addCount);
}