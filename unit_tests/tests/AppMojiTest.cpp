// Moji testovi za parsiranje komandne linije (App_inl.hpp).
#include "app_helper.hpp"

// Neispravan escape znak (\q) u vrednosti pod navodnicima na komandnoj
// liniji mora da izazove ParseError.
TEST_CASE("Moj: neispravan escape znak u komandnoj liniji", "[moji][app]") {
    CLI::App app;
    std::string ime;
    app.add_option("--ime", ime);

    REQUIRE_THROWS_AS(app.parse(R"(--ime "Ana\q")", false), CLI::ParseError);
}

// Pozicioni argument koji moze da ima 0 ili 1 vrednost: kad je zadat,
// vrednost se upisuje u promenljivu.
TEST_CASE("Moj: opcioni pozicioni argument je zadat", "[moji][app]") {
    CLI::App app;
    std::string vrednost;
    app.add_option("pozicioni", vrednost)->expected(0, 1);

    app.parse("abc", false);

    CHECK(vrednost == "abc");
}

// Isti argument, ali nije zadat: nema greske i promenljiva
// zadrzava podrazumevanu vrednost.
TEST_CASE("Moj: opcioni pozicioni argument nije zadat", "[moji][app]") {
    CLI::App app;
    std::string vrednost = "podrazumevano";
    app.add_option("pozicioni", vrednost)->expected(0, 1);

    REQUIRE_NOTHROW(app.parse("", false));

    CHECK(vrednost == "podrazumevano");
}

// Navodnik koji nikad nije zatvoren: ceo ostatak teksta
// ostaje jedan deo, umesto da se podeli po razmaku.
TEST_CASE("Moj: nezatvoreni navodnik u komandnoj liniji", "[moji][app]") {
    std::vector<std::string> delovi = CLI::detail::split_up("\"abc def");

    REQUIRE(delovi.size() == 1u);
    CHECK(delovi.at(0) == "\"abc def");
}
