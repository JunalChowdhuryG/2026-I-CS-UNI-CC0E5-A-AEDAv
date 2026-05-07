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

template <typename Container>
void DemoList(Container& list, string fileName) {
    list.insert(28, 15);
    list.insert(17, 25);
    list.insert(8, 35);
    list.insert(4, 45);
    list.insert(35, 55);
    cout << "Original:      " << list << endl;
    ofstream os(fileName);
    os << list << endl;
    os.close();
    
    Container listFromFile;
    ifstream  is(fileName);
    is >> listFromFile;
    cout << "Leida archivo: " << listFromFile << endl;
}

// Demo LinkedList
void LinkedListDemo() {
    LinkedList<DescendingLinkedListTrait<T1>> list;
    list.insert(2, 200); list.insert(1, 100); list.insert(3, 300);
    
    LinkedList<DescendingLinkedListTrait<T1>> listMove(std::move(list));
    auto [data_front, ref_front] = listMove.pop_front();
    cout << "[POP FRONT] Dato: " << data_front << " | Ref: " << ref_front << endl;

    cout << "\nCONCURRENCIA" << endl;
    LinkedList<AscendingLinkedListTrait<T1>> listconc;
    auto worker = [&listconc](int id) {
        for (int i = 0; i < 1000; i++) listconc.push_front(i, id);
    };
    thread t1(worker,1), t2(worker,2), t3(worker,3), t4(worker,4), t5(worker,5);
    t1.join(); t2.join(); t3.join(); t4.join(); t5.join();
    cout << "  Tamaño esperado 5000: " << listconc.size() << endl;

    cout << "\nESCRITURA/LECTURA ARCHIVOS" << endl;
    LinkedList<AscendingLinkedListTrait<T1>> list1;
    DemoList(list1, "AscLL.txt");
    LinkedList<DescendingLinkedListTrait<T1>> list2;
    DemoList(list2, "DescLL.txt");
}

// Demo DoubleLinkedList
void DoubleLinkedListDemo() {
    cout << "INSERCION ORDENADA" << endl;
    DoubleLinkedList<AscendingDLLTrait<T1>> asc;
    asc.insert(3,30); asc.insert(1,10); asc.insert(5,50);
    asc.insert(2,20); asc.insert(4,40);
    cout << "  Asc:  " << asc << endl;

    cout << "\nITERADOR BACKWARD" << endl;
    cout << "  bwd: ";
    for (auto it = asc.rbegin(); it != asc.rend(); ++it) {
        cout << *it << " ";
    }
    cout << endl;

    cout << "\nESCRITURA / LECTURA ARCHIVOS" << endl;
    DoubleLinkedList<AscendingDLLTrait<T1>> fileAsc;
    DemoList(fileAsc, "AscDLL.txt");
}

// Demo CircularLinkedList
void CircularLinkedListDemo() {
    cout << "1. INSERCION ORDENADA CIRCULAR" << endl;
    CircularLinkedList<AscendingCLLTrait<T1>> asc;
    asc.insert(3,30); asc.insert(1,10); asc.insert(5,50);
    asc.insert(2,20); asc.insert(4,40);
    cout << "  Asc:  " << asc << endl;

    cout << "\n2. NATURALEZA CIRCULAR" << endl;
    CircularLinkedList<AscendingCLLTrait<T1>> circList;
    circList.insert(1,10); circList.insert(2,20); circList.insert(3,30);
    cout << "  2 vueltas: ";
    circList.circularForEach(2, [](T1 &v){ cout << v << " "; });
    cout << endl;

    cout << "\n3. ESCRITURA / LECTURA ARCHIVOS" << endl;
    CircularLinkedList<AscendingCLLTrait<T1>> fileAsc;
    DemoList(fileAsc, "AscCLL.txt");
}

// Demo CircularDoubleLinkedList
void CircularDoubleLinkedListDemo() {
    cout << "1. INSERCION ORDENADA DOBLE CIRCULAR" << endl;
    CircularDoubleLinkedList<AscendingCDLLTrait<T1>> asc;
    asc.insert(3,30); asc.insert(1,10); asc.insert(5,50);
    asc.insert(2,20); asc.insert(4,40);
    cout << "  Asc:  " << asc << endl;
    cout << "\n2. NATURALEZA CIRCULAR DOBLE 2 vueltas" << endl;
    CircularDoubleLinkedList<AscendingCDLLTrait<T1>> circList;
    circList.insert(1,10); circList.insert(2,20); circList.insert(3,30);
    cout << "  fwd x2: ";
    circList.circularForEach(2, 1, [](T1 &v){ cout << v << " "; });
    cout << endl;
    
    cout << "  bwd x2: ";
    circList.circularForEach(2, -1, [](T1 &v){ cout << v << " "; });
    cout << endl;

    cout << "\n3. ESCRITURA / LECTURA ARCHIVOS" << endl;
    CircularDoubleLinkedList<AscendingCDLLTrait<T1>> fileAsc;
    DemoList(fileAsc, "AscCDLL.txt");
}

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