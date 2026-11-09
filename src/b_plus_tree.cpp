#include "b_plus_tree.h"

// This file implements the B+ tree operations declared in b_plus_tree.h.
// Sorted keys and linked leaves enable efficient exact and range queries.
// Reference for vector insert/resize: https://en.cppreference.com/w/cpp/container/vector
// Reference for dynamic node allocation: https://en.cppreference.com/w/cpp/language/new
// These links explain C++ features; the tree algorithm is project code.

BPlusTree::Node* BPlusTree::makeNode(bool isLeaf) {
        Node* node = new Node(isLeaf);
        allNodes.push_back(node);
        return node;
    }

unsigned long long BPlusTree::firstKey(Node* node) const {
        while (!node->leaf) {
            node = node->children[0];
        }
        return node->keys[0];
    }

void BPlusTree::rebuild(Node* node) {
        if (node->leaf) return;
        node->keys.clear();
        for (int i = 1; i < (int)node->children.size(); i++) {
            node->keys.push_back(firstKey(node->children[i]));
        }
    }

void BPlusTree::refreshAncestors(Node* node) {
        while (node != nullptr && node->parent != nullptr) {
            node = node->parent;
            rebuild(node);
        }
    }

BPlusTree::Node* BPlusTree::leafFor(unsigned long long key) const {
        Node* node = root;
        while (node != nullptr && !node->leaf) {
            int index = 0;
            // Equal separator keys belong to the right child.
            while (index < (int)node->keys.size() && key >= node->keys[index]) {
                index++;
            }
            node = node->children[index];
        }
        return node;
    }

void BPlusTree::splitInternal(Node* left) {
        Node* right = makeNode(false);
        int split = (int)left->children.size() / 2;
        for (int i = split; i < (int)left->children.size(); i++) {
            Node* child = left->children[i];
            right->children.push_back(child);
            child->parent = right;
        }
        left->children.resize(split);
        rebuild(left);
        rebuild(right);
        attachRight(left, right);
    }

void BPlusTree::attachRight(Node* left, Node* right) {
        if (left->parent == nullptr) {
            Node* newRoot = makeNode(false);
            newRoot->children.push_back(left);
            newRoot->children.push_back(right);
            left->parent = newRoot;
            right->parent = newRoot;
            rebuild(newRoot);
            root = newRoot;
            return;
        }
        Node* parent = left->parent;
        int index = 0;
        while (parent->children[index] != left) {
            index++;
        }
        parent->children.insert(parent->children.begin() + index + 1, right);
        right->parent = parent;
        rebuild(parent);
        if ((int)parent->keys.size() > MAX_KEYS) {
            splitInternal(parent);
        } else {
            refreshAncestors(parent);
        }
    }

void BPlusTree::splitLeaf(Node* left) {
        Node* right = makeNode(true);
        int split = (int)left->keys.size() / 2;
        for (int i = split; i < (int)left->keys.size(); i++) {
            right->keys.push_back(left->keys[i]);
            right->values.push_back(left->values[i]);
        }
        left->keys.resize(split);
        left->values.resize(split);
        right->next = left->next;
        left->next = right;
        attachRight(left, right);
    }

BPlusTree::BPlusTree() {
        root = nullptr;
        recordCount = 0;
    }

BPlusTree::~BPlusTree() {
        for (int i = 0; i < (int)allNodes.size(); i++) {
            delete allNodes[i];
        }
    }

bool BPlusTree::insert(unsigned long long key, const GameRecord& value) {
        if (root == nullptr) {
            root = makeNode(true);
        }
        Node* leaf = leafFor(key);
        int position = 0;
        while (position < (int)leaf->keys.size() && leaf->keys[position] < key) {
            position++;
        }
        if (position < (int)leaf->keys.size() && leaf->keys[position] == key) {
            return false;
        }
        leaf->keys.insert(leaf->keys.begin() + position, key);
        leaf->values.insert(leaf->values.begin() + position, value);
        recordCount++;
        if ((int)leaf->keys.size() > MAX_KEYS) {
            splitLeaf(leaf);
        } else {
            refreshAncestors(leaf);
        }
        return true;
    }

const GameRecord* BPlusTree::find(unsigned long long key) const {
        Node* leaf = leafFor(key);
        if (leaf == nullptr) return nullptr;
        for (int i = 0; i < (int)leaf->keys.size(); i++) {
            if (leaf->keys[i] == key) return &leaf->values[i];
            if (leaf->keys[i] > key) break;
        }
        return nullptr;
    }

std::vector<GameRecord> BPlusTree::range(unsigned long long lo, unsigned long long hi) const {
        std::vector<GameRecord> result;
        if (root == nullptr || hi < lo) return result;
        Node* leaf = leafFor(lo);
        while (leaf != nullptr) {
            for (int i = 0; i < (int)leaf->keys.size(); i++) {
                if (leaf->keys[i] > hi) return result;
                if (leaf->keys[i] >= lo) result.push_back(leaf->values[i]);
            }
            leaf = leaf->next;
        }
        return result;
    }

unsigned long long BPlusTree::rangeCount(unsigned long long lo, unsigned long long hi) const {
        if (root == nullptr || hi < lo) return 0;
        unsigned long long count = 0;
        Node* leaf = leafFor(lo);
        while (leaf != nullptr) {
            for (int i = 0; i < (int)leaf->keys.size(); i++) {
                if (leaf->keys[i] > hi) return count;
                if (leaf->keys[i] >= lo) count++;
            }
            leaf = leaf->next;
        }
        return count;
    }

unsigned long long BPlusTree::size() const { return recordCount; }

unsigned long long BPlusTree::node_count() const { return allNodes.size(); }

int BPlusTree::height() const {
        int h = 0;
        Node* node = root;
        while (node != nullptr) {
            h++;
            if (node->leaf) break;
            node = node->children[0];
        }
        return h;
    }
