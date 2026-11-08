#ifndef GAME_RECORD_H
#define GAME_RECORD_H
#include <string>
// Project adapter (not a header supplied by Kaggle): fields were modeled from the
// NBA game CSV for tree testing. Dataset/integration teammate should confirm
// names and key encoding before merging, and keep one shared definition.
struct GameRecord {
    unsigned long long key = 0; // two indexed records per NBA game (home=0, away=1)
    unsigned long long gameId = 0;
    std::string date;
    std::string teamId;
    std::string side;
    int points = 0;
    // Compare all fields when checking that two indexes return the same record.
    bool operator==(const GameRecord& other) const {
        return key == other.key && gameId == other.gameId && date == other.date &&
               teamId == other.teamId && side == other.side && points == other.points;
    }
};
#endif
