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

// Neispravan escape znak u viserednoj vrednosti ("""...""")
// mora da izazove ParseError.
TEST_CASE("Moj: neispravan escape znak u viserednoj vrednosti", "[moji][config]") {
    std::stringstream ulaz;
    ulaz << "ime = \"\"\"Ana\n";
    ulaz << "Mar\\qija\"\"\"\n";

    REQUIRE_THROWS_AS(CLI::ConfigTOML().from_config(ulaz), CLI::ParseError);
}

// Neispravan escape znak u imenu sekcije pod navodnicima
// mora da izazove ParseError.
TEST_CASE("Moj: neispravan escape znak u imenu sekcije", "[moji][config]") {
    std::stringstream ulaz;
    ulaz << "[\"mre\\qza\".podesavanja]\n";
    ulaz << "port = 8080\n";

    REQUIRE_THROWS_AS(CLI::ConfigTOML().from_config(ulaz), CLI::ParseError);
}

// Pri pisanju config fajla, string duzi od 100 znakova sa posebnim
// znakom (tab) zapisuje se kao viseredni literal '''...'''.
// Ako pocinje ili se zavrsava novim redom, dodaje se jos jedan novi red.
TEST_CASE("Moj: dug string sa novim redom na pocetku i kraju", "[moji][config]") {
    std::string dug = "\n" + std::string(110, 'a') + "\tb\n";

    std::string rezultat = CLI::detail::convert_arg_for_ini(dug, '"', '\'', false);

    CHECK(rezultat == "'''\n" + dug + "\n'''");
}

// Ako dug string vec sadrzi ''' ne moze se zapisati kao '''...''',
// pa se zapisuje binarno: tab postaje \x09, a apostrof \x27.
TEST_CASE("Moj: dug string koji sadrzi trostruke apostrofe", "[moji][config]") {
    std::string dug = std::string(110, 'a') + "\t'''";

    std::string rezultat = CLI::detail::convert_arg_for_ini(dug, '"', '\'', false);

    CHECK(rezultat == "'B\"(" + std::string(110, 'a') + "\\x09\\x27\\x27\\x27)\"'");
}

// Viseredna vrednost (""") koja nikad nije zatvorena: CLI11 cita
// do kraja fajla i vraca procitani tekst, bez greske.
TEST_CASE("Moj: nezatvorena viseredna vrednost", "[moji][config]") {
    std::stringstream ulaz;
    ulaz << "ime = \"\"\"Ana\n";
    ulaz << "Marija\n";

    std::vector<CLI::ConfigItem> izlaz = CLI::ConfigTOML().from_config(ulaz);

    REQUIRE(izlaz.size() == 1u);
    CHECK(izlaz.at(0).name == "ime");
    CHECK(izlaz.at(0).inputs.at(0) == "Ana\nMarija");
}

// Niz koji se proteze kroz vise redova i nema zatvorenu zagradu ]:
// CLI11 ipak procita sve elemente.
TEST_CASE("Moj: nezatvoren niz kroz vise redova", "[moji][config]") {
    std::stringstream ulaz;
    ulaz << "brojevi = [1,2,\n";
    ulaz << "3\n";

    std::vector<CLI::ConfigItem> izlaz = CLI::ConfigTOML().from_config(ulaz);

    REQUIRE(izlaz.size() == 1u);
    CHECK(izlaz.at(0).inputs == std::vector<std::string>{"1", "2", "3"});
}

// Pri pisanju config fajla, ime koje sadrzi apostrof i novi red
// stavlja se pod dvostruke navodnike, a novi red postaje \n.
TEST_CASE("Moj: ime sa apostrofom i novim redom", "[moji][config]") {
    std::string ime = "it's\nx";

    CLI::detail::clean_name_string(ime, "=:");

    CHECK(ime == "\"it's\\nx\"");
}
