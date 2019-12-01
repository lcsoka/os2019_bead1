# ELTE IK Oprendszerek 2019 1. és 2. beadandó

A futtatashoz buildeljuk le a 'make' paranccsal a programot,
majd a ./bead paranccsal futtassuk.

## Feladatleírás

A "Szakács Tamás" utazási iroda csődbe ment. Sok üdülőhelyen maradtak utasok. A csőd "hullámai" elmosták a pontos nyilvántartást is, így mikor az utasok hazaszállítását szervezik nem is tudják pontosan, hogy hol, kik maradtak.

Készítsen C nyelven programot(aminek egy GNU C/C++ fordítóval Linux rendszeren, ilyen például az opsys.inf.elte.hu is, fordulnia és futnia kell) aminek segítségével a különböző helyeken rekedt utasok fel tudnak iratkozni a mentésre várók listájára. Erre a listára fel kell venni az utas nevét, a helyszínt (csak az utazási iroda rögzített helyszínei adottak, Bali, Mali, Cook szigetek, Bahamák, Izland), az utas telefonszámát és az ott rekedt csoport utazási módját (repülő, hajó, autóbusz). 

Az adatokat fájlban tároljuk, az adatfelvételen túl legyen lehetőségünk az adatokon módosítani, törölni és helyszínenként listáztatni.

A csődbiztos (szülő) amint gyűlnek a mentésre várók, készíti fel a mentőexpedíciókat. Minden úti célhoz adott egy minimum létszám, ami alatt nem indítható expedíció. Amint összejön a létszám adott helyen, a mentőexpedíció elindul (gyerekfolyamat). Amint odaér, visszajelez csődbiztosnak, hogy kéri az utaslistát. Ekkor csődbiztos csövön továbbítja az adatokat az expedíciónak, hogy kiket kell felvenni. Amint az expedíció hazaért, jelzést küld a csődbiztosnak, majd üzenetsoron összegzi, hogy honnan, hány utast hoztak haza. 