#include "b_plus_tree.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <random>
#include <vector>

GameRecord sample(unsigned long long key) {
    GameRecord r;
    r.key = key;
    r.gameId = key / 2;
    r.teamId = "TEST";
    if (key % 2 == 0) r.side = "home";
    else r.side = "away";
    r.points = (int)(key % 151);
    return r;
}

int main() {
    BPlusTree tree;
    assert(tree.size() == 0 && tree.height() == 0);
    assert(tree.find(42) == nullptr);
    assert(tree.range(0, 100).empty());

    std::vector<unsigned long long> keys;
    for (unsigned long long i = 0; i < 150000; ++i) keys.push_back(i * 3 + 10);
    std::mt19937 generator(3530);
    std::shuffle(keys.begin(), keys.end(), generator);
    for (auto key : keys) assert(tree.insert(key, sample(key)));

    assert(tree.size() == keys.size());
    assert(tree.nodeCount() > 1 && tree.height() > 1);
    for (auto key : keys) {
        const GameRecord* found = tree.find(key);
        assert(found != nullptr && found->key == key);
        assert(!tree.insert(key, sample(key)));
    }
    assert(tree.size() == keys.size());
    assert(tree.find(9) == nullptr);
    assert(tree.find(11) == nullptr);
    assert(tree.range(100, 90).empty());

    std::sort(keys.begin(), keys.end());
    std::uniform_int_distribution<int> randomIndex(0, (int)keys.size()-1);
    for (int test = 0; test < 3000; ++test) {
        unsigned long long lo = keys[randomIndex(generator)];
        unsigned long long hi = lo + 150;
        std::vector<GameRecord> results = tree.range(lo, hi);
        auto first = std::lower_bound(keys.begin(), keys.end(), lo);
        auto last = std::upper_bound(keys.begin(), keys.end(), hi);
        assert(results.size() == (unsigned long long)(last - first));
        assert(tree.rangeCount(lo, hi) == results.size());
        for (unsigned long long j = 0; j < results.size(); ++j)
            assert(results[j].key == *(first + j));
    }
    std::cout << "PASS: 150,000 shuffled insertions, all exact lookups, duplicates, and 3,000 ranges\n";
}
