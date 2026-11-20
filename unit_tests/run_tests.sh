#!/bin/bash
# Prevodi CLI11 sa pokrivenoscu koda, pokrece testove i pravi lcov izvestaj.
# Upotreba: ./run_tests.sh [oznaka]   (podrazumevano: pocetna)

set -euo pipefail

OZNAKA="${1:-pocetna}"

SKRIPTA_DIR="$(cd "$(dirname "$0")" && pwd)"
KOREN="$(dirname "$SKRIPTA_DIR")"
CLI11_DIR="$KOREN/CLI11"
BUILD_DIR="$KOREN/build-cov"
REZ_DIR="$SKRIPTA_DIR/coverage/$OZNAKA"
# gcov mora biti iste verzije kao g++ kojim je kod preveden.
GCC_VERZIJA="$(g++ -dumpversion | cut -d. -f1)"
if command -v "gcov-$GCC_VERZIJA" > /dev/null; then
    GCOV="gcov-$GCC_VERZIJA"
else
    GCOV="gcov"
fi
echo "== Koristi se $GCOV (g++ verzija $GCC_VERZIJA)"

# lcov 2.x je strozi od 1.x i prekida rad zbog nebitnih neslaganja
# u sistemskim zaglavljima, pa mu se kaze da ih samo prijavi.
LCOV_VERZIJA="$(lcov --version | grep -o '[0-9]\+' | head -1)"
CAPTURE_DODATNO=()
if [ "$LCOV_VERZIJA" -ge 2 ]; then
    CAPTURE_DODATNO=(--ignore-errors mismatch)
fi
# 1. Prevodjenje sa --coverage (ubacuje brojace izvrsavanja u kod)
echo "== 1/4 Prevodjenje"
rm -rf "$BUILD_DIR"
cmake -S "$CLI11_DIR" -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCLI11_BUILD_TESTS=ON \
    -DCLI11_BUILD_EXAMPLES=OFF \
    -DCLI11_WARNINGS_AS_ERRORS=OFF \
    -DCMAKE_CXX_FLAGS="--coverage -O0"
cmake --build "$BUILD_DIR" -j"$(nproc)"

# 2. Pokretanje testova (tokom izvrsavanja nastaju .gcda fajlovi)
echo "== 2/4 Pokretanje testova"
ctest --test-dir "$BUILD_DIR" --output-on-failure

# 3. Prikupljanje pokrivenosti i izdvajanje samo koda biblioteke
#    (bez samih testova i sistemskih zaglavlja)

# 2b. Moji testovi iz unit_tests/tests/ (za svaku oznaku osim "pocetna")
if [ "$OZNAKA" != "pocetna" ]; then
    echo "== 2b Prevodjenje i pokretanje mojih testova"
    cmake -S "$SKRIPTA_DIR" -B "$BUILD_DIR/moji" \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_CXX_FLAGS="--coverage -O0"
    cmake --build "$BUILD_DIR/moji" -j"$(nproc)"
    ctest --test-dir "$BUILD_DIR/moji" --output-on-failure
fi
echo "== 3/4 Prikupljanje pokrivenosti"
lcov --capture --directory "$BUILD_DIR" \
    --output-file "$BUILD_DIR/coverage.info" \
    --gcov-tool "$GCOV" --rc lcov_branch_coverage=1 \
    "${CAPTURE_DODATNO[@]}"
lcov --extract "$BUILD_DIR/coverage.info" "$CLI11_DIR/include/*" \
    --output-file "$BUILD_DIR/coverage.filtered.info" \
    --rc lcov_branch_coverage=1

# 4. Izvestaji: tabela u tekstualnom fajlu i HTML stranica
echo "== 4/4 Pravljenje izvestaja"
rm -rf "$REZ_DIR"
mkdir -p "$REZ_DIR"
lcov --summary "$BUILD_DIR/coverage.filtered.info" \
    --rc lcov_branch_coverage=1 2>&1 | tee "$REZ_DIR/summary.txt"
lcov --list "$BUILD_DIR/coverage.filtered.info" \
    --rc lcov_branch_coverage=1 | tee -a "$REZ_DIR/summary.txt"
genhtml "$BUILD_DIR/coverage.filtered.info" \
    --branch-coverage --output-directory "$REZ_DIR/html"

echo "== Gotovo. Rezultati su u: $REZ_DIR"
