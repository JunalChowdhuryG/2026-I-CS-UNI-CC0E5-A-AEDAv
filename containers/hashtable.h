#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include "avl.h"
#include <tuple>
#include <utility>
using namespace std;

//KVPair
template <typename Key, typename Value>
struct KVPair {
    using value_type = KVPair;
    Key   m_key;
    mutable Value m_value;
    KVPair() : m_key(), m_value() {}
    KVPair(const Key& k, const Value& v = Value{}) : m_key(k), m_value(v) {}

    bool operator<(const KVPair& o)  const { return m_key <  o.m_key; }
    bool operator>(const KVPair& o)  const { return m_key >  o.m_key; }
    bool operator==(const KVPair& o) const { return m_key == o.m_key; }

    friend ostream& operator<<(ostream& os, const KVPair& p) { return os << p.m_key << ":" << p.m_value; }
    friend istream& operator>>(istream& is, KVPair& p) { Token c; return is >> p.m_key >> c >> p.m_value; }
};

//for (const auto& [k, v] : table)
namespace std {
    template<typename K,typename V> struct tuple_size<KVPair<K,V>>      : integral_constant<size_t,2>{};
    template<typename K,typename V> struct tuple_element<0,KVPair<K,V>> { using type = const K; };
    template<typename K,typename V> struct tuple_element<1,KVPair<K,V>> { using type = V; };
}
template<size_t I,typename K,typename V> decltype(auto) get(KVPair<K,V>& p)
    { if constexpr(I==0) return (const K&)p.m_key; else return p.m_value; }
template<size_t I,typename K,typename V> decltype(auto) get(const KVPair<K,V>& p)
    { if constexpr(I==0) return (const K&)p.m_key; else return (const V&)p.m_value; }

//HashTable
template <typename Key, typename Value>
class HashTable : public AVL<AscendingTrait<AVLNode<KVPair<Key,Value>>>> {
public:
    using Pair  = KVPair<Key,Value>;
    using Base  = AVL<AscendingTrait<AVLNode<Pair>>>;
    using Node  = typename Base::Node;

private:
    Node* buscar(const Key& key) const {
        Node* n = this->m_pRoot;
        while (n) {
            if (key == n->m_data.m_key) return n;
            n = static_cast<Node*>(n->m_pChild[key < n->m_data.m_key ? 0 : 1]);
        }
        return nullptr;
    }

public:
    HashTable() = default;
    Value& operator[](const Key& key) {
        unique_lock<shared_mutex> lock(this->m_mtx);
        if (Node* f = buscar(key)) return f->m_data.m_value;
        this->internal_insert(this->m_pRoot, Pair(key), Ref{});
        return buscar(key)->m_data.m_value;
    }

    const Value& at(const Key& key) const {
        shared_lock<shared_mutex> lock(this->m_mtx);
        Node* f = buscar(key);
        if (!f) throw out_of_range("key no existe");
        return f->m_data.m_value;
    }

    bool contains(const Key& key) const {
        shared_lock<shared_mutex> lock(this->m_mtx);
        return buscar(key) != nullptr;
    }
    //operator<<
    friend ostream& operator<<(ostream& os, const HashTable& t) {
        os << "{";
        bool first = true;
        t.inorder().forEach([&](const Pair& p) {
            if (!first) os << ", ";
            os << p;
            first = false;
        });
        return os << "}";
    }

    //operator>>{k:v, k:v}
    friend istream& operator>>(istream& is, HashTable& t) {
        Token ch;
        if (!(is >> ch) || ch != '{') { is.clear(ios_base::failbit); return is; }
        while (is >> ch && ch != '}') {
            if (ch == ',') continue;
            is.putback(ch);
            Key   key; Token sep;
            if (!(is >> key >> sep) || sep != ':') break;
            Value val{}; string raw;
            Token c;
            while (is.get(c) && c != ',' && c != '}') raw += c;
            auto start = raw.find_first_not_of(' ');
            if (start != string::npos) raw = raw.substr(start);
            val = Value(raw);
            t[key] = val;
            if (c == '}') break;
        }
        return is;
    }

    //begin/end
    auto begin() { return Base::begin(); }
    auto end()   { return Base::end();   }
};

#endif