#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H
#include <string>
#include <vector>
#include "game_record.h"

class RedBlackTree {
private:
    struct Node{
        GameRecord record;
        std::string color;
        Node* left;
        Node* right;
        Node* parent;

        Node(GameRecord record) : record(record), color("RED"), left(nullptr), right(nullptr), parent(nullptr){}
    };
    Node* root;
    Node* default_root;
    int count;

    void rotate_left(Node* node);
    void rotate_right(Node* node);
    void balance_helper(Node* node);
    void range_helper(Node* node, unsigned long long smallest_key, unsigned long long largest_key, std::vector<GameRecord>& result);
    int height_helper(Node* node);
    int node_count(Node* node);
    void delete_helper(Node* node);

  public:
    RedBlackTree();
    ~RedBlackTree();

    void insert(GameRecord& record);
    GameRecord* search(unsigned long long key);
    std::vector<GameRecord> range_search(unsigned long long k1, unsigned long long k2);
    int nodeCount();
    int height();
    int size();

};

#endif //RED_BLACK_TREE_H
