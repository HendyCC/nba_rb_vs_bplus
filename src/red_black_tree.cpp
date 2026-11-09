#include "red_black_tree.h"

RedBlackTree::RedBlackTree() {
    root = nullptr;
    count = 0;
}

GameRecord* RedBlackTree::search(unsigned long long key) {
    //start at root
    Node* current = root;

    while (current != nullptr) {
       if(key == current ->record.key) {
           return &current->record;
       }
        else if(key < current ->record.key) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }
    return nullptr;
}

void RedBlackTree::insert(GameRecord& record) {
   Node* parent = nullptr;
    Node* current = root;

    //find a spot to place it
    while (current != nullptr) {
        parent = current;

        //same game id
        if(record.key == current->record.key) {
               return;
        }

        else if(record.key< current->record.key) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }

    //new node
    Node* node = new Node(record);
    node->parent = parent;

    //attach new node to parent
    if(parent == nullptr) {
        root = node;
    }
    else if(record.key == parent->record.key) {
        parent->right = node;
    }

    else if(record.key < parent->record.key) {
        parent->left = node;
    }

    else {
        parent->right = node;
    }

    count++;

    if(node->parent == nullptr) {
        node->color = "BLACK";
        return;
    }

    if(node->parent->parent == nullptr) {
        return;
    }

    balance_helper(node);
}

void RedBlackTree::rotate_left(Node* node) {
    Node* temp = node->right;
    node->right = temp->left;

    if(temp->left != nullptr) {
        temp->left->parent = node;
    }
    temp->parent = node->parent;

    if(node->parent == nullptr) {
        root = temp;
    }

    else if( node == node->parent->left) {
        node->parent->left = temp;
    }
    else {
        node->parent->right = temp;
    }

    temp->left = node;
    node->parent = temp;
};

void RedBlackTree::rotate_right(Node* node) {
    Node* temp = node->left;
    node->left = temp->right;

    if(temp->right != nullptr) {
        temp->right->parent = node;
    }
    temp->parent = node->parent;

    if(node->parent == nullptr) {
        root = temp;
    }

    else if( node == node->parent->right) {
        node->parent->right = temp;
    }
    else {
        node->parent->left = temp;
    }

    temp->right = node;
    node->parent = temp;

};

void RedBlackTree::balance_helper(Node* node) {
    while(node->parent != nullptr && node->parent->color == "RED") {
        if(node->parent == node->parent->parent->left) {
            Node* uncle = node->parent->parent->right;
            if(uncle != nullptr && uncle->color == "RED") {
                //uncle is red recolor parent, uncle and grandparent
                node->parent->color = "BLACK";
                uncle->color = "BLACK";
                node->parent->parent->color = "RED";
                node = node->parent->parent;
            }
            else {

                if(node == node->parent->right) {
                    node = node->parent;
                    rotate_left(node);
                }
                node->parent->color = "BLACK";
                node->parent->parent->color = "RED";
                rotate_right(node->parent->parent);

            }

        }
        else {
          Node* uncle = node->parent->parent->left;
            if(uncle != nullptr && uncle->color == "RED") {
                node->parent->color = "BLACK";
                uncle->color = "BLACK";
                node->parent->parent->color = "RED";
                node = node->parent->parent;
            }
            else {
                if(node == node->parent->left) {
                    node = node->parent;
                    rotate_right(node);
                }
                node->parent->color = "BLACK";
                node->parent->parent->color = "RED";
                rotate_left(node->parent->parent);
            }
        }
    }
    root->color = "BLACK";
}



int RedBlackTree::size() {
    return count;
}
int RedBlackTree::nodeCount() {
    return count;
}

int RedBlackTree::height_helper(Node* node) {
    if(node == nullptr) {
        return 0;
    }
    int left_height = height_helper(node->left);
    int right_height = height_helper(node->right);

    if(left_height > right_height) {
        return left_height + 1;
    }
    else {
        return right_height + 1;
    }
}

int RedBlackTree::height() {
    return height_helper(root);
}

void RedBlackTree::range_helper(Node* node, unsigned long long smallest_key, unsigned long long largest_key, std::vector<GameRecord>& result) {
    if(node == nullptr) {
        return;
    }

    if(node->record.key > smallest_key) {
        range_helper(node->left, smallest_key, largest_key, result);
    }
    if(node->record.key >= smallest_key && node->record.key <= largest_key) {
        result.push_back(node->record);
    }
    if(node->record.key < largest_key) {
        range_helper(node->right, smallest_key, largest_key, result);
    }

}

std::vector<GameRecord> RedBlackTree::range_search(unsigned long long smallest_key, unsigned long long largest_key) {
    std::vector<GameRecord> result;
    if(largest_key < smallest_key) {
        return result;
    }
    range_helper(root, smallest_key, largest_key, result);
    return result;
}

void  RedBlackTree::delete_helper(Node* node) {
    if(node == nullptr) {
        return;
    }
    delete_helper(node->left);
    delete_helper(node->right);
    delete node;
};

RedBlackTree::~RedBlackTree() {
     delete_helper(root);
}

