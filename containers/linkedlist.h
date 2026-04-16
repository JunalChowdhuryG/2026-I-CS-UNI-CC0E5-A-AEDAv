#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <iostream>
#include <cstddef> // size_t
#include <string>
#include <sstream>
#include <mutex>
#include <shared_mutex> // shared_mutex
#include "general_iterator.h"
#include "util.h"
#include "../types.h"
using namespace std;

// Forward iterator
template <typename Container>
class LinkedListForwardIterator : public general_iterator<Container, LinkedListForwardIterator<Container>>{
public:
    using MySelf = LinkedListForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
    MySelf& operator++() {
        if (this->m_pNode)
            this->m_pNode = this->m_pNode->getNext();
        return *this;
    }
    MySelf operator++(int) { MySelf tmp = *this; ++(*this); return tmp; }
};

// Linked List Node
template <typename T>
class LLNode{
    using Node = LLNode<T>;
private:
    T   m_data;
    Ref m_ref;
    Node *m_next;
public:
    LLNode() : m_data(T()), m_ref(Ref()), m_next(nullptr) {}
    LLNode(T data, Ref ref) : m_data(data), m_ref(ref), m_next(nullptr) {}
    LLNode(T data, Ref ref, Node *next) : m_data(data), m_ref(ref), m_next(next) {}
    virtual ~LLNode() {}

    T      getData() const { return m_data; }
    T&     getDataRef()    { return m_data; }
    void   setData(T data) { m_data = data; }
    Ref    getRef() const  { return m_ref; }
    void   setRef(Ref ref) { m_ref = ref; }
    Node*  getNext() const { return m_next; }
    Node*& getNextRef()    { return m_next; }
    void   setNext(Node *next) { m_next = next; }
};

template <typename T>
ostream& operator<<(ostream& os, LLNode<T>& node){
    return os << "(" << node.getData() << ", " << node.getRef() << ")";
}

template <typename T>
struct AscendingLinkedListTrait{
    using value_type = T;
    using Node = LLNode<T>;
    using Comp = less<T>;
};

template <typename T>
struct DescendingLinkedListTrait{
    using value_type = T;
    using Node = LLNode<T>;
    using Comp = greater<T>;
};

template <typename Trait>
class LinkedList{
public:
    using value_type = typename Trait::value_type;
    using Node       = typename Trait::Node;
    using Comp       = typename Trait::Comp;
    using MySelf     = LinkedList<Trait>;

    using forward_iterator = LinkedListForwardIterator<MySelf>;
    friend forward_iterator;

private:
    Node *m_pRoot = nullptr;
    Node *m_tail = nullptr;
    size_t m_size = 0;
    Comp   m_comp;
    mutable shared_mutex m_mtx;
public:
    LinkedList() {}
    LinkedList(const LinkedList &other){ // Copy constructor
        unique_lock<shared_mutex> lock(other.m_mtx);
        Node *p = other.m_pRoot;
        while(p){
            push_back(p->getData(), p->getRef());
            p = p->getNext();
        }
    }
    LinkedList(LinkedList &&other){ // Move constructor
        unique_lock<shared_mutex> lock(other.m_mtx);
        m_pRoot = other.m_pRoot;
        m_tail = other.m_tail;
        m_size = other.m_size;
        other.m_pRoot = nullptr;
        other.m_tail = nullptr;
        other.m_size = 0;
    }
    LinkedList& operator=(const LinkedList &other){ // Copy assignment operator
        if(this == &other) return *this;
        unique_lock<shared_mutex> lock1(m_mtx, defer_lock);
        unique_lock<shared_mutex> lock2(other.m_mtx, defer_lock);
        lock(lock1, lock2);

        clear();
        Node *p = other.m_pRoot;
        while(p){
            push_back_no_lock(p->getData(), p->getRef());
            p = p->getNext();
        }
        return *this;
    }
    LinkedList& operator=(LinkedList &&other){ // Move assignment operator
        if(this == &other) return *this;
        unique_lock<shared_mutex> lock1(m_mtx, defer_lock);
        unique_lock<shared_mutex> lock2(other.m_mtx, defer_lock);
        lock(lock1, lock2);

        clear();
        m_pRoot = other.m_pRoot;
        m_tail = other.m_tail;
        m_size = other.m_size;
        other.m_pRoot = nullptr;
        other.m_tail = nullptr;
        other.m_size = 0;
        return *this;
    }
    
    virtual        ~LinkedList() {
        clear();
    }

    void clear() {
        Node *p = m_pRoot;
        while(p){
            Node *next = p->getNext();
            delete p;
            p = next;
        }
        m_pRoot = nullptr;
        m_tail = nullptr;
        m_size = 0;
    }

    virtual void    push_front(value_type value, Ref ref);
    virtual void    pop_front();
    virtual void    push_back(value_type value, Ref ref);
    virtual void    pop_back();

private:
    void    push_back_no_lock(value_type value, Ref ref);
    void    internal_insert(Node* &pPrev, const value_type &value, Ref ref);
public:
    virtual void    insert(const value_type &value, Ref ref);
    
    virtual value_type& operator[](size_t index);
    virtual size_t  size() const;
    virtual string  toString() const;

    forward_iterator begin() { return forward_iterator(this, m_pRoot); }
    forward_iterator end()   { return forward_iterator(this, nullptr); }

    // Agregar Foreach
    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&...  args){
        shared_lock<shared_mutex> lock(m_mtx);
        ::ForEach(begin(), end(), func, std::forward<Args>(args)... );
    }
};

template <typename Trait>
void LinkedList<Trait>::push_front(value_type value, Ref ref){
    unique_lock<shared_mutex> lock(m_mtx);
    m_pRoot = new Node(value, ref, m_pRoot);
    if(!m_tail) m_tail = m_pRoot;
    m_size++;
}

template <typename Trait>
void LinkedList<Trait>::pop_front(){
    unique_lock<shared_mutex> lock(m_mtx);
    if(m_pRoot){
        Node *p = m_pRoot;
        m_pRoot = m_pRoot->getNext();
        delete p;
        m_size--;
        if(!m_pRoot) m_tail = nullptr;
    }
}

template <typename Trait>
void LinkedList<Trait>::push_back_no_lock(value_type value, Ref ref){
    Node *newNode = new Node(value, ref);
    if(m_tail){
        m_tail->setNext(newNode);
        m_tail = newNode;
    } else {
        m_pRoot = m_tail = newNode;
    }
    m_size++;
}

template <typename Trait>
void LinkedList<Trait>::push_back(value_type value, Ref ref){
    unique_lock<shared_mutex> lock(m_mtx);
    push_back_no_lock(value, ref);
}

template <typename Trait>
void LinkedList<Trait>::pop_back(){
    unique_lock<shared_mutex> lock(m_mtx);
    if(!m_pRoot) return;
    if(m_pRoot == m_tail){
        delete m_pRoot;
        m_pRoot = m_tail = nullptr;
    } else {
        Node *p = m_pRoot;
        while(p->getNext() != m_tail){
            p = p->getNext();
        }
        delete m_tail;
        m_tail = p;
        m_tail->setNext(nullptr);
    }
    m_size--;
}

template <typename Trait>
void LinkedList<Trait>::internal_insert(Node* &pPrev, const value_type &value, Ref ref){
    if(!pPrev || m_comp(value, pPrev->getDataRef())){
        pPrev = new Node(value, ref, pPrev);
        m_size++;
        if(!pPrev->getNext())
            m_tail = pPrev;
        return;
    }
    internal_insert(pPrev->getNextRef(), value, ref);
}

template <typename Trait>
void LinkedList<Trait>::insert(const value_type &value, Ref ref){
    unique_lock<shared_mutex> lock(m_mtx);
    internal_insert(m_pRoot, value, ref);
}

template <typename Trait>
typename LinkedList<Trait>::value_type& LinkedList<Trait>::operator[](size_t index){
    shared_lock<shared_mutex> lock(m_mtx);
    if(index >= m_size) throw out_of_range("Index out of range");
    Node *p = m_pRoot;
    for(size_t i = 0; i < index; ++i)
        p = p->getNext();
    return p->getDataRef();
}

template <typename Trait>
size_t LinkedList<Trait>::size() const {
    shared_lock<shared_mutex> lock(m_mtx);
    return m_size;
}

template <typename Trait>
string LinkedList<Trait>::toString() const {
    shared_lock<shared_mutex> lock(m_mtx);
    ostringstream oss;
    oss << "[";
    Node *p = m_pRoot;
    while(p){
        oss << "(" << p->getData() << ", " << p->getRef() << ")";
        p = p->getNext();
        if(p) oss << ",";
    }
    oss << "]";
    return oss.str();
}

template <typename Trait>
ostream& operator<<(ostream& os, const LinkedList<Trait>& ll){
    return os << ll.toString();
}

template <typename Trait>
istream& operator>>(istream& is, LinkedList<Trait>& ll){
    // Expects [(val, ref), (val, ref), ...]
    char c;
    is >> c; // '['
    if(c != '[') return is;
    while(is >> c && c != ']'){
        if(c == '('){
            typename Trait::value_type val;
            Ref ref;
            is >> val >> c >> ref >> c; // val, ref)
            ll.push_back(val, ref);
            is >> c; // ',' or ']'
            if(c == ']') break;
        }
    }
    return is;
}

#endif // __LINKEDLIST_H__
