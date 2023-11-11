import subprocess
import ipaddress
import traceback
import os

RED = '\033[0;31m'
GREEN = '\033[0;32m'
YELLOW = '\033[0;33m'
RESET = '\033[0m'

commands = [
    {'desc':'A record',             'file':'domains',   'dig':'dig @8.8.8.8 +noall +answer A ',          'dns':'./test -r -s 8.8.8.8 '       },
    {'desc':'AAAA record',          'file':'domains',   'dig':'dig @8.8.8.8 +noall +answer AAAA ',       'dns':'./test -r -s 8.8.8.8 -6 '    },
    {'desc':'reverse A record',     'file':'ips',       'dig':'dig @8.8.8.8 +noall +answer -x ',       'dns':'./test -r -s 8.8.8.8 -x '    },
    {'desc':'reverse AAAA record',  'file':'ips6',      'dig':'dig @8.8.8.8 +noall +answer -x ',    'dns':'./test -r -s 8.8.8.8 -x -6 ' },
    ]

def print_dict(listDict):
    for oneDict in listDict:
        print(oneDict)
        
def create_artifact(status,domain,dig,dns,test_case):
    file_path = f"../test-artifacts/{test_case['desc']}/{domain}"
    folder_path = f"../test-artifacts/{test_case['desc']}"
    
    os.makedirs(folder_path, exist_ok=True)
    
    with open(file_path, 'w') as file:
        if status == "PASS":
            file.write(f"TEST {test_case['desc']} for {domain} PASSED")
        else:
            file.write(f"TEST {test_case['desc']} for {domain} FAILED {status} not matching")
            
        file.write("\nDIG output:")
        for oneDig in dig:
            file.write(f"{oneDig}")
            
        file.write("\nDNS output:")    
        for oneDns in dig:
            file.write(f"{oneDns}")
            
        file.close()
                             

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
    
    for test_case in commands:
        
        with open(test_case['file'], 'r') as domain_file: 
            print("__________ " + test_case['desc'] + " Test__________")
            
            for domain in domain_file:
                domain = domain.strip()
                if test_case['desc'] == "reverse AAAA record":
                    domain = ipaddress.ip_address(domain).exploded
                    
                dig_output, dig_stderr = run_command(test_case['dig'] + domain)
                dns_output,dns_stderr = run_command(test_case['dns'] + domain)
                
                dig_output=dig_output[:-1].decode("utf-8")
                dns_output=cut_out_answer(dns_output.decode("utf-8"))
                
                dig = []
                dns = []

                for dig_line,dns_line in zip(dig_output.split('\n'),dns_output.split('\n')):
                    try:
                        if dig_line == "":
                            new_record = {'name': "", 'class': "", 'type' : "", 'ttl' : "",'data' : ""}
                            dig.append(new_record)
                        else:
                            name, ttl, rclass, rtype, data = dig_line.split()
                            name = name.strip()
                            rclass = rclass.strip()
                            rtype = rtype.strip()
                            data = data.strip()
                            if "AAAA record" == test_case['desc'] and rtype == 'AAAA': #exploding ip6 from dig
                                data = ipaddress.ip_address(data).exploded

                            new_record = {'name': name, 'class': rclass, 'type' : rtype, 'ttl' : ttl,'data' : data}
                            dig.append(new_record)
                        
                    except ValueError as e:
                        print(f"Error parsing DIG line: {dig_line} with {domain}")
                        print (f"{e}")
                        
                    try:
                        if dns_line == "":
                            new_record = {'name': "", 'class': "", 'type' : "", 'ttl' : "",'data' : ""}
                            dns.append(new_record)
                        else:
                            name, rtype, rclass, ttl, data = dns_line.split(",")
                            name = name.strip()
                            rtype = rtype.strip()
                            rclass = rclass.strip()
                            ttl = ttl.strip()
                            data = data.strip()

                            new_record = {'name': name, 'class': rclass, 'type' : rtype, 'ttl' : ttl,'data' : data}
                            dns.append(new_record)
                    except ValueError as e:
                        print(f"Error parsing DNS line: {dns_line} with {domain}")
                        print (f"{e}")
                    
                dig.sort(key=lambda x: x['data'])
                dns.sort(key=lambda x: x['data'])
                
                for dig_record,dns_record in zip(dig,dns):
                    
                    if dig_record['name'] != dns_record['name']:
                        print(f"Test {RED}FAILED{RESET} [name of {domain}]")
                        print(f"Expected \"{dig_record['name']}\", got \"{dns_record['name']}\"")
                        create_artifact("NAME",domain,dig,dns,test_case)
                        exit(1)

                    elif dig_record['class'] != dns_record['class']:
                        print(f"Test {RED}FAILED{RESET} [class of {domain}]")
                        print(f"Expected \"{dig_record['class']}\", got \"{dns_record['class']}\"")
                        create_artifact("CLASS",domain,dig,dns,test_case)
                        exit(1)

                    elif dig_record['type'] != dns_record['type']:
                        print(f"Test {RED}FAILED{RESET} [type of {domain}]")
                        print(f"Expected \"{dig_record['type']}\", got \"{dns_record['type']}\"")
                        create_artifact("TYPE",domain,dig,dns,test_case)
                        exit(1)

                    elif dig_record['data'] != dns_record['data']:
                        print(f"Test {RED}FAILED{RESET} [addr of {domain}]")
                        print(f"Expected \"{dig_record['data']}\", got \"{dns_record['data']}\"")
                        create_artifact("DATA",domain,dig,dns,test_case)
                        exit(1)

                    else:
                        create_artifact("PASS",domain,dig,dns,test_case)
                        print(f"Test {GREEN}OK{RESET} [ {domain} ]")

if __name__ == "__main__":
    main()
