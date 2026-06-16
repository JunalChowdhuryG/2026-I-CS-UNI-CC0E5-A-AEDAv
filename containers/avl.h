#ifndef __AVL_H__
#define __AVL_H__

#include "binarytree.h"

template<typename T>
struct AVLNode : BinaryTreeNode<T, AVLNode<T>> {
    size_t m_height;
    AVLNode(T data, Ref ref) : BinaryTreeNode<T,AVLNode<T>>(data,ref), m_height(1) {}
};

template<typename Trait>
class AVL : public BinaryTree<Trait> {
public:
    using value_type = typename Trait::value_type;
    using Node       = typename Trait::Node;

protected:
    //altura/balance
    size_t ht(Node* n)         const { return n ? n->m_height : 0; }
    void   upd(Node* n)              { if(n) n->m_height = 1 + max(ht(cast(n,0)), ht(cast(n,1))); }
    auto    bf(Node* n)         const { return n ? (int)ht(cast(n,0)) - (int)ht(cast(n,1)) : 0; }
    Node*  cast(Node* n, size_t d)  const { return static_cast<Node*>(n->m_pChild[d]); }
    Node*& castRef(Node* n, size_t d)   { return reinterpret_cast<Node*&>(n->m_pChild[d]); }

    //rotación unificada
    void rotate(Node* &n, size_t side) {
        auto  opp  = 1 - side;
        Node* hijo = cast(n, side);       
        Node* sub  = cast(hijo, opp);     
        hijo->m_pChild[opp] = n;      
        n->m_pChild[side]   = sub; 
        upd(n); upd(hijo);
        n = hijo;
    }

    //rebalanceo
    void rebalance(Node* &n) {
        upd(n);
        auto b = bf(n);
        for (size_t side = 0; side < 2; ++side) {
            auto peso = (side == 0) ? 1 : -1;   
            auto opp  = 1 - side;
            if (b * peso > 1) {
                if (bf(cast(n, side)) * peso < 0)
                    rotate(castRef(n, side), opp); 
                rotate(n, side);
                return;
            }
        }
    }

    //insert rebalanceo
    void internal_insert(Node* &pNode, const value_type &data, Ref ref) override {
        if (!pNode) { pNode = new Node(data, ref); return; }
        auto branch = !this->m_comp(data, pNode->m_data);
        internal_insert(reinterpret_cast<Node*&>(pNode->m_pChild[branch]), data, ref);
        rebalance(pNode);
    }

    //copia conserv m_height
    Node* internal_copy(Node* pNode) override {
        if (!pNode) return nullptr;
        auto* n        = new Node(pNode->m_data, pNode->m_ref);
        n->m_height    = pNode->m_height;
        n->m_pChild[0] = internal_copy(cast(pNode, 0));
        n->m_pChild[1] = internal_copy(cast(pNode, 1));
        return n;
    }

public:
    AVL() : BinaryTree<Trait>() {}
    AVL(const AVL& other) : BinaryTree<Trait>() {
        shared_lock<shared_mutex> lock(other.m_mtx);
        this->m_pRoot = internal_copy(other.m_pRoot);
    }
    AVL(AVL&& other) : BinaryTree<Trait>(std::move(other)) {}
    AVL& operator=(const AVL& other) {
        if (this != &other) { this->clear(); shared_lock<shared_mutex> lock(other.m_mtx); this->m_pRoot = internal_copy(other.m_pRoot); }
        return *this;
    }
    AVL& operator=(AVL&& other) { BinaryTree<Trait>::operator=(std::move(other)); return *this; }
    virtual ~AVL() {}

    size_t height()  const { shared_lock<shared_mutex> lock(this->m_mtx); return ht(this->m_pRoot); }
    int    balance() const { shared_lock<shared_mutex> lock(this->m_mtx); return bf(this->m_pRoot); }
};

#endif