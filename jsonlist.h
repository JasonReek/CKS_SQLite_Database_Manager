#ifndef JSONLIST_H
#define JSONLIST_H

#include <string>

struct Node;

class JSONList
{
public:
    int size;
    Node* head;
    Node* tail;

    JSONList();
    ~JSONList();
    void insertFront(std::string data);
    void insert(std::string data);
    void display();
    void displayReversed();
    void clear();
    bool isEmpty();
    bool hasChild();
    int length();
};

struct Node
{
    Node* next;
    Node* prev;
    JSONList* child;
    std::string key;
    std::string data;
};

#endif // JSONLIST_H
