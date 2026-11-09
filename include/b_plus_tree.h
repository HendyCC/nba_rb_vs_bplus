#ifndef BPLUS_TREE_H
#define BPLUS_TREE_H

#include "game_record.h"
#include <vector>

// B+ tree logic written for this project: sorted leaf keys, node splitting, linked leaves.
// These references document language/library behavior, not the origin of the tree algorithm.
// In-memory B+ tree with at most 32 keys per node.
// All game records are stored in leaves; leaves link left-to-right.
class BPlusTree {
private:
    static const int MAX_KEYS = 32;

    struct Node {
        bool leaf;
        Node* parent;
        Node* next;
        std::vector<unsigned long long> keys;
        std::vector<GameRecord> values;
        std::vector<Node*> children;

        Node(bool isLeaf) {
            leaf = isLeaf;
            parent = nullptr;
            next = nullptr;
        }
    };

    std::vector<Node*> allNodes;
    Node* root;
    unsigned long long recordCount;

    Node* makeNode(bool isLeaf);

    unsigned long long firstKey(Node* node) const;

    void rebuild(Node* node);

    void refreshAncestors(Node* node);

    Node* leafFor(unsigned long long key) const;

    void splitInternal(Node* left);

    void attachRight(Node* left, Node* right);

    void splitLeaf(Node* left);

public:
    BPlusTree();

    // Disable copying: this object owns every Node pointer.
    // Owning raw pointers cannot be shallow-copied safely (double deletion).
    // Discussion: https://stackoverflow.com/questions/4172722/what-is-the-rule-of-three
    BPlusTree(const BPlusTree&) = delete;
    BPlusTree& operator=(const BPlusTree&) = delete;

    ~BPlusTree();

    bool insert(unsigned long long key, const GameRecord& value);

    const GameRecord* find(unsigned long long key) const;

    std::vector<GameRecord> range(unsigned long long lo, unsigned long long hi) const;

    unsigned long long rangeCount(unsigned long long lo, unsigned long long hi) const;

    unsigned long long size() const;
    unsigned long long nodeCount() const;

    int height() const;
};
#endif
