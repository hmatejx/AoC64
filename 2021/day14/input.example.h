#include <stdint.h>

const char* const polymer_template = "NNCB";

const char* rules[][2] = {
{"CH", "B"},
{"HH", "N"},
{"CB", "H"},
{"NH", "C"},
{"HB", "C"},
{"HC", "B"},
{"HN", "C"},
{"NN", "C"},
{"BH", "H"},
{"NC", "B"},
{"NB", "B"},
{"BN", "B"},
{"BB", "N"},
{"BC", "B"},
{"CC", "N"},
{"CN", "C"}};

constexpr int8_t n_rules = sizeof(rules)/sizeof(rules[0]);
