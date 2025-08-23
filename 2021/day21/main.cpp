#include "AoC64.h"
#include "hashmap_reu.h"
#include "pearson.h"
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif

uint8_t roll_dice() {
    static uint8_t dice = 1;
    uint8_t ret = dice;
    if (++dice > 100)
        dice = 1;
    return ret;
}

uint32_t demo_play(uint8_t pos1, uint8_t pos2) {
    uint8_t pos[2] = {pos1, pos2};
    uint16_t score[2] = {0};
    uint16_t rolled = 0;
    for (;;) {
        // player 1
        uint16_t roll = (uint16_t)roll_dice() + roll_dice() + roll_dice();
        rolled += 3;
        pos[0] = (roll + pos[0]) % 10;
        if (pos[0] == 0)
            pos[0] = 10;
        score[0] += pos[0];
        if (score[0] >= 1000)
            return (uint32_t)score[1]*rolled;
        // player 2
        roll = (uint16_t)roll_dice() + roll_dice() + roll_dice();
        rolled += 3;
        pos[1] = (roll + pos[1]) % 10;
        if (pos[1] == 0)
            pos[1] = 10;
        score[1] += pos[1];
        if (score[1] >= 1000)
            return (uint32_t)score[0]*rolled;
    }
}

struct Wins {
    uint64_t wins1;
    uint64_t wins2;
};

struct GameState {
    uint8_t pos1;
    uint8_t score1;
    uint8_t pos2;
    uint8_t score2;
    const uint16_t hash() const { return hash16((const uint8_t*)this, 4); }
};
bool operator==(const GameState& lhs, const GameState& rhs) {
    return lhs.pos1 == rhs.pos1 && lhs.score1 == rhs.score1 &&
           lhs.pos2 == rhs.pos2 && lhs.score2 == rhs.score2;
}

HashMapREU<GameState, Wins, uint16_t, 20000> parallel_memo;
uint8_t iter;
Wins parallel_play(uint8_t pos1, uint8_t score1, uint8_t pos2, uint8_t score2) {
    // check the memo for result
    const GameState args = {pos1, score1, pos2, score2};
    const Wins* const memo_wins = parallel_memo.get(args);
    if (memo_wins != nullptr)
        return *memo_wins;
    // otherwise explore all combinations
    tick((iter++ >> 2) & (uint8_t)7);
    Wins wins = {};
    // 27 combinations for player 1 throws
    for (uint8_t m1 = 1; m1 < 4; ++m1) {
        for (uint8_t m2 = 1; m2 < 4; ++m2) {
            for (uint8_t m3 = 1; m3 < 4; ++m3) {
                uint8_t tpos1 = (pos1 + m1 + m2 + m3) % 10;
                if (tpos1 == 0)
                    tpos1 = 10;
                uint8_t tscore1 = score1 + tpos1;
                // player 1 wins
                if (tscore1 >= 21)
                    ++wins.wins1;
                // otherwise explor the 27 combinations of player 2 throws
                else {
                    const Wins twins = parallel_play(pos2, score2, tpos1, tscore1);
                    wins.wins1 += twins.wins2;
                    wins.wins2 += twins.wins1;
                }
            }
        }
    }
    // set the memo
    parallel_memo.insert(args, wins);
    return wins;
}

int main(void) {
    init(21);

    printf("part 1: %ld\n", demo_play(position[0], position[1]));

    // initialize the memoization cache
    reu_init(); parallel_memo.init(0);
    Wins res = parallel_play(position[0], 0, position[1], 0);
    printf("part 2: %llu\n", res.wins1 > res.wins2 ? res.wins1 : res.wins2);

    finish();
}