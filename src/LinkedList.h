/* 
 * File:   LinkedList.h
 * Author: ac45145
 *
 * Created on 3 de Setembro de 2014, 14:21
 */

#include <Arduino.h>

#ifndef LINKEDLIST_H
#define	LINKEDLIST_H

template <class T>
struct Node {
    T *element;
    Node<T> *next;

    ~Node() {
        delete element;
    }
};

template <class T>
class LinkedList {
    Node<T> *_head;
    Node<T> *_tail;
    Node<T> *_curr;
    uint16_t _size;

    Node<T> *createNode(T *t) {
        Node<T> *n = new Node<T>;
        n->element = t;
        n->next = NULL;
        return n;
    }
public:

    LinkedList() {
        _curr = _tail = _head = NULL;
        _size = 0;
    }

    ~LinkedList() {
        clear();
    }

    void add(T *t) {
        if (_tail == NULL) {//First element
            _tail = _head = createNode(t);
        } else {
            _tail = _tail->next = createNode(t);
        }
        _size++;
    }

    T* first() {
        return _head == NULL ? NULL : _head->element;
    }

    T* last() {
        return _tail == NULL ? NULL : _tail->element;
    }

    T* current() {
        return _curr == NULL ? NULL : _curr->element;
    }

    void resetCurrent() {
        _curr = NULL;
    }

    void removeFirst() {
        resetCurrent();
        if (_head == NULL) {
            return;
        }
        Node<T> *temp = _head;
        _head = _head->next;
        if (_head == NULL) {
            _tail = NULL;
        }
        delete temp;
        _size--;
    }

    void clear() {
        while (!isEmpty()) {
            removeFirst();
        }
    }

    T* next() {
        _curr = _curr == NULL ? _head : _curr->next;
        return _curr == NULL ? NULL : _curr->element;
    }

    boolean hasNext() {
        return _curr == NULL ? !isEmpty() : _curr->next != NULL;
    }

    boolean isEmpty() {
        return _size == 0;
    }
    
    uint16_t size(){
        return _size;
    }
};
#endif	/* LINKEDLIST_H */

