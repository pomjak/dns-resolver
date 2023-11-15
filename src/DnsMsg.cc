/**
 * @file DnsMsg.cc
 * @author Pomsar Jakub (xpomsa00@stud.fit.vutbr.cz)
 * @brief Implementation of functions needed for constructing and deconstructing DNS messages for the server.
 *
 * @version 0.1
 * @date 2023-11-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "DnsMsg.h"
#include "Maps.h"

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
 * @param param paramParser object containing parameters for constructing the DNS message.
 */
void DnsMessage::constructMsg(paramParser *param)
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

/**
 * @brief prints a DNS domain name from the response buffer.
 *
 * @param response vector containing the DNS response.
 * @param offset pointer to the offset within the response buffer.
 */
void DnsMessage::printName(const std::vector<uint8_t> &response, uint16_t *offset)
{
    uint8_t i;
    while (response[(*offset)])
    {
        if ((response[*offset] & 0xC0) == 0xC0) // msg compression detected
        {
            // concatenating 2 x uint8_t to uint16_t
            uint16_t pos = static_cast<uint16_t>(response[(*offset)]) |
                           (static_cast<uint16_t>(response[(*offset) + 1]) << 8);

            pos = ntohs(pos & 0xFF3F); // getting rid of compress flag
            *offset += 2;
            printName(response, &pos);
            return;
        }
        else // print name directly
        {
            uint8_t labelLength = response[*offset];
            for (i = 1; i <= labelLength; i++)
                std::cout << response[(*offset) + i];
            std::cout << '.';
            *offset += i;
        }
    }
    (*offset)++;
}

/**
 * @brief prints an IPv4 or IPv6 address from the response buffer.
 *
 * @param response  vector containing the DNS response.
 * @param offset pointer to the offset within the response buffer.
 * @param type type of the address (4 for IPv4, 6 for IPv6(pretty self-explanatory)).
 */
void DnsMessage::printAddress(const std::vector<uint8_t> &response, uint16_t *offset, uint8_t type)
{
    uint16_t len = ntohs(answer.length);
    uint16_t i = 0;

    for (i = 0; i < len;)
    {
        if (type == 6)
        {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(response[(*offset) + i]);
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(response[(*offset) + i + 1]);
            if (i != (len - 2))
                std::cout << ":";
            i += 2;
        }
        else if (type == 4)
        {
            std::cout << static_cast<int>(response[(*offset) + i]);
            if (i != (len - 1))
                std::cout << ".";
            i += 1;
        }
    }

    *offset += i;
}

/**
 * @brief prints information related to a DNS SOA (Start of Authority) record from the response buffer.
 *
 * @param response  vector containing the DNS response.
 * @param offset  pointer to the offset within the response buffer.
 */
void DnsMessage::printSOA(const std::vector<uint8_t> &response, uint16_t *offset)
{
    printName(response, offset);
    std::cout << ", ";
    printName(response, offset);

    for (int i = 0; i < 5; i++)
    {
        std::cout << ", " << ntohl(*reinterpret_cast<const uint32_t *>(&response[(*offset)]));
        (*offset) += sizeof(uint32_t);
    }
}

/**
 * @brief prints information related to the DNS message header from the response buffer.
 *
 * @param response  vector containing the DNS response.
 * @param offset pointer to the offset within the response buffer.
 */
void DnsMessage::printHeader(const std::vector<uint8_t> &response, uint16_t *offset)
{
    uint16_t originalId = header.id;

    // reset header and copy from response
    memset(&header, 0, sizeof(Header));
    memcpy(&header, response.data(), sizeof(Header));

    (*offset) += sizeof(Header);

    // check if the ID matches
    if (originalId != header.id)
    {
        throw std::runtime_error("Header ID does not match");
        return;
    }
    // check if the msg is response
    if (header.qr != 1)
    {
        throw std::runtime_error("Message is a query, not a response");
        return;
    }

    std::cout << "Authoritative: " << (header.aa ? "Yes, " : "No, ");
    std::cout << "Recursive: " << ((header.ra && header.rd) ? "Yes, " : "No, ");
    std::cout << "Truncated: " << (header.tc ? "Yes" : "No");

    // print ERROR and remapped rcode if rcode is received, but still continue in printing output
    if (header.rcode)
        std::cout << ", ERROR: " << remapRcode(header.rcode);
}

/**
 * @brief prints information related to the DNS message's question section from the response buffer.
 *
 * @param response  vector containing the DNS response.
 * @param offset pointer to the offset within the response buffer.
 */
void DnsMessage::printQuestion(const std::vector<uint8_t> &response, uint16_t *offset)
{
    std::cout << "Question section (" << ntohs(header.qCount) << ")" << std::endl;

    for (uint8_t i = 0; i < ntohs(header.qCount); i++)
    {
        std::cout << "  ";
        printName(response, offset);

        // reset question and copy from response
        memset(&question, 0, sizeof(Question));
        memcpy(&question, response.data() + (*offset), sizeof(Question));
        (*offset) += sizeof(Question);

        std::cout << ", " << remapQType(ntohs(question.qType)) << ", "
                  << remapQClass(ntohs(question.qClass)) << std::endl;
    }
}

/**
 * @brief prints information related to DNS resource records from the response buffer.
 *
 * @param response  vector containing the DNS response.
 * @param offset pointer to the offset within the response buffer.
 * @param cnt count of resource records to print.
 */
void DnsMessage::printRR(const std::vector<uint8_t> &response, uint16_t *offset, uint16_t cnt)
{
    for (uint8_t i = 0; i < ntohs(cnt); i++)
    {
        std::cout << "  ";
        printName(response, offset);

        // reset answer and copy from response
        memset(&answer, 0, sizeof(ResourceRecord));
        memcpy(&answer, response.data() + (*offset), sizeof(ResourceRecord));
        (*offset) += sizeof(ResourceRecord);

        std::cout << ", " << remapQType(ntohs(answer.type))
                  << ", " << remapQClass(ntohs(answer.rClass))
                  << ", " << std::dec << static_cast<uint32_t>(ntohl(answer.ttl)) << ", ";

        if (auto type = remapQType(ntohs(answer.type)); type == "A" || type == "AAAA")
            printAddress(response, offset, (type == "A") ? 4 : 6);

        else if (type == "CNAME" || type == "PTR" || type == "NS")
            printName(response, offset);

        else if (type == "SOA")
            printSOA(response, offset);

        std::cout << std::endl;
    }
}

/**
 * @brief used propagating error code from server to user
 *
 * @return unsigned int
 */
unsigned int DnsMessage::propagateError()
{
    return header.rcode;
}

/**
 * @brief prints information related to a DNS message from the response buffer.
 *
 * @param response The vector containing the DNS response.
 */
void DnsMessage::printMsg(const std::vector<uint8_t> &response)
{
    uint16_t offset = 0;

    // Print header
    printHeader(response, &offset);
    std::cout << std::endl;

    // Print question section
    printQuestion(response, &offset);

    // Print answer section
    std::cout << "Answer section (" << ntohs(header.ansCount) << ")" << std::endl;
    printRR(response, &offset, header.ansCount);

    // Print authority section
    std::cout << "Authority section (" << ntohs(header.authCount) << ")" << std::endl;
    printRR(response, &offset, header.authCount);

    // Print additional section
    std::cout << "Additional section (" << ntohs(header.addCount) << ")" << std::endl;
    printRR(response, &offset, header.addCount);
}
