## Obsah
1. Úvod do problematiky
    1. Překlad doménových jmen
    2. Řízení dotazů a přenos dat
    3. Chybové zprávy a ošetření chyb
    4. Konfigurovatelnost a Flexibilita
2. popis implementace
3. základní informace o programu
4. Návod na použití
5. Testovani

## 1. Úvod do problematiky


### 1.1 Překlad doménových jmen

DNS resolver musí efektivně překládat doménová jména na odpovídající IP adresy. To zahrnuje správné sestavení DNS dotazu, který je následně odeslán na DNS server. Resolver musí také řešit specifikace, jako jsou rekurzivní dotazy, dotazy na reverzní adresy nebo požadavky na IPv6 adresy. [RFC1035]

### 1.2 Řízení dotazů a přenos dat

Implementace musí správně řídit různé druhy dotazů a zajistit přenos dat mezi uživatelem a DNS serverem. To může zahrnovat manipulaci s DNS zprávami, správu připojení k serverům a zajištění efektivní komunikace. [RFC1035]

### 1.3 Chybové zprávy a ošetření chyb

DNS resolver by měl být schopen zpracovávat chybové stavy a situace. To zahrnuje interpretaci chybových zpráv od DNS serverů a vhodnou reakci na ně .[RFC1035]


## 2. Popis implementace

Dns resolver je implementován pomocí 3 tříd: SocketHandler, ParamParser a DnsMsg. Třída ParamParser obsahuje třídní metodu paramProcess, která zpracovává jednotlivé argumenty přijaté při spuštění programu. Následně předá zpracovaný port a adresu serveru objektu SocketHandler, který otevře UDP socket pro příslušnou adresu. V případě potřeby si vyhledá IP adresu pomocí funkce gethostbyname().

Dále objekt ParamParser předá zpracované argumenty objektu třídy DnsMsg, který příslušně nastaví hlavičku a otázku DNS zprávy.

## Použitá literatura
 -y