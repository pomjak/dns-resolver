## Obsah
1. Úvod do problematiky
2. popis implementace
3. Návod na použití
4. Testovani

## 1. Úvod do problematiky

### 1.1 Úvod do resolveru
Resolver je program, který slouží k propojení uživatelských programů s doménovými jmennými servery. V nejjednodušším případě resolver obdrží požadavek od uživatelského programu ve formě podprogramového volání, systémového volání atd. a vrátí požadované informace ve formě kompatibilní s datovými formáty místního hosta. [rfc1034]

Resolver je umístěn na stejném stroji jako program, který vyžaduje služby resolveru, ale může být nutné konzultovat jmenné servery na jiných hostech. Protože resolver může potřebovat konzultovat několik jmenných serverů nebo může mít požadované informace v místní mezipaměti, doba, kterou resolver potřebuje k dokončení, se může značně lišit, od milisekund po několik sekund. [rfc1034]

Prvním krokem, který resolver provede, je transformace požadavku klienta, vyjádřeného ve formátu vhodném pro místní operační systém, do hledací specifikace pro záznamy zóny na konkrétním jménu, které odpovídají konkrétnímu typu otázky (`QTYPE`) a třídě otázky (`QCLASS`). [rfc1035]


### 1.2 Zjednodušený resolver (Stub resolver) 

Jednou možností implementace resolveru je přesunout funkci rozlišování z místního stroje do jmenného serveru, který podporuje rekurzivní dotazy. To může poskytnout snadný způsob poskytování služby domény na PC, které nemá dostatek prostředků k provádění funkce resolveru, nebo může centralizovat mezipaměť pro celou místní síť nebo organizaci.[rfc1034]


## Návrh
Po spojení informací z zadání , RFC 1034 a RFC 1035 je resolver implementován jako "stub resolver", který nemá seznam DNS serverů, které může kontaktovat při dotazech. Na rozdíl od "`stub resolveru`" z RFC, má pouze adresu jednoho serveru, která je zadána uživatelem. Podle standardu může resolver v některých případech komunikovat pomocí protokolu `TCP`, ale dle zadání to není požadováno [RFC1034]. Z důvodu zjednodušení implementace probíhá komunikace výhradně přes `UDP`. `TCP` komunikace a vyhledávání v mezipaměti tedy není implementováno. Resolver tedy konzultuje dotazy pouze s jedním DNS serverem a interpretuje jeho odpovědi a chybové stavy.



## 2. Popis implementace

### Obecný popis implementace
Dns resolver je implementován pomocí 3 tříd: `SocketHandler`, `ParamParser` a `DnsMsg`. Třída `ParamParser` obsahuje třídní metodu `paramProcess`, která zpracovává jednotlivé argumenty přijaté při spuštění programu. Následně předá zpracovaný port a adresu serveru objektu `SocketHandler`, který otevře UDP socket pro příslušnou adresu. V případě potřeby si vyhledá IP adresu pomocí funkce `gethostbyname()`.

Dále objekt ParamParser předá zpracované argumenty objektu třídy `DnsMsg`, který příslušně nastaví hlavičku a otázku DNS zprávy.

Předá vytvořenou zprávu objektu třídy `SocketHandler`, který ji zašle na požadovaný DNS server a očekává odpověď. Následně jí vrátí objektu třídy `DnsMsg`. Ten zpracuje přijatou zprávu a vypíše ji uživateli na standardní výstup. V případě chyby resolveru nebo serveru na chybový výstup. Nakonec se program ukončí s odpovídajícím návratovým kódem.<br>
Znazorneni komunikace mezi tridami pomoci sekvencniho diagramu
[seq_dia]
 
### Podrobnější popis zajímavých částí
####  Překlad adresy DNS serveru
V případě, že resolver neobdrží IP adresu DNS serveru, ale pouze doménové jméno DNS serveru, je nutné toto jméno přeložit na odpovídající IP adresu. Resolver tedy musí provést dotaz na překlad doménového jména. Pro tento případ je použita funkce gethostbyname(), která může vrátit několik IP adres. Resolver následně iteruje přes vrácené IP adresy a končí buď při prvním úspěchu, když najde platnou IP adresu, nebo končí neúspěšně a ukončuje program. Existuje také možnost rozšíření implementace nad rámec zadání, jak resolver dokáže zjistit IP adresu bez použití funkce `gethostbyname()`.

V tomto případě může resolver nahlédnout do souboru `/etc/hosts`, zda se v něm nachází záznam o překladu doménového jména. V případě neúspěchu provede nahlédnutí do souboru `/etc/resolv.conf`, kde jsou implicitně zadány servery, kterých se může dotazovat. Tímto způsobem může resolver získat potřebnou IP adresu. Celkově tedy provede svou funkci dvakrát – nejprve pro získání IP adresy DNS serveru a poté pro dotaz na adresu zadávanou uživatelem.

#### Kontrola adres
Ve stávající implementaci není provedena žádná kontrola získaných adres od uživatele, a předpokládá se, že uživatel zadá adresu ve správném formátu odpovídajícím použitým symbolům či jejich kombinacím. Chybný vstup bude odhalen až při dotazu na DNS server, který vrátí odpovídající chybové hlášení. Resolver následně propaguje chybu a vrátí totožný návratový kód, jaký obdržel od DNS serveru. 

V následujícím příkladu je spuštěn DNS resolver s adresou `1.1.1.1`, avšak není uveden příznak `-x`, který nastavuje typ dotazu na reverzní dotaz. Adresa je tedy špatně přeložena, a DNS server zasílá chybu *3* `Name Error [No such name]`, na základě které resolver ukončuje program s návratovým kódem *3*.

```bash
$ ./dns -s dns.google 1.1.1.1
Authoritative: No, Recursive: No, Truncated: No, ERROR: Name Error [No such name]
Question section (1)
  1.1.1.1., A, IN
Answer section (0)
Authority section (1)
  , SOA, IN, 84776, a.root-servers.net., nstld.verisign-grs.com.,
   2023111500, 1800, 900, 604800, 86400
Additional section (0)

$ echo $?
3
```

#### Kontrola obdržených zpráv
Je také prováděna kontrola jednotlivých příznaků hlavičky příchozí zprávy. Zvláště je kontrolováno ID, aby se zajistilo, že se shoduje s ID dotazu a odpovědi. Také se kontroluje priznak qr, který určuje, zda přijímaná zpráva je ve skutečnosti odpovědí, a ne další dotaz.

V případě, že je zpráva zkrácena, musí být nastaven priznak TC DNS serverem. Resolver by měl zahodit UDP datagram a přejít na TCP[serverfault]. Ovšem komunikace pomocí TCP není implementována, a v tomto případě se vypíše celá, i když zkrácená, zpráva spolu s informací`Truncation : Yes`.
[https://serverfault.com/questions/991520/how-is-truncation-performed-in-dns-according-to-rfc-1035]

#### Reverzní adresa
Standard RFC 1035 specifikuje způsob zasílání reverzních dotazů pomocí nastavení OPCODE v hlavičce zprávy na hodnotu 2, což reprezentuje reverzní dotaz.[rfc1035] V praxi většina serverů odpoví chybovou zprávou `not-implemented error`, což je povinná vlastnost implementace DNS serveru. Alternativou je zaslat IP adresu v reverzním formátu s příponou `in-addr.arpa` pro **IPv4** nebo v reverzním formátu s příponou `ip6.arpa` pro **IPv6** adresy.



## Použitá literatura
 -y