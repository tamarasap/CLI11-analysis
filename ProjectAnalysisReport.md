# Izveštaj o analizi projekta CLI11

## 1. Uvod

### 1.1 Analizirani projekat

- Projekat: [CLI11](https://github.com/CLIUtils/CLI11)
- Grana: `main`
- Verzija: v2.7.2 (poslednje stabilno izdanje u trenutku analize)
- Commit: `cbd58a3696887b34c70949aef21a71735a0c2ad5`

Analizirano je stabilno izdanje, a ne najnovije stanje grane `main`, jer se projekat aktivno razvija.

### 1.2 Opis projekta

CLI11 je C++11 biblioteka za parsiranje argumenata komandne linije. Prepoznaje opcije i pozicione
argumente, pretvara vrednosti u odgovarajuće tipove i proverava njihovu ispravnost. Iste opcije
mogu se pročitati i iz konfiguracionog fajla (INI/TOML).

Organizacija koda:

| Direktorijum | Sadržaj |
|--------------|---------|
| `include/CLI/` | zaglavlja biblioteke (`App.hpp`, `Option.hpp`, `Validators.hpp`, ...) |
| `include/CLI/impl/` | implementacija (`App_inl.hpp`, `Config_inl.hpp`, ...) |
| `tests/` | postojeći testovi projekta (Catch2) |
| `fuzz/` | programi za fuzz testiranje |
| `examples/` | primeri upotrebe |

## 2. Jedinični testovi i pokrivenost koda

### 2.1 Opis alata

Jedinični test proverava jednu funkciju ili klasu odvojeno od ostatka programa: zada joj ulaz i
uporedi rezultat sa očekivanim. Testove biramo tako da pokriju granične vrednosti, neispravan ulaz i
posebne slučajeve.

CLI11 koristi radni okvir **Catch2**, pa su i moji testovi napisani u njemu. Korišćeni makroi:

- `TEST_CASE` - jedan test,
- `CHECK` i `REQUIRE` - provera uslova (`REQUIRE` prekida test ako uslov nije ispunjen),
- `REQUIRE_THROWS_AS` - provera da je bačen očekivani izuzetak,
- `REQUIRE_NOTHROW` - provera da izuzetak nije bačen.

Pokrivenost koda pokazuje koje delove koda su testovi zaista izvršili. Neizvršen kod je neproveren
kod. Pokrivenost ne dokazuje da je kod ispravan, ali pokazuje koje testove treba dopisati.

- **Pokrivenost linija** - da li je linija bar jednom izvršena.
- **Pokrivenost funkcija** - da li je funkcija bar jednom pozvana.
- **Pokrivenost grana** - da li je svaki uslov (`if`) isproban i kad je tačan i kad je netačan.
  Ovo je najstroža od tri mere.

Za merenje sam koristila **gcov** (deo GCC-a) i **lcov** (prikupljanje podataka i HTML izveštaj).

### 2.2 Pokretanje

```bash
./unit_tests/run_tests.sh pocetna   # samo postojeći testovi CLI11
./unit_tests/run_tests.sh posle     # postojeći testovi + moji testovi
```

Skripta radi u 4 koraka:

1. prevodi CLI11 i testove sa opcijama `--coverage -O0`; nastaju `.gcno` fajlovi,
2. pokreće testove; tokom izvršavanja nastaju `.gcda` fajlovi sa brojačima izvršavanja,
3. `lcov --capture` prikuplja brojače, a `lcov --extract` zadržava samo kod biblioteke iz
   `CLI11/include/` (bez testova i sistemskih zaglavlja),
4. pravi tabelu (`summary.txt`) i HTML izveštaj (`html/index.html`).

Skripta automatski bira `gcov` iste verzije kao `g++` (na mom računaru `gcov-10`), jer bi se u
suprotnom format podataka razlikovao i rezultati ne bi bili pouzdani.

Moji testovi se nalaze u `unit_tests/tests/` i imaju poseban CMake projekat
(`unit_tests/CMakeLists.txt`), kako ne bih menjala izvorni kod projekta u submodulu. Koriste istu
verziju Catch2 (2.13.10) i isti pomoćni fajl `app_helper.hpp` kao postojeći testovi CLI11.

### 2.3 Početna pokrivenost (postojeći testovi projekta)

| Mera | Pokrivenost |
|------|-------------|
| Linije | 98,9% (5125 od 5180) |
| Funkcije | 78,3% (2022 od 2581) |
| Grane | 63,0% (6686 od 10607) |

Pokrivenost linija je vrlo visoka, ali je pokrivenost grana znatno niža. Najviše nepokrivenih
linija (30) imao je fajl `Config_inl.hpp`, zadužen za čitanje i pisanje konfiguracionih fajlova.

### 2.4 Napisani testovi

Testove sam smišljala metodom **bele kutije**: u HTML izveštaju sam pronašla nepokrivenu liniju,
pročitala uslove ispred nje i napravila ulaz koji ih ispunjava. Najviše pažnje posvetila sam fajlu
`Config_inl.hpp`, jer je imao najviše nepokrivenih linija, a ulaz je običan tekst, pa se lako prave
ispravni i neispravni primeri.

**Čitanje konfiguracionog fajla** (`tests/ConfigMojiTest.cpp`):

| Test | Vrsta | Šta proverava |
|------|-------|---------------|
| Neispravan escape znak u vrednosti | neispravan ulaz | `ime = "Ana\q"` mora da izazove `ParseError` |
| Neispravan escape znak u višerednoj vrednosti | neispravan ulaz | isto, za vrednost `"""..."""` |
| Neispravan escape znak u imenu sekcije | neispravan ulaz | `["mre\qza".podesavanja]` mora da izazove `ParseError` |
| Nezatvorena višeredna vrednost | nepotpun ulaz | CLI11 čita do kraja fajla bez greške |
| Nezatvoren niz kroz više redova | nepotpun ulaz | elementi niza se ipak pročitaju |

**Pisanje konfiguracionog fajla** (`tests/ConfigMojiTest.cpp`):

| Test | Vrsta | Šta proverava |
|------|-------|---------------|
| Dug string sa novim redom na početku i kraju | granična vrednost | string duži od 100 znakova zapisuje se kao `'''...'''` |
| Dug string koji sadrži `'''` | poseban slučaj | prelazi se na binarni zapis `'B"(...)"'`, jer bi `'''` pokvario zapis |
| Ime sa apostrofom i novim redom | poseban slučaj | ime se zapisuje pod dvostrukim navodnicima |

**Komandna linija i imena opcija** (`tests/AppMojiTest.cpp`, `tests/ImenaMojiTest.cpp`):

| Test | Vrsta | Šta proverava |
|------|-------|---------------|
| Neispravno ime pozicionog argumenta | neispravan ulaz | ime `ime=x` mora da izazove `BadNameString` |
| Neispravan escape znak u komandnoj liniji | neispravan ulaz | `--ime "Ana\q"` mora da izazove `ParseError` |
| Opcioni pozicioni argument je zadat | ispravan ulaz | vrednost se upisuje u promenljivu |
| Opcioni pozicioni argument nije zadat | granična vrednost | nema greške, ostaje podrazumevana vrednost |
| Nezatvoreni navodnik u komandnoj liniji | nepotpun ulaz | `"abc def` ostaje jedan deo |

Svih 13 testova prolazi.

### 2.5 Pokrivenost nakon dodavanja testova

| Fajl | Linije pre | Linije posle | Grane pre | Grane posle |
|------|------------|--------------|-----------|-------------|
| `Config_inl.hpp` | 93,9% (459/489) | 96,3% (471/489) | 63,9% (759/1188) | 66,5% (790/1188) |
| `Split_inl.hpp` | 97,7% (84/86) | 98,8% (85/86) | 69,4% (129/186) | 71,0% (132/186) |
| `App_inl.hpp` | 99,2% | 99,5% | 72,5% | 72,8% |
| **Ceo CLI11** | 98,9% (5125/5180) | 99,3% (5146/5180) | 63,0% (6686/10607) | 63,5% (6731/10607) |

Broj nepokrivenih linija u celoj biblioteci smanjen je sa 55 na 34.

Da je određenu liniju pokrio baš moj test, proverila sam poređenjem HTML izveštaja `pocetna` i
`posle`: jedina razlika između ta dva merenja su moji testovi.

### 2.6 Zaključak

- CLI11 je vrlo dobro testiran projekat: postojeći testovi pokrivaju 98,9% linija.
- Pokrivenost grana je znatno niža (oko 63%). Veliki deo nepokrivenih grana nije moguće dostići
  testovima, jer ih kompajler ubacuje za obradu izuzetaka (na primer, ako pravljenje stringa ne
  uspe zbog nedostatka memorije). Na primer, u `ExtraValidators_inl.hpp` su sve linije pokrivene,
  a oko 42% grana nije.
- Mojih 13 testova pokriva 21 liniju koja ranije nije bila izvršena, uglavnom obradu neispravnog i
  nepotpunog ulaza u konfiguracionim fajlovima i na komandnoj liniji. Dva testa ne podižu
  pokrivenost, ali proveravaju ponašanje u graničnim slučajevima.
- Preostale nepokrivene linije su uglavnom zatvorene zagrade funkcija koje gcov broji zbog obrade
  izuzetaka, ili delovi koda do kojih se teško dolazi.
- Testovima nije pronađena greška u kodu.
