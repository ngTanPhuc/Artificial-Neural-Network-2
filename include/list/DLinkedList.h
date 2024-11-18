/*
 * File:   DLinkedList.h
 */

#ifndef DLINKEDLIST_H
#define DLINKEDLIST_H

#include "IList.h"

#include <sstream>
#include <iostream>
#include <type_traits>
using namespace std;

template <class T>
class DLinkedList : public IList<T>
{
public:
    class Node;        // Forward declaration
    class Iterator;    // Forward declaration
    class BWDIterator; // Forward declaration

protected:
    Node *head; // this node does not contain user's data
    Node *tail; // this node does not contain user's data
    int count;
    bool (*itemEqual)(T &lhs, T &rhs);        // function pointer: test if two items (type: T&) are equal or not
    void (*deleteUserData)(DLinkedList<T> *); // function pointer: be called to remove items (if they are pointer type)

public:
    DLinkedList(
        void (*deleteUserData)(DLinkedList<T> *) = 0,
        bool (*itemEqual)(T &, T &) = 0);
    DLinkedList(const DLinkedList<T> &list);
    DLinkedList<T> &operator=(const DLinkedList<T> &list);
    ~DLinkedList();

    // Inherit from IList: BEGIN
    void add(T e);
    void add(int index, T e);
    T removeAt(int index);
    bool removeItem(T item, void (*removeItemData)(T) = 0);
    bool empty();
    int size();
    void clear();
    T &get(int index);
    int indexOf(T item);
    bool contains(T item);
    string toString(string (*item2str)(T &) = 0);
    // Inherit from IList: END

    void println(string (*item2str)(T &) = 0)
    {
        cout << toString(item2str) << endl;
    }
    void setDeleteUserDataPtr(void (*deleteUserData)(DLinkedList<T> *) = 0)
    {
        this->deleteUserData = deleteUserData;
    }

    bool contains(T array[], int size)
    {
        int idx = 0;
        for (DLinkedList<T>::Iterator it = begin(); it != end(); it++)
        {
            if (!equals(*it, array[idx++], this->itemEqual))
                return false;
        }
        return true;
    }

    /*
     * free(DLinkedList<T> *list):
     *  + to remove user's data (type T, must be a pointer type, e.g.: int*, Point*)
     *  + if users want a DLinkedList removing their data,
     *      he/she must pass "free" to constructor of DLinkedList
     *      Example:
     *      DLinkedList<T> list(&DLinkedList<T>::free);
     */
    static void free(DLinkedList<T> *list)
    {
        typename DLinkedList<T>::Iterator it = list->begin();
        while (it != list->end())
        {
            delete *it;
            it++;
        }
    }

    /* begin, end and Iterator helps user to traverse a list forwardly
     * Example: assume "list" is object of DLinkedList

        DLinkedList<char>::Iterator it;
        for(it = list.begin(); it != list.end(); it++){
            char item = *it;
            std::cout << item; //print the item
        }
     */
    Iterator begin()
    {
        return Iterator(this, true);
    }
    Iterator end()
    {
        return Iterator(this, false);
    }

    /* last, beforeFirst and BWDIterator helps user to traverse a list backwardly
     * Example: assume "list" is object of DLinkedList

        DLinkedList<char>::BWDIterator it;
        for(it = list.last(); it != list.beforeFirst(); it--){
            char item = *it;
            std::cout << item; //print the item
        }
     */
    BWDIterator bbegin()
    {
        return BWDIterator(this, true);
    }
    BWDIterator bend()
    {
        return BWDIterator(this, false);
    }

protected:
    static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &))
    {
        if (itemEqual == 0)
            return lhs == rhs;
        else
            return itemEqual(lhs, rhs);
    }
    void copyFrom(const DLinkedList<T> &list);
    void removeInternalData();
    Node *getPreviousNodeOf(int index);

    //////////////////////////////////////////////////////////////////////
    ////////////////////////  INNER CLASSES DEFNITION ////////////////////
    //////////////////////////////////////////////////////////////////////
public:
    class Node
    {
    public:
        T data;
        Node *next;
        Node *prev;
        friend class DLinkedList<T>;

    public:
        Node(Node *next = 0, Node *prev = 0)
        {
            this->next = next;
            this->prev = prev;
        }
        Node(T data, Node *next = 0, Node *prev = 0)
        {
            this->data = data;
            this->next = next;
            this->prev = prev;
        }
    };

    //////////////////////////////////////////////////////////////////////
    class Iterator
    {
    private:
        DLinkedList<T> *pList;
        Node *pNode;

    public:
        Iterator(DLinkedList<T> *pList = 0, bool begin = true)
        {
            if (begin)
            {
                if (pList != 0)
                    this->pNode = pList->head->next;  // !head pointer points to a blank Node
                else
                    pNode = 0;
            }
            else
            {
                if (pList != 0)
                    this->pNode = pList->tail;
                else
                    pNode = 0;
            }
            this->pList = pList;
        }

        Iterator &operator=(const Iterator &iterator)
        {
            this->pNode = iterator.pNode;
            this->pList = iterator.pList;
            return *this;
        }
        void remove(void (*removeItemData)(T) = 0)
        {
            pNode->prev->next = pNode->next;
            pNode->next->prev = pNode->prev;
            Node *pNext = pNode->prev; // MUST prev, so iterator++ will go to end
            if (removeItemData != 0)
                removeItemData(pNode->data);
            delete pNode;
            pNode = pNext;
            pList->count -= 1;
        }

        T &operator*()
        {
            return pNode->data;
        }
        bool operator!=(const Iterator &iterator)
        {
            return pNode != iterator.pNode;
        }
        // Prefix ++ overload
        Iterator &operator++()
        {
            pNode = pNode->next;
            return *this;
        }
        // Postfix ++ overload
        Iterator operator++(int)
        {
            Iterator iterator = *this;
            ++*this;
            return iterator;
        }
    };
    //////////////////////////////////////////////////////////////////////
    // TODO
    class BWDIterator
    {
    private:
        DLinkedList<T> *pList;
        Node *pNode;

    public:
        BWDIterator(DLinkedList<T> *pList = 0, bool bbegin = true)
        {
            if (bbegin)
            {
                if (pList != 0)
                    this->pNode = pList->tail->prev;  // !tail pointer points to a blank Node
                else
                    pNode = 0;
            }
            else
            {
                if (pList != 0)
                    this->pNode = pList->head;
                else
                    pNode = 0;
            }
            this->pList = pList;
        }

        BWDIterator &operator=(const BWDIterator &iterator)
        {
            this->pNode = iterator.pNode;
            this->pList = iterator.pList;
            return *this;
        }
        void remove(void (*removeItemData)(T) = 0)
        {
            pNode->prev->next = pNode->next;
            pNode->next->prev = pNode->prev;
            Node *pNext = pNode->next; // MUST next, so iterator-- will go to begin
            if (removeItemData != nullptr)
                removeItemData(pNode->data);
            delete pNode;
            pNode = pNext;
            pList->count -= 1;

            if (pNode == nullptr || pNode == pList->tail || pNode == pList->head) {
                return;
            }

            Node* removedNode = pNode;
            pNode = pNode->next;

            removedNode->prev->next = removedNode->next;
            removedNode->next->prev = removedNode->prev;

            if (removeInternalData != nullptr) {
                removeInternalData(removedNode->data);
            }

            delete removedNode;
            pList->count--;
        }

        T &operator*()
        {
            return pNode->data;
        }
        bool operator!=(const BWDIterator &iterator)
        {
            return pNode != iterator.pNode;
        }
        // Prefix -- overload
        BWDIterator &operator++()
        {
            pNode = pNode->prev;
            return *this;
        }
        // Postfix -- overload
        BWDIterator operator++(int)
        {
            BWDIterator biterator = *this;
            ++(*this);
            return biterator;
        }
    };
};
//////////////////////////////////////////////////////////////////////
// Define a shorter name for DLinkedList:

template <class T>
using List = DLinkedList<T>;

//////////////////////////////////////////////////////////////////////
////////////////////////     METHOD DEFNITION      ///////////////////
//////////////////////////////////////////////////////////////////////

template <class T>  
DLinkedList<T>::DLinkedList(
    void (*deleteUserData)(DLinkedList<T> *),
    bool (*itemEqual)(T &, T &))
{
    // TODO
    this->count = 0;
    this->deleteUserData = deleteUserData;
    this->itemEqual = itemEqual;

    this->head = new Node;
    this->tail = new Node;

    this->head->next = tail;
    this->tail->prev = head;
}

template <class T>
DLinkedList<T>::DLinkedList(const DLinkedList<T> &list)
{
    // TODO
    this->head = new Node;
    this->tail = new Node;

    this->head->next = tail;
    this->tail->prev = head;

    this->count = 0;

    copyFrom(list);
}

template <class T>
DLinkedList<T> &DLinkedList<T>::operator=(const DLinkedList<T> &list)
{
    // TODO
    if (this != &list) {
        this->copyFrom(list);
    }
    return *this;
}

template <class T>
DLinkedList<T>::~DLinkedList()
{
    // TODO
    if (deleteUserData != nullptr) {
        deleteUserData(this);
    }
    Node* ptr = this->head->next;
    while (ptr != tail) {
        Node* nextPtr = ptr->next;
        delete ptr;
        ptr = nextPtr;
    }
    delete head;
    delete tail;
}

template <class T>
void DLinkedList<T>::add(T e)
{
    // TODO
    Node *q = new Node;
    q->data = e;  // (1)
    q->next = this->tail;  // (2)
    
    if (this->head->next != this->tail) {  // If the list is not empty
        Node *ptr = this->tail->prev;
        ptr->next = q;  // (3)
        q->prev = ptr;
    } else {  // If the list is empty
        this->head->next = q;  // (3)
        q->prev = this->head;  // (4)
    }
    this->tail->prev = q;  // (5)
    
    count++;
}
    
template <class T>
void DLinkedList<T>::add(int index, T e)
{
    // TODO
    if (index < 0 || index > count) {
        throw out_of_range("Index is out of range!");
    } else {
        if ((index == count) || (count == 0)) {  // If the index is at the end of the list or the list is empty
            add(e);  // Call the add function above
        } else {  // If the index is in the middle or at the beginning
            Node *q = new Node;  // (1)
            q->data = e;
            int cont = 0;  // To count to the desired index
            Node *ptr_1st = this->head->next;
            Node *ptr_2nd = this->head;
            while (cont != index) {
                ptr_2nd = ptr_1st;
                ptr_1st = ptr_1st->next;
                cont++;
            }
            q->next = ptr_1st;  // (2)
            ptr_2nd->next = q;  // (3)
            ptr_1st->prev = q;  // (4)
            q->prev = ptr_2nd;  // (5)
            count++;
        }
    }
}

template <class T>
typename DLinkedList<T>::Node *DLinkedList<T>::getPreviousNodeOf(int index)
{
    /*
     * Returns the node preceding the specified index in the doubly linked list.
     * If the index is in the first half of the list, it traverses from the head; 
     * otherwise, it traverses from the tail.
     * Efficiently navigates to the node by choosing the shorter path based on the index's position.
     */
    // TODO
    if (index < 0 || index > count) {
        throw out_of_range("Index is out of range!");
    } else {
        int middle =  this->count / 2;  // This will take the integer part only
        Node *prevNode = nullptr;
        if (index <= middle) {  // If the index is in the first-half
            int i = 0;
            prevNode = this->head->next;
            while ((i != index - 1)) {
                prevNode = prevNode->next;
                i++;
            }
        } else {  // If the index is in the last-half of the list
            int i = count - 1;
            prevNode = this->tail->prev;
            while ((i != index - 1)) {
                prevNode = prevNode->prev;
                i--;
            }
        }
        return prevNode;
    }
}

template <class T>
T DLinkedList<T>::removeAt(int index)
{
    // TODO
    if (index < 0 || index > (count-1)) {
        throw out_of_range("Index is out of range!");
    } else {
        int cont = 0;
        Node *ptr = this->head->next;
        
        while (cont != index) {
            ptr = ptr->next;
            cont++;
        }

        Node *prPtr = ptr->prev; 
        Node *nPtr = ptr->next;

        prPtr->next = nPtr;  // (1)
        nPtr->prev = prPtr;  // (2)
        T returnData = ptr->data;
        delete ptr;  // (3)

        count--;
        return returnData;
    }
}

template <class T>
bool DLinkedList<T>::empty()
{
    // TODO
    if (count == 0) return true;
    else return false;
}

template <class T>
int DLinkedList<T>::size()
{
    // TODO
    return this->count;
}

template <class T>
void DLinkedList<T>::clear()
{
    // TODO
    if (deleteUserData != nullptr) {
        deleteUserData(this);
    }
    int numItemRemove = count;
    if (!this->empty()) {
        for (int i = 0; i < numItemRemove; i++) {
            removeAt(0);
        }
    }

    count = 0;
}

template <class T>
T &DLinkedList<T>::get(int index)
{
    // TODO
    if (index < 0 || index > (count-1)) {
        throw out_of_range("Index is out of range!");
    } else {
        Node *ptr = this->head->next;
        int cont = 0;
        while (cont != index) {
            ptr = ptr->next;
            cont++;
        }
        return ptr->data;
    }
}

template <class T>
int DLinkedList<T>::indexOf(T item)
{
    // TODO
    Node *ptr = this->head->next;
    int cont = 0;
    bool found = false;
    while (ptr != this->tail && !found) {
        if (ptr->data == item) {
            return cont;
        } else if (itemEqual != nullptr && itemEqual(ptr->data, item)) {
            return cont;
        }
        ptr = ptr->next;
        cont++;
    }
    return -1;
}

template <class T>
bool DLinkedList<T>::removeItem(T item, void (*removeItemData)(T))
{
    // TODO
    Node *ptr = this->head->next;
    bool found = false;
    int index = indexOf(item);

    if (index == -1) return false;
    else {
        T dltItem = removeAt(index);
        if (removeItemData != nullptr) {
            removeItemData(dltItem);
        }
        return true;
    }
    return true;
}

template <class T>
bool DLinkedList<T>::contains(T item)
{
    // TODO
    Node *ptr = this->head->next;
    bool found = false;
    while ((ptr != this->tail)) {  // traverse through the list to find the item
        if (itemEqual != nullptr) {
            if (itemEqual(ptr->data, item)) {
                found = true;
                break;
            }
        } else {
            if (ptr->data == item) {
                found = true;
                break;
            }
        }
        ptr = ptr->next;
    }
    return found;
}

template <class T>
string DLinkedList<T>::toString(string (*item2str)(T &))
{
    /*
     * Converts the list into a string representation, where each element is formatted using a user-provided function.
     * If no custom function is provided, it directly uses the element's default string representation.
     * Example: If the list contains {1, 2, 3} and the provided function formats integers, 
     * calling toString would return "[1, 2, 3]".
     * @param item2str A function that converts an item of type T to a string. 
     * If null, default to string conversion of T.
     * @return A string representation of the list with elements separated by commas and enclosed in square brackets.
     */
    // TODO
    Node *ptr = this->head->next;
    int i = 0;
    stringstream ss;
    ss << "[";

    while (ptr != this->tail) {
        if (i > 0) {  // To make sure the "," doesn't come out in the beginning
            ss << ", ";
        }

        if (item2str != nullptr) {
            ss << item2str(ptr->data);
        } else {
            ss << ptr->data;
        }
        ptr = ptr->next;
        i++;
    }

    ss <<  "]";

    return ss.str();
}

template <class T>
void DLinkedList<T>::copyFrom(const DLinkedList<T> &list)  // !Pay attention to Deep Copy
{
    /*
     * Copies the contents of another doubly linked list into this list.
     * Initializes the current list to an empty state and then duplicates all data and pointers from the source list.
     * Iterates through the source list and adds each element, preserving the order of the nodes.
     */
    // TODO
    this->deleteUserData = list.deleteUserData;
    this->itemEqual = list.itemEqual;

    Node *listPtr = list.head->next;
    
    while (listPtr != list.tail) {
        this->add(listPtr->data);
        listPtr = listPtr->next;
    }
}

template <class T>
void DLinkedList<T>::removeInternalData()
{
    /*
     * Clears the internal data of the list by deleting all nodes and user-defined data.
     * If a custom deletion function is provided, it is used to free the user's data stored in the nodes.
     * Traverses and deletes each node between the head and tail to release memory.
     */
    // TODO
    while (!this->empty()) {
        removeAt(0);
    }
    if (this->deleteUserData != nullptr) {
        this->deleteUserData(this); 
    }
    this->head = nullptr;
    this->tail = nullptr;
    this->count = 0;
}

#endif /* DLINKEDLIST_H */