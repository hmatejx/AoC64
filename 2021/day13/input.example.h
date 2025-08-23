#include <stdint.h>

const int16_t dots[18][2] = {
{6,10},
{0,14},
{9,10},
{0,3},
{10,4},
{4,11},
{6,0},
{6,12},
{4,1},
{0,13},
{10,12},
{3,4},
{3,0},
{8,4},
{1,10},
{2,14},
{8,10},
{9,0}};

const char* folds[] =
{"fold along y=7",
"fold along x=5"};

constexpr int16_t n_dots = sizeof(dots)/sizeof(dots[0]);
constexpr int8_t n_folds = 2;
