# ISA - DNS Resolver

**Autor:** Jakub Pomsár  
**Datum:** 14. 11. 2023  

## Obsah
1. Úvod
2. Spuštění programu
3. Chybový výstup
4. Struktura


## 1. Úvod

Program `dns` je implementací **DNS resolveru**, což znamená, že se specializuje na překlad lidsky čitelných doménových jmen na odpovídající IP adresy. Jeho hlavním úkolem je efektivně zpracovat vstup od uživatele, který obsahuje různé parametry a dotazy na DNS. Po získání vstupních informací vytvoří program odpovídající dotaz na DNS server, přičemž respektuje případné specifikace, jako jsou rekurze, dotazy na reverzní adresy nebo dotazy na IPv6 adresy.

Po odeslání dotazu na DNS server program čeká na odpověď. V případě úspěchu zobrazí obdrženou odpověď, která může obsahovat záznamy typu `A`, `AAAA`, `CNAME`, `PTR`, `NS` či `SOA`  V případě chyby nebo jiných problémů program zobrazí chybovou zprávu, která pomáhá uživateli identifikovat a pochopit příčinu selhání dotazu.

Celkově program dns slouží jako prostředník mezi uživatelem a DNS serverem, usnadňující překlad doménových jmen na IP adresy a umožňující uživateli efektivní práci s DNS.

## 2. Spuštění programu
Program lze spustit následovně:

```bash
USAGE:
./dns [-r] [-x] [-6] [-p port] -s server adresa
```
Kde: <br>
*   Symbol `r` označuje požadovanou rekurzi (*recursion desired*).
*   Symbol `x` signalizuje dotaz na reverzní adresu (*záznam PTR*).
*   Symbol `6` indikuje dotaz na IPv6 adresu (*záznam AAAA*).
*   Symbol `p` s argumentem `port` umožňuje specifikovat jiný port než výchozí *53*.
*   Povinný symbol `s` s argumentem `server` definuje DNS server, na který je dotaz odeslán.
*   Povinný argument `adresa` určuje předmět dotazu.

Při kombinaci symbolů `x` a `6` program interpretuje jako reverzní dotaz na IPv6 adresu.

## 3. Chybový výstup
Pokud nejsou zadány všechny povinné argumenty, program se ukončí s chybovým hlášením a výpisem použití (USAGE).

Pokud jsou zadány argumenty nad rámec výše zmíněného použití, jsou ignorovány, vypsáno je použití (USAGE), a program pokračuje.

V případě, že není možné navázat spojení se serverem, program ukončí s chybovým výstupem a odpovídajícím návratovým kódem.

Program **neprovádí žádnou kontrolu adresy** a očekává, že uživatel zadá adresu ve správném formátu odpovídajícím použitým symbolům či jejich kombinacím. Chybný vstup bude odhalen až při dotazu na DNS server, který vrátí odpovídající chybové hlášení. Typicky bývá takovéto chybové hlášení vypsáno následovně.

```bash
./dns -s kazi.fit.vutbr.cz 192.168.0.1

Authoritative: No, Recursive: No, Truncated: No, ERROR: Name Error [No such name]
Question section (1)
  192.168.0.1., A, IN
Answer section (0)
Authority section (1)
  , SOA, IN, 86400, a.root-servers.net., nstld.verisign-grs.com., 2023111400, 1800, 900, 604800, 86400
Additional section (0)
```
V tomto případě byl na server zaslán dotaz s adresou *192.168.0.1*, avšak nebyl specifikován argument `-x`, který by adresu správně zformátoval a nastavil příslušné příznaky v hlavičce zprávy. Tudíž DNS server vrací chybu *Name Error [No such name]*, a program se ukončí s odpovídajícím návratovým kódem (v tomto případě **3**).

## 4. Struktura 
* include/  
    * DnsMsg.h
    * Maps.h
    * paramParser.h
    * socketHandler.h

* src/
    * Dns.cc    
    * DnsMsg.cc
    * Maps.cc
    * paramParser.cc
    * socketHandler.cc

* test/
    * domains
    * ips
    * ips6
    * test.py

* CHANGELOG.md
* LICENSE
* Makefile
* manual.pdf
* README.md