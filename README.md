Multimédia: Pásmový ekvalizátor
===============================

Řešitelé
--------
* Bc. Jan Kaláb (xkalab00)
* Bc. Jan Lipovský (xlipov00)
* Bc. František Skála (xskala05)

Zadání
------
Navrhněte a implementujte pásmový ekvalizátor pro filtraci zvukových souborů (např. formát WAV) a též zvuku v reálném čase. Ekvalizátor by měl mít nejméně 8 regulovatelných pásem (tvar pásem libovolný, iniciiativě se meze nekladou). Předpokládá se vytvoření grafického uživatelského rozhraní pro ovládání ekvalizátoru a též pro zobrazování parametrů zpracovávaného zvuku.

Datum vypracování
-----------------
<p>1. května 2011</p>

Popis řešení projektu
---------------------
TODO - GUI

Samotné ovlivnění frekvenčních pásem je realizováno pomocí FFT - signál je převeden na spektrum, příslušné rozsahu jsou vynásobeny hodnotou získanou z posuvníku a pomocí inverzní FFT je získán upravený zvuk. Ve frekvenčním spektru mají tedy jednotlivé posuvníky vliv na obdálníkové okno. Pro spojení zvuků po inverzní Fourierově transformaci je využíváno Hannovo okno, posunuje se tedy pouze o polovinu počtu vzorků, než na kolik je aplikována FFT.

Hraniční hodnoty frekvencí odpovídajícím posuvníku jsou počítány pomocí Mel scale, tedy tak, aby odpovídaly lidskému vnímání významu frekvencí.

Popis obsluhy programu
----------------------
TODO

Zhodnocení výsledků
-------------------
Ovlivnění pásem je v podstatě stoprocentní, neboť se pracuje přímo s frekvenčním spektrem signálu. Oproti použití FIR filtrů, se kterými jsme taktéž experimentovali, je dosaženo znatelně lepších výsledků, neboť FIR filtry neměly obdélníkovou frekvenční charakteristiku. Co se výkonnosti týče, je toto řešení o něco výpočetně náročnější, nícméně vzhledem ke složitosti FFT je stále použitelné.

Použitá literatura
------------------
TODO
