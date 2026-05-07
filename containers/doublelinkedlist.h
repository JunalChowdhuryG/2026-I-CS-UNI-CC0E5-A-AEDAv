#ifndef __DOUBLELINKEDLIST_H__
#define __DOUBLELINKEDLIST_H__

#include "linkedlist.h"

// 1. NODO DOBLE (Pasa su propio tipo como parámetro a LLNode)
template <typename T>
class DLLNode : public LLNode<T, DLLNode<T>> {
private:
    DLLNode* m_prev;
public:
    DLLNode() : LLNode<T, DLLNode<T>>(), m_prev(nullptr) {}
    DLLNode(T data, Ref ref, DLLNode* next = nullptr, DLLNode* prev = nullptr)
        : LLNode<T, DLLNode<T>>(data, ref, next), m_prev(prev) {}
    
    DLLNode* getPrev() const { return m_prev; }
    void setPrev(DLLNode* prev) { m_prev = prev; }
};

template <typename T>
struct AscendingDLLTrait : BaseTrait<T, less<T>, DLLNode<T>> {};
template <typename T>
struct DescendingDLLTrait : BaseTrait<T, greater<T>, DLLNode<T>> {};

// 2. ITERADOR BACKWARD
template <typename Container>
class DLLBackwardIterator : public LinkedListForwardIterator<Container> {
public:
    using Parent = LinkedListForwardIterator<Container>;
    using Node   = typename Container::Node;
    
    DLLBackwardIterator(Node* pNode) : Parent(pNode) {}

    DLLBackwardIterator& operator++() { 
        if (this->m_pNode) {
            this->m_pNode = this->m_pNode->getPrev(); // El compilador ya sabe que es DLLNode
        }
        return *this;
    }
};

// 3. LA CLASE DOBLE
template <typename Trait>
class DoubleLinkedList : public LinkedList<Trait> {
public:
    using value_type = typename Trait::value_type;
    using Node       = typename Trait::Node; 
    using backward_iterator = DLLBackwardIterator<DoubleLinkedList<Trait>>;
    
    DoubleLinkedList() : LinkedList<Trait>() {}

    void push_back(value_type value, Ref ref) override {
        unique_lock<shared_mutex> lock(this->m_mtx);
        Node* newNode = new Node(value, ref);
        
        if (this->m_size == 0) {
            this->m_pRoot = this->m_tail = newNode;
        } else {
            this->m_tail->setNext(newNode);
            newNode->setPrev(this->m_tail); // Sin static_cast
            this->m_tail = newNode;
        }
        this->m_size++;
    }

    void push_front(value_type value, Ref ref) override {
        unique_lock<shared_mutex> lock(this->m_mtx);
        Node* newNode = new Node(value, ref, this->m_pRoot);
        
        if (this->m_size == 0) {
            this->m_tail = newNode;
        } else {
            this->m_pRoot->setPrev(newNode);
        }
        this->m_pRoot = newNode;
        this->m_size++;
    }

    std::tuple<value_type, Ref> pop_front() override {
        unique_lock<shared_mutex> lock(this->m_mtx);
        if (!this->m_pRoot) throw runtime_error("Lista vacia");
        
        Node* temp = this->m_pRoot;
        auto result = std::make_tuple(temp->getData(), temp->getRef());
        
        this->m_pRoot = temp->getNext();
        if (this->m_pRoot) {
            this->m_pRoot->setPrev(nullptr);
        } else {
            this->m_tail = nullptr;
        }
        
        delete temp;
        this->m_size--;
        return result;
    }

    std::tuple<value_type, Ref> pop_back() override {
        unique_lock<shared_mutex> lock(this->m_mtx);
        if (!this->m_pRoot) throw runtime_error("Lista vacia");
        
        Node* temp = this->m_tail;
        auto result = std::make_tuple(temp->getData(), temp->getRef());
        
        this->m_tail = this->m_tail->getPrev();
        if (this->m_tail) {
            this->m_tail->setNext(nullptr);
        } else {
            this->m_pRoot = nullptr;
        }
        
        delete temp;
        this->m_size--;
        return result;
    }

    void insert(const value_type &value, Ref ref) override {
        unique_lock<shared_mutex> lock(this->m_mtx);
        Node* newNode = new Node(value, ref);
        
        if (this->m_size == 0) {
            this->m_pRoot = this->m_tail = newNode;
        } else if (this->m_comp(value, this->m_pRoot->getDataRef())) {
            newNode->setNext(this->m_pRoot);
            this->m_pRoot->setPrev(newNode);
            this->m_pRoot = newNode;
        } else {
            Node* act = this->m_pRoot;
            while (act->getNext() != nullptr && !this->m_comp(value, act->getNext()->getDataRef())) {
                act = act->getNext();
            }
            newNode->setNext(act->getNext());
            if (act->getNext()) {
                act->getNext()->setPrev(newNode);
            } else {
                this->m_tail = newNode;
            }
            act->setNext(newNode);
            newNode->setPrev(act);
        }
        this->m_size++;
    }

    backward_iterator rbegin() { return backward_iterator(this->m_tail); }
    backward_iterator rend()   { return backward_iterator(nullptr); }

    template <typename Func, typename... Args>
    void ReverseForEach(Func func, Args &&...args) {
        unique_lock<shared_mutex> lock(this->m_mtx);
        if (this->m_size == 0) return;
        for (auto it = rbegin(); it != rend(); ++it) {
            func(*it, std::forward<Args>(args)...);
        }
    }
};

#endif // __DOUBLELINKEDLIST_H__