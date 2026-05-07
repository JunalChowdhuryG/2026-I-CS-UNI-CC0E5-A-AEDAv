#ifndef __CIRCULARLINKEDLIST_H__
#define __CIRCULARLINKEDLIST_H__

#include "linkedlist.h"

template <typename T>
struct AscendingCLLTrait : BaseTrait<T, less<T>, LLNode<T>> {};
template <typename T>
struct DescendingCLLTrait : BaseTrait<T, greater<T>, LLNode<T>> {};

template <typename Trait>
class CircularLinkedList : public LinkedList<Trait> {
public:
    using value_type = typename Trait::value_type;
    using Node       = typename Trait::Node; 

    CircularLinkedList() : LinkedList<Trait>() {}

    // --- MUTADORES SOBRESCRITOS (CABLEADO CIRCULAR) ---
    void push_back(value_type value, Ref ref) override {
        unique_lock<shared_mutex> lock(this->m_mtx);
        Node* newNode = new Node(value, ref);
        
        if (this->m_size == 0) {
            this->m_pRoot = this->m_tail = newNode;
            newNode->setNext(newNode); // Círculo de 1
        } else {
            newNode->setNext(this->m_pRoot);
            this->m_tail->setNext(newNode);
            this->m_tail = newNode;
        }
        this->m_size++;
    }

    void push_front(value_type value, Ref ref) override {
        unique_lock<shared_mutex> lock(this->m_mtx);
        Node* newNode = new Node(value, ref);
        
        if (this->m_size == 0) {
            this->m_pRoot = this->m_tail = newNode;
            newNode->setNext(newNode);
        } else {
            newNode->setNext(this->m_pRoot);
            this->m_tail->setNext(newNode);
            this->m_pRoot = newNode;
        }
        this->m_size++;
    }

    std::tuple<value_type, Ref> pop_front() override {
        unique_lock<shared_mutex> lock(this->m_mtx);
        if (!this->m_pRoot) throw runtime_error("Lista vacia");
        
        Node* temp = this->m_pRoot;
        auto result = std::make_tuple(temp->getData(), temp->getRef());
        
        if (this->m_size == 1) {
            this->m_pRoot = this->m_tail = nullptr;
        } else {
            this->m_pRoot = temp->getNext();
            this->m_tail->setNext(this->m_pRoot); // Reparar círculo
        }
        
        delete temp;
        this->m_size--;
        return result;
    }

    std::tuple<value_type, Ref> pop_back() override {
        unique_lock<shared_mutex> lock(this->m_mtx);
        if (!this->m_pRoot) throw runtime_error("Lista vacia");
        
        auto result = std::make_tuple(this->m_tail->getData(), this->m_tail->getRef());
        
        if (this->m_size == 1) {
            delete this->m_tail;
            this->m_pRoot = this->m_tail = nullptr;
        } else {
            Node* act = this->m_pRoot;
            while (act->getNext() != this->m_tail) {
                act = act->getNext();
            }
            delete this->m_tail;
            this->m_tail = act;
            this->m_tail->setNext(this->m_pRoot); // Reparar círculo
        }
        this->m_size--;
        return result;
    }

    void insert(const value_type &value, Ref ref) override {
        unique_lock<shared_mutex> lock(this->m_mtx);
        Node* newNode = new Node(value, ref);
        
        if (this->m_size == 0) {
            this->m_pRoot = this->m_tail = newNode;
            newNode->setNext(newNode);
        } else if (this->m_comp(value, this->m_pRoot->getDataRef())) {
            newNode->setNext(this->m_pRoot);
            this->m_tail->setNext(newNode);
            this->m_pRoot = newNode;
        } else {
            Node* act = this->m_pRoot;
            while (act->getNext() != this->m_pRoot && !this->m_comp(value, act->getNext()->getDataRef())) {
                act = act->getNext();
            }
            newNode->setNext(act->getNext());
            act->setNext(newNode);
            if (act == this->m_tail) {
                this->m_tail = newNode;
            }
        }
        this->m_size++;
    }

    // --- MÉTODOS EXCLUSIVOS ---
    template <typename Func, typename... Args>
    void circularForEach(size_t vueltas, Func func, Args &&...args) {
        unique_lock<shared_mutex> lock(this->m_mtx);
        if (this->m_size == 0 || vueltas == 0) return;
        
        Node* act = this->m_pRoot;
        size_t pasos = this->m_size * vueltas;
        for (size_t i = 0; i < pasos; ++i) {
            func(act->getDataRef(), std::forward<Args>(args)...);
            act = act->getNext();
        }
    }
};

#endif // __CIRCULARLINKEDLIST_H__