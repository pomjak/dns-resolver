import subprocess

RED = '\033[0;31m'
GREEN = '\033[0;32m'
YELLOW = '\033[0;33m'
RESET = '\033[0m'

commands = [
    {'desc':'A record','dig':'dig -r @8.8.8.8 +noall +answer A ', 'dns':'./test -r -s 8.8.8.8 '},
    {'desc':'AAAA record','dig':'dig -r @8.8.8.8 +noall +answer AAAA ', 'dns':'./test -r -s 8.8.8.8 -6 '},
    {'desc':'reverse A record','dig':'dig -r @8.8.8.8 +noall +answer -x ', 'dns':'./test -r -s 8.8.8.8 -x '},
    {'desc':'reverse AAAA record','dig':'dig -r @8.8.8.8 +noall +answer -x AAAA ', 'dns':'./test -r -s 8.8.8.8 -x -6 '},
    ]

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
        for domain in domain_file:
            # domain = "pinterest.com"
            domain = domain.strip()

            dig_output, dig_stderr = run_command(commands[1]['dig'] + domain)
            dns_output,dns_stderr = run_command(commands[1]['dns'] + domain)
            
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
                    print(f"Error parsing dig line: {dig_line} with {domain}")
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
                    print(f"Error parsing dns line: {dns_line} with {domain}")
                    exit(1)
                
                
            dig.sort(key=lambda x: x['data'])
            dns.sort(key=lambda x: x['data'])
            
            for dig_record,dns_record in zip(dig,dns):
                
                if dig_record['name'] != dns_record['name']:
                    print(f"Test {RED}FAILED{RESET} [name of {domain}]")
                    print(f"Expected \"{dig_record['name']}\", got \"{dns_record['name']}\"")
                    exit(1)

                elif dig_record['class'] != dns_record['class']:
                    print(f"Test {RED}FAILED{RESET} [class of {domain}]")
                    print(f"Expected \"{dig_record['class']}\", got \"{dns_record['class']}\"")
                    exit(1)

                elif dig_record['type'] != dns_record['type']:
                    print(f"Test {RED}FAILED{RESET} [type of {domain}]")
                    print(f"Expected \"{dig_record['type']}\", got \"{dns_record['type']}\"")
                    exit(1)

                elif dig_record['data'] != dns_record['data']:
                    print(f"Test {RED}FAILED{RESET} [addr of {domain}]")
                    print(f"Expected \"{dig_record['data']}\", got \"{dns_record['data']}\"")
                    exit(1)

                else:
                    print(f"Test {GREEN}OK{RESET} [ {domain} ]")

if __name__ == "__main__":
    main()
