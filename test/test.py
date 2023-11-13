import subprocess
import ipaddress
import traceback
import os

RED = '\033[0;31m'
GREEN = '\033[0;32m'
YELLOW = '\033[0;33m'
RESET = '\033[0m'

SERVER = "8.8.8.8"

args_cases = [
    {'desc':'without_args'                              , 'cmd':'./dns -s'                                  ,'exp':1},
    {'desc':'only_s_arg'                                , 'cmd':'./dns -s hello'                            ,'exp':1},
    {'desc':'without_address'                           , 'cmd':'./dns -s hello '                           ,'exp':1},
    {'desc':'bad_server'                                , 'cmd':'./dns -s hello world'                      ,'exp':1},
    {'desc':'without_address_but_server_ok'             , 'cmd':'./dns -s kazi.fit.vutbr.cz'                ,'exp':1},
    {'desc':'entered_correctly_but_extra_arguments'     , 'cmd':'./dns -s kazi.fit.vutbr.cz www.fit.cz -f'  ,'exp':0},
]

commands = [
    {'desc':'A-record',             'file':'domains',   'dig':f'dig @{SERVER} +noall +answer A ',    'dns':f'./dns -r -s {SERVER} '      },
    {'desc':'AAAA-record',          'file':'domains',   'dig':f'dig @{SERVER} +noall +answer AAAA ', 'dns':f'./dns -r -s {SERVER} -6 '   },
    {'desc':'reverse-A-record',     'file':'ips',       'dig':f'dig @{SERVER} +noall +answer -x ',   'dns':f'./dns -r -s {SERVER} -x '   },
    {'desc':'reverse-AAAA-record',  'file':'ips6',      'dig':f'dig @{SERVER} +noall +answer -x ',   'dns':f'./dns -r -s {SERVER} -x -6 '},
    {'desc':'Authority'          ,  'file':'domains',   'dig':f'dig @kazi.fit.vutbr.cz +noall +authority +norecurse ',   'dns':f'./dns -s kazi.fit.vutbr.cz '},
    {'desc':'Additional'          , 'file':'domains',   'dig':f'dig @kazi.fit.vutbr.cz +noall +additional +norecurse ',   'dns':f'./dns -s kazi.fit.vutbr.cz '},
    ]

def print_dict(listDict):
    for oneDict in listDict:
        print(oneDict)
     
def create_artifact_args(status,ret_code,test_case):
    file_path = f"../test-artifacts/args/{test_case['desc']}"
    folder_path = f"../test-artifacts/args"
    
    os.makedirs(folder_path, exist_ok=True)
    
    with open(file_path, 'w') as file:
        if status == "PASS":
            file.write(f"TEST {test_case['desc']}  PASSED\n")
        else:
            file.write(f"TEST {test_case['desc']} FAILED\n")
        
        file.write(f"\nExpected \"{test_case['exp']}\", got \"{ret_code}\"\n{test_case['cmd']}\n")   
         
        file.close()
        
def create_artifact_auth(status, output):
    file_path = f"../test-artifacts/misc/Authoritative"
    folder_path = f"../test-artifacts/misc"
    os.makedirs(folder_path, exist_ok=True)
    
    with open(file_path, 'w') as file:
        if status == "PASS":
            file.write(f"TEST Authoritative  PASSED\n")
        else:
            file.write(f"TEST Authoritative FAILED\n")
            file.write(f"Expected \"Authoritative: Yes\", got something else\n")   
            
        file.write(output)     
        file.close()
        
def create_artifact(status,domain,dig,dns,test_case):
    file_path = f"../test-artifacts/{test_case['desc']}/{domain}"
    folder_path = f"../test-artifacts/{test_case['desc']}"
    
    os.makedirs(folder_path, exist_ok=True)
    
    with open(file_path, 'w') as file:
        if status == "PASS":
            file.write(f"TEST {test_case['desc']} for {domain} PASSED\n")
        else:
            file.write(f"TEST {test_case['desc']} for {domain} FAILED {status} not matching\n")
            
        file.write("DIG output:\n")
        file.write(f"{test_case['dig']}{domain}\n")
        
        for oneDig in dig:
            file.write(f"{oneDig}\n")
            
        file.write("\nDNS output:\n")    
        file.write(f"{test_case['dns']}{domain}\n")

        for oneDns in dns:
            file.write(f"{oneDns}\n")
            
        file.close()
                             

def run_command(cmd):   
    process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    return stdout, stderr, process.returncode

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

def cut_out_auth(data):
    capture = False
    result_lines = []

    for line in data.split('\n'):
        if capture:
            result_lines.append(line.strip())
        
        if "Authority section " in line:
            capture = True
            
        if "Additional section " in line:
            capture = False
            result_lines[-1] = ""
    
    return '\n'.join(result_lines[:-1])

def cut_out_add(data):
    capture = False
    result_lines = []

    for line in data.split('\n'):
        if capture:
            result_lines.append(line.strip())
        
        if "Additional section " in line:
            capture = True
    
    return '\n'.join(result_lines[:-1])

def args():
    print("\n__________ Argument Test__________")
    for test_case in args_cases:
        output, stderr, ret_code =run_command(test_case['cmd'])
        if ret_code != test_case['exp']:
            print(f"Test {RED}FAILED{RESET} [{test_case['desc']}]")
            print(f"Expected \"{test_case['exp']}\", got \"{ret_code}\"")
            create_artifact_args(ret_code,ret_code,test_case)
        else:
            print(f"Test {GREEN}OK{RESET} [{test_case['desc']}]")
            create_artifact_args("PASS",ret_code,test_case)
        

def Authoritative():
    print("\n__________ Authoritative Test__________")
    output, stderr, ret_code =run_command("./dns -s kazi.fit.vutbr.cz nes.fit.vutbr.cz")
    
    if "Authoritative: Yes," not in output.decode("utf-8"):
        print(f"Test {RED}FAILED{RESET} [ Authoritative ]")
        print(f"Expected \"Authoritative: Yes\", got something else")
        create_artifact_auth("FAIL", output.decode("utf-8"))
    else:
        print(f"Test {GREEN}OK{RESET} [ Authoritative ]")
        create_artifact_auth("PASS",output.decode("utf-8"))  
    
def dns_vs_dig():
    for test_case in commands:
        with open(test_case['file'], 'r') as domain_file: 
            print("\n__________ " + test_case['desc'] + " Test__________")
            
            for domain in domain_file:
                domain = domain.strip()
                if test_case['desc'] == "reverse-AAAA-record":
                    domain = ipaddress.ip_address(domain).exploded
                    
                dig_output, dig_stderr, ret_code = run_command(test_case['dig'] + domain)
                dns_output,dns_stderr, ret_code= run_command(test_case['dns'] + domain)
                
                dig_output=dig_output[:-1].decode("utf-8")
                if test_case['desc'] == "Authority":
                    dns_output=cut_out_auth(dns_output.decode("utf-8"))
                elif test_case['desc'] == "Additional":
                    dns_output=cut_out_add(dns_output.decode("utf-8"))
                else:
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
                            if rtype == 'AAAA': #exploding ip6 from dig
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


def main():
    args()
    Authoritative()
    dns_vs_dig()
    

if __name__ == "__main__":
    main()
