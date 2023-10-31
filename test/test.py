import subprocess

DIG = "dig @147.229.8.12 +noall +answer"
PROG = "test -s 147.229.8.12 "

RED = '\033[0;31m'
GREEN = '\033[0;32m'
YELLOW = '\033[0;33m'
RESET = '\033[0m'

def run_command(command):
    result = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    return result.stdout

def main():
    with open('domains', 'r') as domain_file: 
        for domain in domain_file:
            domain = domain.strip()

            dig_output = run_command(f"{DIG} {domain}")
            prog_output = run_command(f"{PROG} {domain}")

            no_dig = len(dig_output.splitlines())

            parsed_dns = None
            lines = prog_output.splitlines()
            last_complete = ""
            for line in lines:
                if "Authority" in line:
                    break
                last_complete = line

            parsed_dns = last_complete

            no_dns = len(parsed_dns.splitlines())

            for i in range(1, no_dig + 1):
                dig_line = dig_output.splitlines()[i - 1]
                dig_name, dig_ttl, dig_class, dig_type, dig_data = dig_line.split()
                dig_name = dig_name.strip()
                dig_class = dig_class.strip()
                dig_type = dig_type.strip()
                dig_data = dig_data.strip()

                dns_line = parsed_dns.splitlines()[i - 1].replace(" ", "")
                dns_name, dns_type, dns_class, dns_ttl, dns_data = dns_line.split(',')
                dns_name = dns_name.strip()
                dns_type = dns_type.strip()
                dns_class = dns_class.strip()
                dns_ttl = dns_ttl.strip()
                dns_data = dns_data.strip()

                if dig_name != dns_name:
                    print(dig_line)
                    print(dns_line)
                    print(f"Test {RED}FAILED{RESET} [name of {domain}]")
                    print(f"Expected \"{dig_name}\", got \"{dns_name}\"")
                    exit(1)
                elif dig_class != dns_class:
                    print(dig_line)
                    print(dns_line)
                    print(f"Test {RED}FAILED{RESET} [class of {domain}]")
                    print(f"Expected \"{dig_class}\", got \"{dns_class}\"")
                    exit(1)
                elif dig_type != dns_type:
                    print(dig_line)
                    print(dns_line)
                    print(f"Test {RED}FAILED{RESET} [type of {domain}]")
                    print(f"Expected \"{dig_type}\", got \"{dns_type}\"")
                    exit(1)
                elif dig_data != dns_data:
                    print(dig_line)
                    print(dns_line)
                    print(f"Test {RED}FAILED{RESET} [addr of {domain}]")
                    print(f"Expected \"{dig_data}\", got \"{dns_data}\"")
                    exit(1)
                else:
                    print(f"Test {GREEN}SUCCEEDED{RESET} [ {domain} ]")

if __name__ == "__main__":
    main()
