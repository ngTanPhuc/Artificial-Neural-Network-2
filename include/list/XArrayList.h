/*
 * File:   XArrayList.h
 */

#ifndef XARRAYLIST_H
#define XARRAYLIST_H
// #include "list/IList.h"
#include "IList.h"
#include <memory.h>
#include <sstream>
#include <iostream>
#include <type_traits>
using namespace std;

template <class T>
class XArrayList : public IList<T>
{
public:
    class Iterator; // forward declaration

protected:
    T *data;                                 // dynamic array to store the list's items
    int capacity;                            // size of the dynamic array
    int count;                               // number of items stored in the array
    bool (*itemEqual)(T &lhs, T &rhs);       // function pointer: test if two items (type: T&) are equal or not
    void (*deleteUserData)(XArrayList<T> *); // function pointer: be called to remove items (if they are pointer type)

public:
    XArrayList(
        void (*deleteUserData)(XArrayList<T> *) = 0,  // func ptr to delete-data func, used to deallocate memory when delete obj
        bool (*itemEqual)(T &, T &) = 0,  // func ptr to comparing func, used to compare 2 items in the list
        int capacity = 10);
    XArrayList(const XArrayList<T> &list);
    XArrayList<T> &operator=(const XArrayList<T> &list);
    ~XArrayList();

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
    void setDeleteUserDataPtr(void (*deleteUserData)(XArrayList<T> *) = 0)
    {
        this->deleteUserData = deleteUserData;
    }

    Iterator begin()
    {
        return Iterator(this, 0);
    }
    Iterator end()
    {
        return Iterator(this, count);
    }

    /** free:
     * if T is pointer type:
     *     pass THE address of method "free" to XArrayList<T>'s constructor:
     *     to:  remove the user's data (if needed)
     * Example:
     *  XArrayList<Point*> list(&XArrayList<Point*>::free);
     *  => Destructor will call free via function pointer "deleteUserData"
     */
    static void free(XArrayList<T> *list)
    {
        typename XArrayList<T>::Iterator it = list->begin();
        while (it != list->end())
        {
            delete *it;
            it++;
        }
    }

protected:
    void checkIndex(int index);     // check validity of index for accessing
    void ensureCapacity(int index); // auto-allocate if needed

    /** equals:
     * if T: primitive type:
     *      indexOf, contains: will use native operator ==
     *      to: compare two items of T type
     * if T: object type:
     *      indexOf, contains: will use native operator ==
     *      to: compare two items of T type
     *      Therefore, class of type T MUST override operator ==
     * if T: pointer type:
     *      indexOf, contains: will use function pointer "itemEqual"
     *      to: compare two items of T type
     *      Therefore:
     *      (1): must pass itemEqual to the constructor of XArrayList
     *      (2): must define a method for comparing
     *           the content pointed by two pointers of type T
     *          See: definition of "equals" of class Point for more detail
     */
    static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &))
    {
        if (itemEqual == 0)
            return lhs == rhs;
        else
            return itemEqual(lhs, rhs);
    }

    void copyFrom(const XArrayList<T> &list);

    void removeInternalData();

    //////////////////////////////////////////////////////////////////////
    ////////////////////////  INNER CLASSES DEFNITION ////////////////////
    //////////////////////////////////////////////////////////////////////
public:
    // Iterator: BEGIN
    class Iterator
    {
    private:
        int cursor;
        XArrayList<T> *pList;

    public:
        Iterator(XArrayList<T> *pList = 0, int index = 0)
        {
            this->pList = pList;
            this->cursor = index;
        }
        Iterator &operator=(const Iterator &iterator)
        {
            cursor = iterator.cursor;
            pList = iterator.pList;
            return *this;
        }
        void remove(void (*removeItemData)(T) = 0)
        {
            T item = pList->removeAt(cursor);
            if (removeItemData != 0)
                removeItemData(item);
            cursor -= 1; // MUST keep index of previous, for ++ later
        }

        T &operator*()
        {
            return pList->data[cursor];
        }
        bool operator!=(const Iterator &iterator)
        {
            return cursor != iterator.cursor;
        }
        // Prefix ++ overload
        Iterator &operator++()
        {
            this->cursor++;
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
    // Iterator: END
};

//////////////////////////////////////////////////////////////////////
////////////////////////     METHOD DEFNITION      ///////////////////
//////////////////////////////////////////////////////////////////////

template <class T>
XArrayList<T>::XArrayList(
    void (*deleteUserData)(XArrayList<T> *),
    bool (*itemEqual)(T &, T &),
    int capacity)
{
    // TODO
    this->capacity = capacity;
    this->count = 0;
    this->deleteUserData = deleteUserData;
    this->itemEqual = itemEqual;

    data = new T[capacity];
}

template <class T>
void XArrayList<T>::copyFrom(const XArrayList<T> &list)  // !Pay attention to Deep Copy
{
    /*
     * Copies the contents of another XArrayList into this list.
     * Initializes the list with the same capacity as the source list and copies all elements.
     * Also duplicates user-defined comparison and deletion functions, if applicable.
     */
    // TODO
    this->removeInternalData();
    this->capacity = list.capacity;
    this->deleteUserData = list.deleteUserData;
    this->itemEqual = list.itemEqual;
    this->count = list.count;
    
    try  // In case of memory allocation failure, catches std::bad_alloc.
    {
        this->data = new T[this->capacity];
    }
    catch(const std::bad_alloc& e)
    {
        std::cerr << "Failed to allocate memory: " << e.what() << '\n';
        throw;
    }

    // Copying data
    if (std::is_pointer<T>::value) {
        for (int i = 0; i < list.count; i++) {
            this->data[i] = (*list.data[i]);  // To ensure deep copying the pointer
        }
    } else { 
        for (int i = 0; i < list.count; i++) {
            this->data[i] = list.data[i];
        }
    }
}

template <class T>
void XArrayList<T>::removeInternalData()
{
    /*
     * Clears the internal data of the list by deleting the dynamic array and any user-defined data.
     * If a custom deletion function is provided, it is used to free the stored elements.
     * Finally, the dynamic array itself is deallocated from memory.
     */
    // TODO
    this->count = 0;

    if (this->deleteUserData != nullptr) {
        deleteUserData(this);
    }

    delete[] data;
}

template <class T>
XArrayList<T>::XArrayList(const XArrayList<T> &list)
{
    // TODO
    this->removeInternalData();
    this->capacity = list.capacity;
    this->deleteUserData = list.deleteUserData;
    this->itemEqual = list.itemEqual;
    
    this->data = new T[this->capacity];

    // Copying data  // ! To be checked
    if constexpr (std::is_pointer<T>::value) {
        for (int i = 0; i < list.count; i++) {
            *this->data[i] = (*list.data[i]);  // To ensure deep copying the pointer
        }
    } else { 
        for (int i = 0; i < list.count; i++) {
            this->data[i] = list.data[i];
        }
    }
}

template <class T>
XArrayList<T> &XArrayList<T>::operator=(const XArrayList<T> &list)
{
    // TODO
    if (this != &list) {
        delete[] data;
        this->capacity = list.capacity;
        this->deleteUserData = list.deleteUserData;
        this->itemEqual = list.itemEqual;
        this->count = list.count;
    
        this->data = new T[this->capacity];

        // Copying data
        if constexpr (std::is_pointer<T>::value) {
            for (int i = 0; i < list.count; i++) {
                this->data[i] = (*list.data[i]);  // To ensure deep copying the pointer
            }
        } else { 
            for (int i = 0; i < list.count; i++) {
                this->data[i] = list.data[i];
            }
        }
    }
    return *this;
}

template <class T>
XArrayList<T>::~XArrayList()
{
    // TODO
    this->clear();
}

template <class T>
void XArrayList<T>::add(T e)  // Adds an element e to the end of the list
{
    // TODO
    ensureCapacity(count);  // !Might be ensureCapacity(count)
    data[count] = e;
    count++;
}

template <class T>
void XArrayList<T>::add(int index, T e)
{
    // TODO
    checkIndex(index);
    ensureCapacity(count + 1);
    // Shift the array to the right
    for (int i = count - 1; i >= index; i--) {
        data[i + 1] = data[i]; 
    }
    data[index] = e; // insert data to the index
    
    count++;
}

template <class T>
T XArrayList<T>::removeAt(int index)  // !What will happen if T is a pointer type? Do we need to deallocate the memory?
{
    // TODO
    if (index < 0 || index >= count) {  // *Changed index > count - 1 to index >= count
        throw out_of_range("Index is out of range!");
    }

    T returnValue = data[index];

    for (int i = index; i < count - 1; i++) {  // *Changed i < count to i < count - 1
        data[i] = data[i + 1];
    }

    count--;
    return returnValue;
}

template <class T>
bool XArrayList<T>::removeItem(T item, void (*removeItemData)(T))  // ?Don't know if the function ptr is initially = 0 or not
{                                      // The function ptr can be used to call the function removeAt() above
    // TODO
    int index = indexOf(item);
    if (index == -1) return false;
    else {
        T dltItem = removeAt(index);
        if (removeItemData != nullptr) {
            removeItemData(dltItem);
        }
        return true;
    }
}

template <class T>
bool XArrayList<T>::empty()  // Check if the list is empty, not emptying the list
{
    // TODO
    if (count == 0) return true;
    else return false;
}

template <class T>
int XArrayList<T>::size()
{
    // TODO
    if (count > 0) return count;
    return 0;
}

template <class T>
void XArrayList<T>::clear()
{
    // TODO
    if (deleteUserData != nullptr) {
        deleteUserData(this);
    }
    delete[] data;  // Delete the allocated memory
    data = nullptr;  // Assign the pointer to null to avoid dangling pointer
    count = 0;
    capacity = 0;
}

template <class T>
T &XArrayList<T>::get(int index)
{
    // TODO
    if (index < 0 || index > count - 1 || count == 0) {  // !Might not be count == 0
        throw out_of_range("Index is out of range!");
    }
    return data[index];
}

template <class T>
int XArrayList<T>::indexOf(T item)
{
    // TODO
    for (int i = 0; i < count; i++) {
        if (itemEqual == nullptr && data[i] == item) {
            return i;
        } else if (itemEqual != nullptr && itemEqual(data[i], item)) {
            return i;
        }
    }
    return -1;
}
template <class T>
bool XArrayList<T>::contains(T item)
{
    // TODO
    int index = indexOf(item);
    if (index == -1) {
        return false;
    }
    return true;
}

template <class T>  
string XArrayList<T>::toString(string (*item2str)(T &))
{
    /**
     * Converts the array list into a string representation, formatting each element using a user-defined function.
     * If no function is provided, it uses the default string representation for each element.
     * Example: Given an array list with elements {1, 2, 3} and a function that converts integers to strings, calling toString would return "[1, 2, 3]".
     *
     * @param item2str A function pointer for converting items of type T to strings. If null, default to the string conversion of T.
     * @return A string representation of the array list with elements separated by commas and enclosed in square brackets.
     */

    // TODO
    stringstream ss;
    ss << "[";

    for (int i = 0; i < count; i++) {
        if (i > 0) {  // To make sure the "," doesn't come out in the beginning
            ss << ", ";
        }

        if (item2str != nullptr) {
            ss << item2str(data[i]);
        } else {
            ss << data[i];
        }
    }

    ss <<  "]";

    return ss.str();
}

//////////////////////////////////////////////////////////////////////
//////////////////////// (private) METHOD DEFNITION //////////////////
//////////////////////////////////////////////////////////////////////
template <class T>
void XArrayList<T>::checkIndex(int index)  // Put this in functions that work with existing elements
{
    /**
     * Validates whether the given index is within the valid range of the list.
     * Throws an std::out_of_range exception if the index is negative or exceeds the // >number of elements.
     * Ensures safe access to the list's elements by preventing invalid index operations.
     */
    // TODO
    if (index < 0 || index > count) {  // !Might be index >= count at add(T e)
        throw out_of_range("Index is out of range!");
    }
}

template <class T>
void XArrayList<T>::ensureCapacity(int index)  // Put this in functions that add new elements
{
    /*
     * Ensures that the list has enough capacity to accommodate the given index.
     * If the index is out of range, it throws an std::out_of_range exception. 
     * If the index exceeds the current // >capacity,
     * reallocates the internal array with // >increased 
     * capacity, copying the existing elements to the new array.
     * In case of memory allocation failure, catches std::bad_alloc.
     */
    // TODO
    if (index < 0 || index > capacity) {  // If the index is out of range, // *Changed index>capacity+1 to index > capacity
                                                                    //it throws an std::out_of_range exception.
        throw out_of_range("Index is out of range!");
    }

    if  (index >= this->capacity) { // If the index exceeds the current // >capacity
        int newCapacity = this->capacity * 2 + 1;
        T *newList = nullptr;
        try  // In case of memory allocation failure, catches std::bad_alloc.
        {
            newList = new T[newCapacity];
        }
        catch(const std::bad_alloc& e)
        {
            std::cerr << "Failed to allocate memory: " << e.what() << '\n';
            throw;
        }
        
        

        // Copy elements from data to newList
        for (int i = 0; i < this->count; i++) {
            newList[i] = data[i];
        }
        delete[] data;  // Deallocate the data

        // Allocate new memory to data with increased capacity and copy elements form newList to data
        try  // In case of memory allocation failure, catches std::bad_alloc.
        {
            data = new T[newCapacity];
        }
        catch(const std::bad_alloc& e)
        {
            std::cerr << "Failed to allocate memory: " << e.what() << '\n';
            throw;
        }

        for (int i = 0; i < this->count; i++) {
            data[i] = newList[i];
        }

        // Deallocate newList and assign new capacity
        delete[] newList;
        this->capacity = newCapacity;
    }
}

#endif /* XARRAYLIST_H */
