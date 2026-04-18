#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <iostream>
#include <cstddef> // size_t
#include <string>
#include <sstream>
#include <stdexcept>
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
    // TODO: Completar el operator++
    //t4:fowrard it(op++)
    MySelf operator++() {
        if (this->m_pNode) {
            this->m_pNode=this->m_pNode->getNext();
        }
        return *this;
    }
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
    //Ref metadadto
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
    void internal_insert(Node* &pPrev, const value_type &value, Ref ref);
public:
    LinkedList() {}
    LinkedList(const LinkedList &other) : m_pRoot(nullptr), m_tail(nullptr), m_size(0) {
        //t13:concu
        shared_lock<shared_mutex> lock(other.m_mtx); 
        for (Node* curr = other.m_pRoot; curr != nullptr; curr = curr->getNext()) {
            push_back(curr->getData(), curr->getRef());
        }
    }
    
    //t2:movve construtor
    LinkedList(LinkedList &&other): m_pRoot(nullptr), m_tail(nullptr), m_size(0) {
        //t13:concurrencia
        unique_lock<shared_mutex> lockOther(other.m_mtx);
        m_pRoot=other.m_pRoot;
        m_tail=other.m_tail;
        m_size=other.m_size;
        other.m_pRoot=nullptr;
        other.m_tail=nullptr;
        other.m_size=0;
    }
     //t3:: edestructor seguro
    virtual ~LinkedList() {
        //t13:concurrencia
        unique_lock<shared_mutex>lock(m_mtx);
        Node* current=m_pRoot;
        while(current){
            Node* next = current->getNext();
            delete current;
            current = next;
        }
        m_pRoot = nullptr;
        m_tail = nullptr;
        m_size = 0;
    }
    virtual void    push_front(value_type value, Ref ref);
    virtual void    pop_front();
    virtual void    push_back(value_type value, Ref ref);
    virtual void    pop_back();
    virtual void    insert(const value_type &value, Ref ref);
    
    virtual value_type& operator[](size_t index);
    virtual size_t  size() const;
    //virtual string  toString() const;

    forward_iterator begin() { return forward_iterator(this, m_pRoot); }
    forward_iterator end()   { return forward_iterator(this, nullptr); }
    //t12: Foreach
    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&... args) {
        //t13:concu
        unique_lock<shared_mutex> lock(m_mtx);
        if (m_size == 0) return;
        ::ForEach(begin(), end(), func, std::forward<Args>(args)... );
    }

    //t11: operator<<
    friend ostream& operator<<(ostream& os, const LinkedList& list) {
        //t13:concurencia
        shared_lock<shared_mutex>lock(list.m_mtx); 
        os<<"[";
        Node* act =list.m_pRoot;
        while(act){
            os<<"("<<act->getData()<<","<<act->getRef()<<")";
                if(act->getNext())os<<",";
                act=act->getNext();
        }
        os<<"]";
        return os;
    }
};

template <typename Trait>
void LinkedList<Trait>::internal_insert(Node* &pPrev, const value_type &value, Ref ref){
    if(!pPrev || m_comp(value, pPrev->getDataRef())){
        pPrev = new Node(value, ref, pPrev);
        m_size++;
        if(pPrev->getNext() == nullptr){
            m_tail = pPrev;}
        return;
    }
    internal_insert(pPrev->getNextRef(), value, ref);
}

template <typename Trait>
void LinkedList<Trait>::insert(const value_type &value, Ref ref){
    unique_lock<shared_mutex> lock(m_mtx); //t13
    internal_insert(m_pRoot, value, ref);
    if(m_size == 1){
        m_tail = m_pRoot;
    }else{
        Node* act=m_pRoot;
        while(act && act->getNext()){
            act=act->getNext();
        }
        m_tail=act;
    }
}
//t5: push_front
template <typename Trait>
void LinkedList<Trait>::push_front(value_type value, Ref ref) {
    unique_lock<shared_mutex> lock(m_mtx); //t13:concu
    m_pRoot = new Node(value, ref, m_pRoot);
    if(m_size == 0){
        m_tail = m_pRoot;
    }
    m_size++;
}
//t7:push_back
template <typename Trait>
void LinkedList<Trait>::push_back(value_type value, Ref ref) {
    unique_lock<shared_mutex> lock(m_mtx); //t13:concu
    Node* newNode = new Node(value, ref);
    if(m_size== 0){
        m_pRoot=newNode;
        m_tail=newNode;
    }else{
        m_tail->setNext(newNode);
        m_tail=newNode;
    }
    m_size++;
}
//t6:pop_front
template <typename Trait>
void LinkedList<Trait>::pop_front() {
    unique_lock<shared_mutex> lock(m_mtx); //t13:concu
    Node* temp=m_pRoot;
    m_pRoot=m_pRoot->getNext();
    delete temp;
    m_size=m_size-1;
    if (m_size == 0){
        m_tail = nullptr;
    } 
}
//t8: pop_back
template <typename Trait>
void LinkedList<Trait>::pop_back() {
    unique_lock<shared_mutex> lock(m_mtx); //t13:concu
    if (!m_pRoot) return;
    if (m_pRoot == m_tail) {
        delete m_pRoot;
        m_pRoot = m_tail = nullptr;
    } else {
        Node* act = m_pRoot;
        while (act->getNext() != m_tail) {
            act = act->getNext();
        }
        delete m_tail;
        m_tail = act;
        m_tail->setNext(nullptr);
    }
    m_size=m_size-1;
}

//t9:operator[]
template <typename Trait>
typename LinkedList<Trait>::value_type& LinkedList<Trait>::operator[](size_t index) {
    //t13
    shared_lock<shared_mutex> lock(m_mtx); 
    Node* act = m_pRoot;
    for (size_t i = 0; i < index; ++i) {
        act = act->getNext();
    }
    return act->getDataRef();
}

template <typename Trait>
size_t LinkedList<Trait>::size() const {
    shared_lock<shared_mutex> lock(m_mtx); //t13
    return m_size;
}

//t10:operator>>
template <typename Trait>
istream& operator>>(istream& is, LinkedList<Trait>& list) {
    typename Trait::value_type val;
    Ref ref;
    if (is >> val >> ref) {
        list.push_back(val, ref);
    }
    return is;
}

#endif // __LINKEDLIST_H__