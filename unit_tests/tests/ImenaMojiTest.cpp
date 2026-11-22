// Moji testovi za proveru imena opcija (Split_inl.hpp).
#include "app_helper.hpp"

// Ime pozicionog argumenta ne sme da sadrzi '=' (to je separator
// izmedju imena i vrednosti), pa CLI11 mora da baci BadNameString.
TEST_CASE("Moj: neispravno ime pozicionog argumenta", "[moji][names]") {
    CLI::App app;

    REQUIRE_THROWS_AS(app.add_option("ime=x"), CLI::BadNameString);
}
