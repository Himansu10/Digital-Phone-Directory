#ifndef UNDOSTACK_H
#define UNDOSTACK_H

#include "Contact.h"

// ------------------------------------------------------------
// UndoStack
// A stack (built on a singly linked list, LIFO) that remembers
// recently deleted contacts so the user can undo a delete.
// ------------------------------------------------------------
class UndoStack {
private:
    struct Node {
        Contact data;
        Node* next;
        Node(const Contact& c) : data(c), next(nullptr) {}
    };
    Node* topNode;
    int count;

public:
    UndoStack() : topNode(nullptr), count(0) {}

    ~UndoStack() {
        while (topNode) {
            Node* tmp = topNode;
            topNode = topNode->next;
            delete tmp;
        }
    }

    void push(const Contact& c) {
        Node* n = new Node(c);
        n->next = topNode;
        topNode = n;
        count++;
    }

    bool pop(Contact& out) {
        if (!topNode) return false;
        out = topNode->data;
        Node* tmp = topNode;
        topNode = topNode->next;
        delete tmp;
        count--;
        return true;
    }

    bool isEmpty() const { return topNode == nullptr; }
    int size() const { return count; }
};

#endif
