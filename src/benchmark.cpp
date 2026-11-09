#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include "b_plus_tree.h"
#include "red_black_tree.h"

struct BenchmarkResult {
    double elapsedMicroseconds;
    unsigned long long node_count;
    unsigned long long recordCount;
    int height;
};

class TreeBenchmark {
public:
    static BenchmarkResult benchmarkBPlusFind(const BPlusTree& btree, unsigned long long searchKey) {
        BenchmarkResult result;
        result.node_count = btree.node_count();
        result.recordCount = btree.size();
        result.height = btree.height();

        auto start = std::chrono::high_resolution_clock::now();

        volatile const GameRecord* found = btree.find(searchKey);

        auto end = std::chrono::high_resolution_clock::now();
        result.elapsedMicroseconds = std::chrono::duration<double, std::micro>(end - start).count();

        return result;
    }
    static BenchmarkResult benchmarkBPlusRange(const BPlusTree& btree, unsigned long long lo, unsigned long long hi) {
        BenchmarkResult result;
        result.node_count = btree.node_count();
        result.recordCount = btree.size();
        result.height = btree.height();

        auto start = std::chrono::high_resolution_clock::now();
        std::vector<GameRecord> results = btree.range(lo, hi);

        auto end = std::chrono::high_resolution_clock::now();
        result.elapsedMicroseconds = std::chrono::duration<double, std::micro>(end - start).count();
        return result;
    }

    static BenchmarkResult benchmarkRBFind(RedBlackTree& rbtree, unsigned long long searchKey) {
        BenchmarkResult result;
        result.node_count = rbtree.nodeCount();
        result.recordCount = rbtree.size();
        result.height = rbtree.height();

        auto start = std::chrono::high_resolution_clock::now();
        volatile const GameRecord* found = rbtree.search(searchKey);
        auto end = std::chrono::high_resolution_clock::now();
        result.elapsedMicroseconds = std::chrono::duration<double, std::micro>(end - start).count();
        return result;
    }

    static BenchmarkResult benchmarkRBRange(RedBlackTree& rbtree, unsigned long long lo, unsigned long long hi) {
        BenchmarkResult result;
        result.node_count = rbtree.nodeCount();
        result.recordCount = rbtree.size();
        result.height = rbtree.height();

        auto start = std::chrono::high_resolution_clock::now();
        std::vector<GameRecord> results = rbtree.range_search(lo, hi);
        auto end = std::chrono::high_resolution_clock::now();

        result.elapsedMicroseconds = std::chrono::duration<double, std::micro>(end - start).count();
        return result;
    }
};