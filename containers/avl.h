#ifndef __AVL_H__
#define __AVL_H__

#include "binarytree.h"

template<typename T>
struct AVLNode : BinaryTreeNode<T, AVLNode<T>> {
    size_t m_height;
    AVLNode(T data, Ref ref) : BinaryTreeNode<T,AVLNode<T>>(data,ref), m_height(1) {}
};

//AVL
template<typename Trait>
class AVL : public BinaryTree<Trait> {
public:
    using value_type = typename Trait::value_type;
    using Node       = typename Trait::Node;

protected:
    size_t   ht(Node* n)         const { return n ? n->m_height : 0; }
    void     upd(Node* n)              { if(n) n->m_height = 1 + max(ht(n->child(0)), ht(n->child(1))); }
    ptrdiff_t bf(Node* n)        const { return n ? (ptrdiff_t)ht(n->child(0)) - (ptrdiff_t)ht(n->child(1)) : 0; }

    //rotacion side=0 izquierdo - side=1 derecho  
    void rotate(Node* &n, size_t side) {
        auto   opp  = 1 - side;
        Node*  hijo = n->child(side);
        Node*  sub  = hijo->child(opp);
        hijo->child(opp)  = n;
        n->child(side)    = sub;
        upd(n); upd(hijo);
        n = hijo;
    }

    //rebalanceo 
    void rebalance(Node* &n) {
        upd(n);
        auto b = bf(n);
        for (size_t side = 0; side < 2; ++side) {
            ptrdiff_t peso = (side == 0) ? 1 : -1;
            if (b * peso > 1) {
                if (bf(n->child(side)) * peso < 0)
                    rotate(n->child(side), 1 - side);
                rotate(n, side);
                return;
            }
        }
    }

    void internal_insert(Node* &pNode, const value_type &data, Ref ref) override {
        if (!pNode) { pNode = new Node(data, ref); return; }
        auto branch = !this->m_comp(data, pNode->m_data);
        internal_insert(pNode->child(branch), data, ref);
        rebalance(pNode);
    }

    Node* internal_copy(Node* pNode) override {
        if (!pNode) return nullptr;
        auto* n      = new Node(pNode->m_data, pNode->m_ref);
        n->m_height  = pNode->m_height;
        n->child(0)  = internal_copy(pNode->child(0));
        n->child(1)  = internal_copy(pNode->child(1));
        return n;
    }

public:
    AVL() : BinaryTree<Trait>() {}
    AVL(const AVL& o) : BinaryTree<Trait>() {
        shared_lock<shared_mutex> lock(o.m_mtx);
        this->m_pRoot = internal_copy(o.m_pRoot);
    }
    AVL(AVL&& o) : BinaryTree<Trait>(move(o)) {}
    AVL& operator=(const AVL& o) {
        if (this!=&o){ this->clear(); shared_lock<shared_mutex> lock(o.m_mtx); this->m_pRoot=internal_copy(o.m_pRoot); }
        return *this;
    }
    AVL& operator=(AVL&& o) { BinaryTree<Trait>::operator=(move(o)); return *this; }
    virtual ~AVL() {}

    size_t    height()  const { shared_lock<shared_mutex> lock(this->m_mtx); return ht(this->m_pRoot); }
    ptrdiff_t balance() const { shared_lock<shared_mutex> lock(this->m_mtx); return bf(this->m_pRoot); }
};

#endif // __AVL_H__