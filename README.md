# Analiza projekta CLI11

Seminarski rad iz kursa **Verifikacija softvera**, Matematički fakultet, Univerzitet u Beogradu.

## Autor

- Ime i prezime: Tamara Šaponjić
- Broj indeksa: 1125/2026


## O projektu

[CLI11](https://github.com/CLIUtils/CLI11) je C++11 biblioteka za parsiranje argumenata komandne linije.
Programer opiše koje opcije program prima (`--ime`, `-n`, pozicione argumente, potkomande), a CLI11
pri pokretanju programa prepozna argumente, pretvori ih u odgovarajuće tipove, proveri njihovu
ispravnost i automatski napravi `--help`. Iste opcije mogu da se pročitaju i iz konfiguracionog
fajla (INI/TOML).

- Izvorni kod: https://github.com/CLIUtils/CLI11
- Analizirana grana: `main`
- Analizirana verzija: **v2.7.2**
- Commit: [`cbd58a3696887b34c70949aef21a71735a0c2ad5`](https://github.com/CLIUtils/CLI11/commit/cbd58a3696887b34c70949aef21a71735a0c2ad5)

Projekat je uključen kao git submodul u direktorijumu `CLI11/`.

## Preuzimanje repozitorijuma

```bash
git clone --recurse-submodules https://github.com/tamarasap/CLI11-analysis.git
```

## Korišćeni alati

| # | Alat | Direktorijum | Status |
|---|------|--------------|--------|
| 1 | Jedinični testovi (Catch2) + pokrivenost koda (gcov, lcov) | `unit_tests/` | završeno |
| 2 | Valgrind Memcheck | `valgrind/memcheck/` | u izradi |
| 3 | cppcheck | `cppcheck/` | planirano |
| 4 | libFuzzer | | planirano |
| 5 | clang-tidy | | planirano |
| 6 | hyperfine ili heaptrack | | planirano |

## Uputstva za reprodukciju rezultata

### 1. Jedinični testovi i pokrivenost koda

Potrebno: `g++`, `cmake` (3.14+), `lcov`.

```bash
./unit_tests/run_tests.sh pocetna   # samo postojeći testovi projekta CLI11
./unit_tests/run_tests.sh posle     # postojeći testovi + moji testovi iz unit_tests/tests/
```

Rezultati:

- `unit_tests/coverage/<oznaka>/summary.txt`: tabela pokrivenosti,
- `unit_tests/coverage/<oznaka>/html/index.html`: HTML izveštaj.

## Zaključci

1. **Jedinični testovi i pokrivenost:** CLI11 je vrlo dobro testiran (98,9% linija pokriveno
   postojećim testovima). Napisala sam 13 testova koji pokrivaju 21 liniju koju postojeći testovi
   nisu izvršavali (pokrivenost linija 98,9% → 99,3%), uglavnom obradu neispravnog i nepotpunog
   ulaza u konfiguracionim fajlovima. Testovima nije pronađena greška.

Detaljan opis analize nalazi se u fajlu [ProjectAnalysisReport.md](ProjectAnalysisReport.md).
