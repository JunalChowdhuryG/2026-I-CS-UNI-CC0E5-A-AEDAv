#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

#include "../types.h"
#include "linkedlist.h"
#include "doublelinkedlist.h"
#include "circularlinkedlist.h"
#include "circulardoublelinkedlist.h"

using namespace std;

// ─── DemoList ─────────────────────────────────────────────────────────────────
// Demuestra reutilizacion de operator<< y operator>>:
// Ambos estan definidos UNA sola vez en LinkedList y funcionan para
// LinkedList, DoubleLinkedList, CircularLinkedList y CircularDoubleLinkedList
// sin ningun cambio. operator>> llama a insert() virtual, por lo que cada
// hijo aplica su propio orden y manejo de circularidad.
template <typename Container>
void DemoList(Container& list, string fileName) {
    list.insert(28, 15);
    list.insert(17, 25);
    list.insert(8,  35);
    list.insert(4,  45);
    list.insert(35, 55);

    cout << "  Original (operator<< heredado):      " << list << endl;

    ofstream os(fileName);
    os << list << endl;   // operator<< heredado de LinkedList
    os.close();

    Container listFromFile;
    ifstream  is(fileName);
    is >> listFromFile;   // operator>> heredado de LinkedList, usa insert() virtual
    cout << "  Leida archivo (operator>> heredado): " << listFromFile << endl;
}

// ─── LinkedListDemo ───────────────────────────────────────────────────────────
void LinkedListDemo() {
    // Reutilizacion de nodo: LLNode<T1> via AscendingTrait / DescendingTrait
    LinkedList<DescendingTrait<LLNode<T1>>> list;
    list.insert(2, 200); list.insert(1, 100); list.insert(3, 300);

    LinkedList<DescendingTrait<LLNode<T1>>> listMove(std::move(list));
    auto [data_front, ref_front] = listMove.pop_front();
    cout << "  [POP FRONT] Dato: " << data_front << " | Ref: " << ref_front << endl;

    // Reutilizacion de iterador: LinkedListForwardIterator hereda de general_iterator
    cout << "  Iterador forward (general_iterator): ";
    LinkedList<AscendingTrait<LLNode<T1>>> listIt;
    listIt.insert(10, 1); listIt.insert(20, 2); listIt.insert(30, 3);
    for (auto it = listIt.begin(); it != listIt.end(); ++it)
        cout << *it << " ";
    cout << endl;

    cout << "\n  CONCURRENCIA" << endl;
    LinkedList<AscendingTrait<LLNode<T1>>> listconc;
    auto worker = [&listconc](int id) {
        for (int i = 0; i < 1000; i++) listconc.push_front(i, id);
    };
    thread t1(worker,1), t2(worker,2), t3(worker,3), t4(worker,4), t5(worker,5);
    t1.join(); t2.join(); t3.join(); t4.join(); t5.join();
    cout << "  Tamaño esperado 5000: " << listconc.size() << endl;

    cout << "\n  ESCRITURA/LECTURA ARCHIVOS (operator<< y >> heredados)" << endl;
    LinkedList<AscendingTrait<LLNode<T1>>>  list1;
    DemoList(list1, "AscLL.txt");
    LinkedList<DescendingTrait<LLNode<T1>>> list2;
    DemoList(list2, "DescLL.txt");
}

// ─── DoubleLinkedListDemo ─────────────────────────────────────────────────────
void DoubleLinkedListDemo() {
    // Reutilizacion de nodo: DLLNode<T1> extiende LLNode<T1> agregando solo m_prev
    // AscendingTrait<DLLNode<T1>> deduce value_type desde DLLNode::value_type
    cout << "  INSERCION ORDENADA (DLLNode reutiliza LLNode + agrega m_prev)" << endl;
    DoubleLinkedList<AscendingTrait<DLLNode<T1>>> asc;
    asc.insert(3,30); asc.insert(1,10); asc.insert(5,50);
    asc.insert(2,20); asc.insert(4,40);
    cout << "  Asc:  " << asc << endl;

    // Reutilizacion de iterador: DLLBackwardIterator hereda de general_iterator,
    // solo cambia operator++ para usar getPrev() en lugar de getNext()
    cout << "\n  ITERADOR BACKWARD (DLLBackwardIterator hereda general_iterator)" << endl;
    cout << "  bwd: ";
    for (auto it = asc.rbegin(); it != asc.rend(); ++it)
        cout << *it << " ";
    cout << endl;

    cout << "\n  ESCRITURA / LECTURA ARCHIVOS (operator<< y >> heredados)" << endl;
    DoubleLinkedList<AscendingTrait<DLLNode<T1>>> fileAsc;
    DemoList(fileAsc, "AscDLL.txt");
}

// ─── CircularLinkedListDemo ───────────────────────────────────────────────────
void CircularLinkedListDemo() {
    // Reutilizacion de nodo: CLL usa LLNode<T1> — el MISMO nodo que LinkedList,
    // sin agregar ningun campo. Solo cambia como se conectan los punteros.
    cout << "  INSERCION ORDENADA (LLNode<T1> reutilizado desde LinkedList)" << endl;
    CircularLinkedList<AscendingTrait<LLNode<T1>>> asc;
    asc.insert(3,30); asc.insert(1,10); asc.insert(5,50);
    asc.insert(2,20); asc.insert(4,40);
    cout << "  Asc:  " << asc << endl;

    // Reutilizacion de iterador: CLLForwardIterator hereda circular_iterator
    // que hereda general_iterator. Solo redefine operator++ para detectar
    // la vuelta completa (cuando next == root) y poner m_pNode = nullptr.
    cout << "\n  ITERADOR CIRCULAR (CLLForwardIterator hereda circular_iterator)" << endl;
    cout << "  Una vuelta con cbegin()/cend(): ";
    for (auto it = asc.cbegin(); it != asc.cend(); ++it)
        cout << *it << " ";
    cout << endl;

    cout << "\n  NATURALEZA CIRCULAR (circularForEach 2 vueltas)" << endl;
    CircularLinkedList<AscendingTrait<LLNode<T1>>> circList;
    circList.insert(1,10); circList.insert(2,20); circList.insert(3,30);
    cout << "  2 vueltas: ";
    circList.circularForEach(2, [](T1 &v){ cout << v << " "; });
    cout << endl;

    cout << "\n  ESCRITURA / LECTURA ARCHIVOS (operator<< y >> heredados)" << endl;
    CircularLinkedList<AscendingTrait<LLNode<T1>>> fileAsc;
    DemoList(fileAsc, "AscCLL.txt");
}

// ─── CircularDoubleLinkedListDemo ─────────────────────────────────────────────
void CircularDoubleLinkedListDemo() {
    // Reutilizacion de nodo: CDLL usa DLLNode<T1> — el MISMO nodo que
    // DoubleLinkedList, con m_prev. No agrega ningun campo nuevo.
    cout << "  INSERCION ORDENADA (DLLNode<T1> reutilizado desde DoubleLinkedList)" << endl;
    CircularDoubleLinkedList<AscendingTrait<DLLNode<T1>>> asc;
    asc.insert(3,30); asc.insert(1,10); asc.insert(5,50);
    asc.insert(2,20); asc.insert(4,40);
    cout << "  Asc:  " << asc << endl;

    // Reutilizacion de iterador: CDLLForwardIterator y CDLLBackwardIterator
    // heredan circular_iterator. El backward solo cambia operator++ para
    // usar getPrev() con deteccion de vuelta circular.
    cout << "\n  ITERADOR FORWARD CIRCULAR (una vuelta con cbegin()/cend())" << endl;
    cout << "  fwd: ";
    for (auto it = asc.cbegin(); it != asc.cend(); ++it)
        cout << *it << " ";
    cout << endl;

    cout << "\n  ITERADOR BACKWARD CIRCULAR (una vuelta con crbegin()/crend())" << endl;
    cout << "  bwd: ";
    for (auto it = asc.crbegin(); it != asc.crend(); ++it)
        cout << *it << " ";
    cout << endl;

    cout << "\n  NATURALEZA CIRCULAR DOBLE (circularForEach 2 vueltas)" << endl;
    CircularDoubleLinkedList<AscendingTrait<DLLNode<T1>>> circList;
    circList.insert(1,10); circList.insert(2,20); circList.insert(3,30);
    cout << "  fwd x2: ";
    circList.circularForEach(2,  1, [](T1 &v){ cout << v << " "; });
    cout << endl;
    cout << "  bwd x2: ";
    circList.circularForEach(2, -1, [](T1 &v){ cout << v << " "; });
    cout << endl;

    cout << "\n  ESCRITURA / LECTURA ARCHIVOS (operator<< y >> heredados)" << endl;
    CircularDoubleLinkedList<AscendingTrait<DLLNode<T1>>> fileAsc;
    DemoList(fileAsc, "AscCDLL.txt");
}

// ─── ListsDemo ────────────────────────────────────────────────────────────────
void ListsDemo() {
    cout << "=============================" << endl;
    cout << "PRUEBAS LINKEDLIST" << endl;
    cout << "=============================" << endl;
    LinkedListDemo();

    cout << "\n=============================" << endl;
    cout << "PRUEBAS DOUBLELINKEDLIST" << endl;
    cout << "=============================" << endl;
    DoubleLinkedListDemo();

    cout << "\n=============================" << endl;
    cout << "PRUEBAS CIRCULARLINKEDLIST" << endl;
    cout << "=============================" << endl;
    CircularLinkedListDemo();

    cout << "\n=============================" << endl;
    cout << "PRUEBAS CIRCULARDOUBLELINKEDLIST" << endl;
    cout << "=============================" << endl;
    CircularDoubleLinkedListDemo();

    cout << "\nFIN DE LAS PRUEBAS" << endl;
}