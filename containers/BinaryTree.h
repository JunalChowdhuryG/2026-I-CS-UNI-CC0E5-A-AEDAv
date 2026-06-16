#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include <shared_mutex>
#include "stack.h"
#include "vector.h"
#include "../types.h"
#include "traits.h"
using namespace std;

//BinaryTreeNode
template<typename T, typename DerivedNode = void>
struct BinaryTreeNode {
    using value_type = T;
    using Node = conditional_t<is_void_v<DerivedNode>, BinaryTreeNode, DerivedNode>;
    T     m_data;
    Ref   m_ref;
    Node *m_pChild[2];

    BinaryTreeNode(T data, Ref ref) : m_data(data), m_ref(ref), m_pChild{nullptr,nullptr} {}

    Node*&       child(size_t d)       { return m_pChild[d]; }
    Node* const& child(size_t d) const { return m_pChild[d]; }
};

template<typename Node, typename value_type>
class BTIteratorBase {
protected:
    Stack<Node*> m_nodes;
    DiffType    m_index;
public:
    BTIteratorBase() : m_index(0) {}
    BTIteratorBase(Stack<Node*> s, DiffType idx) : m_nodes(s), m_index(idx) {}
    value_type& operator*()  const { return m_nodes[m_index]->m_data; }
    Node*       getNode()    const { return m_nodes[m_index]; }
    bool operator==(const BTIteratorBase& o) const { return m_index == o.m_index; }
    bool operator!=(const BTIteratorBase& o) const { return m_index != o.m_index; }
};

template<typename Node, typename value_type>
class BTForwardIterator : public BTIteratorBase<Node,value_type> {
public:
    using BTIteratorBase<Node,value_type>::BTIteratorBase;
    BTForwardIterator& operator++() { ++this->m_index; return *this; }
};

template<typename Node, typename value_type>
class BTBackwardIterator : public BTIteratorBase<Node,value_type> {
public:
    using BTIteratorBase<Node,value_type>::BTIteratorBase;
    BTBackwardIterator& operator++() { --this->m_index; return *this; }
};

template<typename ForwardIt, typename BackwardIt>
class TraversalView {
    ForwardIt  m_begin, m_end;
    BackwardIt m_rbegin, m_rend;
public:
    TraversalView(ForwardIt b, ForwardIt e, BackwardIt rb, BackwardIt re)
        : m_begin(b), m_end(e), m_rbegin(rb), m_rend(re) {}
    ForwardIt  begin()  const { return m_begin;  }
    ForwardIt  end()    const { return m_end;    }
    BackwardIt rbegin() const { return m_rbegin; }
    BackwardIt rend()   const { return m_rend;   }

    template<typename Func, typename... Args>
    void forEach(Func func, Args&&... args) {
        for (auto it=begin(); it!=end(); ++it) func(*it, forward<Args>(args)...);
    }
    template<typename Func, typename... Args>
    void forEachNode(Func func, Args&&... args) {
        for (auto it=begin(); it!=end(); ++it) func(*it.getNode(), forward<Args>(args)...);
    }
};

template<typename Trait>
class BinaryTree {
public:
    using value_type = typename Trait::value_type;
    using Node       = typename Trait::Node;
    using Comp       = typename Trait::Comp;
    using ForwardIt  = BTForwardIterator<Node,value_type>;
    using BackwardIt = BTBackwardIterator<Node,value_type>;
    using InorderView   = TraversalView<ForwardIt,BackwardIt>;
    using PreorderView  = TraversalView<ForwardIt,BackwardIt>;
    using PostorderView = TraversalView<ForwardIt,BackwardIt>;

protected:
    Node                *m_pRoot;
    Comp                 m_comp;
    mutable shared_mutex m_mtx;

    virtual void internal_insert(Node* &pNode, const value_type &data, Ref ref) {
        if (!pNode) { pNode = new Node(data, ref); return; }
        auto branch = !m_comp(data, pNode->m_data);
        internal_insert(pNode->child(branch), data, ref); 
    }
    virtual void internal_clear(Node* pNode) {
        if (!pNode) return;
        internal_clear(pNode->child(0));
        internal_clear(pNode->child(1));
        delete pNode;
    }
    virtual Node* internal_copy(Node* pNode) {
        if (!pNode) return nullptr;
        Node* n       = new Node(pNode->m_data, pNode->m_ref);
        n->child(0)   = internal_copy(pNode->child(0));
        n->child(1)   = internal_copy(pNode->child(1));
        return n;
    }
    virtual Node* internal_search(Node* pNode, const value_type& data) const {
        if (!pNode) return nullptr;
        if (!m_comp(data,pNode->m_data) && !m_comp(pNode->m_data,data)) return pNode;
        return internal_search(pNode->child(!m_comp(data,pNode->m_data)), data);
    }
    virtual size_t internal_size(Node* n) const {
        if (!n) return 0;
        return 1 + internal_size(n->child(0)) + internal_size(n->child(1));
    }

    void fill_inorder(Node* n, Stack<Node*>& s) const {
        if (!n) return;
        fill_inorder(n->child(0), s); s.push(n); fill_inorder(n->child(1), s);
    }
    void fill_preorder(Node* n, Stack<Node*>& s) const {
        if (!n) return;
        s.push(n); fill_preorder(n->child(0), s); fill_preorder(n->child(1), s);
    }
    void fill_postorder(Node* n, Stack<Node*>& s) const {
        if (!n) return;
        fill_postorder(n->child(0), s); fill_postorder(n->child(1), s); s.push(n);
    }

    TraversalView<ForwardIt,BackwardIt> make_view(Stack<Node*> s) const {
        DiffType last = (DiffType)s.size()-1;
        return { ForwardIt(s,0), ForwardIt(s,(DiffType)s.size()),
                 BackwardIt(s,last), BackwardIt(s,-1) };
    }
    string traversalToString(Stack<Node*>& s) const {
        ostringstream oss; oss<<"[";
        for (size_t i=0;i<s.size();++i){
            if(i) oss<<",";
            oss<<"("<<s[i]->m_data<<","<<s[i]->m_ref<<")";
        }
        return oss<<"]", oss.str();
    }

public:
    BinaryTree() : m_pRoot(nullptr) {}
    Node* getRoot() const { return m_pRoot; }
    BinaryTree(const BinaryTree& o) : m_pRoot(nullptr) {
        shared_lock<shared_mutex> lock(o.m_mtx);
        m_pRoot = internal_copy(o.m_pRoot);
    }
    BinaryTree(BinaryTree&& o) : m_pRoot(nullptr) {
        unique_lock<shared_mutex> lock(o.m_mtx);
        m_pRoot = o.m_pRoot; o.m_pRoot = nullptr;
    }
    BinaryTree& operator=(const BinaryTree& o) {
        if (this!=&o){ clear(); shared_lock<shared_mutex> lock(o.m_mtx); m_pRoot=internal_copy(o.m_pRoot); }
        return *this;
    }
    BinaryTree& operator=(BinaryTree&& o) {
        if (this!=&o){ clear(); unique_lock<shared_mutex> lock(o.m_mtx); m_pRoot=o.m_pRoot; o.m_pRoot=nullptr; }
        return *this;
    }
    virtual ~BinaryTree() { clear(); }

    void   clear()  { unique_lock<shared_mutex> lock(m_mtx); internal_clear(m_pRoot); m_pRoot=nullptr; }
    void   insert(const value_type& data, Ref ref) { unique_lock<shared_mutex> lock(m_mtx); internal_insert(m_pRoot,data,ref); }
    size_t size()   const { shared_lock<shared_mutex> lock(m_mtx); return internal_size(m_pRoot); }
    bool   isEmpty()const { shared_lock<shared_mutex> lock(m_mtx); return m_pRoot==nullptr; }

    tuple<value_type,Ref> search(const value_type& data) const {
        shared_lock<shared_mutex> lock(m_mtx);
        Node* f = internal_search(m_pRoot,data);
        if (!f) throw runtime_error("elemento no encontrado");
        return {f->m_data, f->m_ref};
    }

    InorderView   inorder()   const { shared_lock<shared_mutex> lock(m_mtx); Stack<Node*> s; fill_inorder(m_pRoot,s);   return make_view(s); }
    PreorderView  preorder()  const { shared_lock<shared_mutex> lock(m_mtx); Stack<Node*> s; fill_preorder(m_pRoot,s);  return make_view(s); }
    PostorderView postorder() const { shared_lock<shared_mutex> lock(m_mtx); Stack<Node*> s; fill_postorder(m_pRoot,s); return make_view(s); }

    ForwardIt begin() const { return inorder().begin(); }
    ForwardIt end()   const { return inorder().end();   }

    string toString() const {
        shared_lock<shared_mutex> lock(m_mtx);
        Stack<Node*> s; string r;
        fill_inorder(m_pRoot,s);   r+="Inorder:   "+traversalToString(s)+"\n"; while(!s.empty())s.pop();
        fill_preorder(m_pRoot,s);  r+="Preorder:  "+traversalToString(s)+"\n"; while(!s.empty())s.pop();
        fill_postorder(m_pRoot,s); r+="Postorder: "+traversalToString(s);
        return r;
    }

    friend ostream& operator<<(ostream& os, const BinaryTree& t) {
        shared_lock<shared_mutex> lock(t.m_mtx);
        Stack<Node*> s; t.fill_inorder(t.m_pRoot,s); os<<t.traversalToString(s); return os;
    }
    friend istream& operator>>(istream& is, BinaryTree& t) {
        Token ch;
        if(!(is>>ch)||ch!='['){is.clear(ios_base::failbit);return is;}
        value_type val; Ref ref; Token comma, paren;
        while(is>>ch&&ch!=']')
            if(ch=='(') if(is>>val>>comma>>ref>>paren) if(comma==','&&paren==')') t.insert(val,ref);
        return is;
    }
};

#endif // __BINARYTREE_H__