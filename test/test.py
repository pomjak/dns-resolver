import subprocess

RED = '\033[0;31m'
GREEN = '\033[0;32m'
YELLOW = '\033[0;33m'
RESET = '\033[0m'

def run_command(cmd):   
    process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    return stdout, stderr

def cut_out_answer(data):
    capture = False
    result_lines = []

    for line in data.split('\n'):
        if capture:
            result_lines.append(line.strip())
        
        if "Answer section " in line:
            capture = True
            
        if "Authority section " in line:
            capture = False
            result_lines[-1] = ""
    
    return '\n'.join(result_lines[:-1])

def main():
    with open('domains', 'r') as domain_file: 
        for domain in domain_file:\
            # domain = "pinterest.com"
            domain = domain.strip()

            dig_output, dig_stderr = run_command("dig @dns.google +noall +answer " + domain)
            dns_output,dns_stderr = run_command("./test -s dns.google " + domain)
            
            dig_output=dig_output[:-1].decode("utf-8")
            dns_output=cut_out_answer(dns_output.decode("utf-8"))
            dig = []
            dns = []

            for dig_line,dns_line in zip(dig_output.split('\n'),dns_output.split('\n')):
                try:
                    name, ttl, rclass, rtype, data = dig_line.split()
                    name = name.strip()
                    rclass = rclass.strip()
                    rtype = rtype.strip()
                    data = data.strip()

                    new_record = {'name': name, 'class': rclass, 'type' : rtype, 'ttl' : ttl,'data' : data}
                    dig.append(new_record)
                except ValueError:
                    print(f"Error parsing dig line: {dig_line}")
                    exit(1)
                try:
                    name, rtype, rclass, ttl, data = dns_line.split(",")
                    name = name.strip()
                    rtype = rtype.strip()
                    rclass = rclass.strip()
                    ttl = ttl.strip()
                    data = data.strip()

                    new_record = {'name': name, 'class': rclass, 'type' : rtype, 'ttl' : ttl,'data' : data}
                    dns.append(new_record)
                except ValueError:
                    print(f"Error parsing dns line: {dns_line}")
                    exit(1)
                
                
            dig.sort(key=lambda x: x['data'])
            dns.sort(key=lambda x: x['data'])
            # print(dig)
            # print(dns)
            
            for dig_record,dns_record in zip(dig,dns):
                
                if dig_record['name'] != dns_record['name']:
                    print(dig_record['name'])
                    print(dns_record['name'])
                    print(f"Test {RED}FAILED{RESET} [name of {domain}]")
                    print(f"Expected \"{dig_record['name']}\", got \"{dns_record['name']}\"")
                    exit(1)
                elif dig_record['class'] != dns_record['class']:
                    print(dig_record)
                    print(dns_record)
                    print(f"Test {RED}FAILED{RESET} [class of {domain}]")
                    print(f"Expected \"{dig_record['class']}\", got \"{dns_record['class']}\"")
                    exit(1)
                elif dig_record['type'] != dns_record['type']:
                    print(dig_record)
                    print(dns_record)
                    print(f"Test {RED}FAILED{RESET} [type of {domain}]")
                    print(f"Expected \"{dig_record['type']}\", got \"{dns_record['type']}\"")
                    exit(1)
                elif dig_record['data'] != dns_record['data']:
                    print(dig_record)
                    print(dns_record)
                    print(f"Test {RED}FAILED{RESET} [addr of {domain}]")
                    print(f"Expected \"{dig_record['data']}\", got \"{dns_record['data']}\"")
                    exit(1)
                elif dig_record['ttl'] != dns_record['ttl']:
                    print(dig_record)
                    print(dns_record)
                    print(f"Test {RED}FAILED{RESET} [addr of {domain}]")
                    print(f"Expected \"{dig_record['ttl']}\", got \"{dns_record['ttl']}\"")
                    exit(1)
                else:
                    print(f"Test {GREEN}SUCCEEDED{RESET} [ {domain} ]")

if __name__ == "__main__":
    main()
