#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include <tuple>
#include <utility>
#include "../types.h"
#include "avl.h"
using namespace std;

//KVPair
template <typename Key, typename Value, typename Comp = less<Key>>
struct KVPair {
    using value_type = KVPair;
    Key           m_key;
    mutable Value m_value;

    KVPair() : m_key(), m_value() {}
    KVPair(const Key& k, const Value& v = Value{}) : m_key(k), m_value(v) {}
    bool operator< (const KVPair& o) const { return Comp{}(m_key, o.m_key); }
    bool operator> (const KVPair& o) const { return Comp{}(o.m_key, m_key); }
    bool operator==(const KVPair& o) const { return !(*this < o) && !(o < *this); }

    friend ostream& operator<<(ostream& os, const KVPair& p) { return os << p.m_key << ":" << p.m_value; }
    friend istream& operator>>(istream& is, KVPair& p)       { Token c; return is >> p.m_key >> c >> p.m_value; }
};

//for (const auto& [k, v] : table)
namespace std {
    template<typename K,typename V,typename C> struct tuple_size<KVPair<K,V,C>>      : integral_constant<size_t,2>{};
    template<typename K,typename V,typename C> struct tuple_element<0,KVPair<K,V,C>> { using type = const K; };
    template<typename K,typename V,typename C> struct tuple_element<1,KVPair<K,V,C>> { using type = V; };
}
template<size_t I,typename K,typename V,typename C> decltype(auto) get(KVPair<K,V,C>& p)
    { if constexpr(I==0) return (const K&)p.m_key; else return p.m_value; }
template<size_t I,typename K,typename V,typename C> decltype(auto) get(const KVPair<K,V,C>& p)
    { if constexpr(I==0) return (const K&)p.m_key; else return (const V&)p.m_value; }

//HashTable
template <typename Trait>
class HashTable {
public:
    using Key   = typename Trait::Key;
    using Value = typename Trait::Value;
    using Comp  = typename Trait::Comp;
    using Pair  = KVPair<Key, Value, Comp>;
    using Tree  = AVL<AscendingTrait<AVLNode<Pair>>>;
    using Node  = typename Tree::Node;

private:
    Tree                 m_tree;
    mutable shared_mutex m_mtx;
    Node* buscar(const Key& key) const {
        Node* n = m_tree.getRoot();
        while (n) {
            if (key == n->m_data.m_key) return n;
            n = n->child(Comp{}(key, n->m_data.m_key) ? 0 : 1);
        }
        return nullptr;
    }

public:
    HashTable() = default;

    HashTable(const HashTable& o) {
        shared_lock<shared_mutex> lock(o.m_mtx);
        m_tree = o.m_tree;
    }
    HashTable(HashTable&& o) {
        unique_lock<shared_mutex> lock(o.m_mtx);
        m_tree = move(o.m_tree);
    }
    HashTable& operator=(const HashTable& o) {
        if (this != &o) { unique_lock<shared_mutex> lk(m_mtx); shared_lock<shared_mutex> lo(o.m_mtx); m_tree = o.m_tree; }
        return *this;
    }
    HashTable& operator=(HashTable&& o) {
        if (this != &o) { unique_lock<shared_mutex> lk(m_mtx); unique_lock<shared_mutex> lo(o.m_mtx); m_tree = move(o.m_tree); }
        return *this;
    }
    ~HashTable() = default;

    //operator[]
    Value& operator[](const Key& key) {
        unique_lock<shared_mutex> lock(m_mtx);
        if (Node* f = buscar(key)) return f->m_data.m_value;
        m_tree.insert(Pair(key), Ref{});
        return buscar(key)->m_data.m_value;
    }

    const Value& at(const Key& key) const {
        shared_lock<shared_mutex> lock(m_mtx);
        Node* f = buscar(key);
        if (!f) throw out_of_range("key no existe");
        return f->m_data.m_value;
    }

    bool   contains(const Key& k) const { shared_lock<shared_mutex> lock(m_mtx); return buscar(k) != nullptr; }
    size_t size()    const { return m_tree.size(); }
    bool   isEmpty() const { return m_tree.isEmpty(); }

    //iiterador  for(const auto& [key, value] : table)
    auto begin() { return m_tree.begin(); }
    auto end()   { return m_tree.end();   }
    auto begin() const { return m_tree.begin(); }
    auto end()   const { return m_tree.end();   }

    //operator<<
    friend ostream& operator<<(ostream& os, const HashTable& t) {
        os << "{";
        bool first = true;
        t.m_tree.inorder().forEach([&](const Pair& p) {
            if (!first) os << ", ";
            os << p;
            first = false;
        });
        return os << "}";
    }

    //operator>>
    friend istream& operator>>(istream& is, HashTable& t) {
        is >> t.m_tree;
        return is;
    }

};

#endif // __HASHTABLE_H__