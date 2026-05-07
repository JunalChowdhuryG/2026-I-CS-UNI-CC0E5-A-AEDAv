#ifndef __ITERATOR_H__
#define __ITERATOR_H__
#include <algorithm>
#include <utility>

template <typename Container, class IteratorBase>
class general_iterator {
public:
    using Node       = typename Container::Node;
    using value_type = typename Container::value_type;
    using MySelf     = general_iterator<Container, IteratorBase>;

protected:
    Container *m_pContainer;
    Node      *m_pNode;

public:
    general_iterator(Container *pContainer, Node *pNode)
        : m_pContainer(pContainer), m_pNode(pNode) {}
    general_iterator(const MySelf &other)
        : m_pContainer(other.m_pContainer), m_pNode(other.m_pNode) {}
    general_iterator(MySelf &&other) {
        m_pContainer = std::move(other.m_pContainer);
        m_pNode      = std::move(other.m_pNode);
    }
    IteratorBase &operator=(IteratorBase &iter) {
        m_pContainer = std::move(iter.m_pContainer);
        m_pNode      = std::move(iter.m_pNode);
        return *(IteratorBase *)this;
    }

    Node *getNode() const { return m_pNode; }

    friend bool operator==(const IteratorBase &a, const IteratorBase &b) {
        return a.getNode() == b.getNode();
    }
    friend bool operator!=(const IteratorBase &a, const IteratorBase &b) {
        return a.getNode() != b.getNode();
    }

    // operator* devuelve value_type (dato) — compatible con el uso existente
    value_type &operator*() { return m_pNode->getDataRef(); }

    // operator++ por defecto: avanza con getNext()
    // CLL/CDLL lo redefinen para detectar la vuelta
    IteratorBase &operator++() {
        if (m_pNode) m_pNode = m_pNode->getNext();
        return *(IteratorBase *)this;
    }
};

#endif