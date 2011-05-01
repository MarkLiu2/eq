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
Grafického uživatelského rozhraní pro ovládání ekvalizátoru bylo vytvořeno pomocí knohoven GTK+. duraz byl kladen na jednoduchos a přehlednost aby se uživatel rychle zoriantoval v ovládání ekvalizátoru. 

Samotné ovlivnění frekvenčních pásem je realizováno pomocí FFT - signál je převeden na spektrum, příslušné rozsahu jsou vynásobeny hodnotou získanou z posuvníku a pomocí inverzní FFT je získán upravený zvuk. Ve frekvenčním spektru mají tedy jednotlivé posuvníky vliv na obdálníkové okno. Pro spojení zvuků po inverzní Fourierově transformaci je využíváno Hannovo okno, posunuje se tedy pouze o polovinu počtu vzorků, než na kolik je aplikována FFT.

Hraniční hodnoty frekvencí odpovídajícím posuvníku jsou počítány pomocí Mel scale, tedy tak, aby odpovídaly lidskému vnímání významu frekvencí.

Popis obsluhy programu
----------------------
Po spuštění programu `./eq` se zobrazí hlavní okno. Aby bylo možné vyzkoušet, jak aplikace pěkně pracuje, je nutné nejprve otevřít .wav soubor. K tomu slouží tlačítko "Open", které se nachází na panelu nástrojů. Po otevření souboru se zobrazí ve spodní části okna základní informace (Název souboru, Frekvence a počet kanálů). Na panelu nástrojů také najdete tlačítka PLAY, PAUSE a STOP, která slouží ke spuštění resp. zastavení přehrávání zvuku. Uprostřed okna se nachýzí 8 posuvníku, jimiž je možné nastavovat hodnoty filtru jednotlivých frekvencí.

Zhodnocení výsledků
-------------------
Ovlivnění pásem je v podstatě stoprocentní, neboť se pracuje přímo s frekvenčním spektrem signálu. Oproti použití FIR filtrů, se kterými jsme taktéž experimentovali, je dosaženo znatelně lepších výsledků, neboť FIR filtry neměly obdélníkovou frekvenční charakteristiku. Co se výkonnosti týče, je toto řešení o něco výpočetně náročnější, nícméně vzhledem ke složitosti FFT je stále použitelné.

Použitá literatura
------------------
* [GTK+ 3 Reference Manual](http://developer.gnome.org/gtk3/stable/)
