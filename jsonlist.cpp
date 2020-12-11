#include "jsonlist.h"

#include <iostream>

JSONList::JSONList()
{
    this->size = 0;
    this->head = NULL;
    this->tail = NULL;
    this->child = NULL;
}

JSONList::~JSONList()
{

}

/*
    void insertFront
    -Inserts node at the front of the list.
*/
void JSONList::insertFront(std::string data)
{
    // Allocate memory for New node.
    Node* node = new Node();
    node->data = data;

    if (this->head == NULL)
    {
        this->head = node;
        this->tail = node;
    }
    else
    {
        this->head->prev = node;
        node->next = this->head;
        this->head = node;
    }
    this->size++;
}

/*
    void insertEnd
    Insert a new node at the end of the list.
*/
void JSONList::insert(std::string data)
{
    // Allocate memory for node.
    Node* node = new Node;
    node->data = data;

    if (this->tail == NULL)
    {
        this->head = node;
        this->tail = node;
    }
    else
    {
        this->tail->next = node;
        node->prev = this->tail;
        this->tail = node;
    }
    this->size++;
}

bool JSONList::isEmpty()
{
    if (size <= 0)
        return true;
    return false;
}

bool JSONList::hasChild()
{
    if(this->)
}

int JSONList::length()
{
    return this->size;
}

void JSONList::display()
{
    Node* temp = this->head;

    while (temp != NULL)
    {
        std::cout << temp->data << std::endl;
        temp = temp->next;
    }
    //std::cout << "NULL" << std::endl;
}

void JSONList::displayReversed()
{
    Node* temp = this->tail;

    while (temp != NULL)
    {
        std::cout << temp->data << std::endl;
        temp = temp->prev;
    }
    //std::cout << "NULL" << std::endl;
}

void JSONList::clear()
{
    Node* temp = this->head;

    while (temp != NULL)
    {
        temp = temp->next;
    }
}
