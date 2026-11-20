// Moji testovi za citanje konfiguracionih fajlova (Config_inl.hpp).
#include "app_helper.hpp"

#include <sstream>

// Neispravan escape znak (\q) u vrednosti pod navodnicima
// mora da izazove gresku ParseError.
TEST_CASE("Moj: neispravan escape znak u vrednosti", "[moji][config]") {
    std::stringstream ulaz;
    ulaz << "ime = \"Ana\\q\"\n";

    REQUIRE_THROWS_AS(CLI::ConfigTOML().from_config(ulaz), CLI::ParseError);
}
