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

// Prueba generica: Funciona para cualquier contenedor que tenga insert operator<< y operator>>
template <typename Container>
void DemoList(Container& list, string fileName){
    list.insert(28, 15);
    list.insert(17, 25);
    list.insert(8,35);
    list.insert(4,45);
    list.insert(35,55);
    cout << "Original:      " << list << endl;
    ofstream os(fileName);
    os << list << endl;
    os.close();
    Container listFromFile;
    ifstream  is(fileName);
    is >> listFromFile;
    cout << "Leida archivo: " << listFromFile << endl;
}

//demo linkedlist
void LinkedListDemo()
{
    //move constructor
    LinkedList<DescendingLinkedListTrait<T1>> list;
    list.insert(2, 200); list.insert(1, 100); list.insert(3, 300);
    LinkedList<DescendingLinkedListTrait<T1>> listMove(std::move(list));
    auto [data_front, ref_front] = listMove.pop_front();
    cout << "[POP FRONT] Dato: " << data_front
         << " | Ref: "           << ref_front << endl;

    //concurrencia
    cout << "\nCONCURRENCIA" << endl;
    LinkedList<AscendingLinkedListTrait<T1>> listconc;
    auto worker = [&listconc](int id) {
        for (int i = 0; i < 1000; i++) listconc.push_front(i, id);
    };
    thread t1(worker,1), t2(worker,2), t3(worker,3), t4(worker,4), t5(worker,5);
    t1.join(); t2.join(); t3.join(); t4.join(); t5.join();
    cout << "  Tamano esperado 5000: " << listconc.size() << endl;
    cout << "  ESTADO: " << (listconc.size()==5000 ? "EXITO" : "FALLO") << endl;

    // Archivos
    cout << "\nESCRITURA/LECTURA ARCHIVOS" << endl;
    LinkedList<AscendingLinkedListTrait<T1>>  list1;
    DemoList(list1, "AscLL.txt");
    LinkedList<DescendingLinkedListTrait<T1>> list2;
    DemoList(list2, "DescLL.txt");

    // Operadores
    cout << "\nOPERADORES" << endl;
    LinkedList<AscendingLinkedListTrait<T1>> listOp;
    stringstream ss("[(10, 100), (20, 200), (30, 300)]");
    ss >> listOp;
    cout << "  operator>>: " << listOp << endl;
    cout << "  operator[] [0]=" << listOp[0] << " [2]=" << listOp[2] << endl;
}

// demo doublelinkedlist
void DoubleLinkedListDemo(){
    //insercion ordenada
    cout << "INSERCION ORDENADA" << endl;
    DoubleLinkedList<AscendingDLLTrait<T1>> asc;
    asc.insert(3,30); asc.insert(1,10); asc.insert(5,50);
    asc.insert(2,20); asc.insert(4,40);
    cout << "  Asc:  " << asc  << endl;

    DoubleLinkedList<DescendingDLLTrait<T1>> desc;
    desc.insert(3,30); desc.insert(1,10); desc.insert(5,50);
    desc.insert(2,20); desc.insert(4,40);
    cout << "  Desc: " << desc << endl;

    //push front / push back
    cout << "\n2. PUSH FRONT / PUSH BACK" << endl;
    DoubleLinkedList<AscendingDLLTrait<T1>> pushList;
    pushList.push_back(20,2); pushList.push_back(30,3);
    pushList.push_front(10,1); pushList.push_front(5,0);
    cout << "  Luego de pushes: " << pushList << endl;

    //pop front / pop back 
    cout << "\n3. POP FRONT / POP BACK" << endl;
    auto [d1,r1] = pushList.pop_front();
    cout << "  pop_front -> (" << d1 << "," << r1 << ") | lista: " << pushList << endl;
    auto [d2,r2] = pushList.pop_back();
    cout << "  pop_back  -> (" << d2 << "," << r2 << ") | lista: " << pushList << endl;

    //iterador forward
    cout << "\n4. ITERADOR FORWARD" << endl;
    DoubleLinkedList<AscendingDLLTrait<T1>> iterList;
    iterList.insert(1,10); iterList.insert(2,20); iterList.insert(3,30);
    iterList.insert(4,40); iterList.insert(5,50);
    cout << "  fwd: ";
    for (auto &v : iterList) cout << v << " ";
    cout << endl;

    //iterador backward
    cout << "\n5. ITERADOR BACKWARD" << endl;
    cout << "  bwd: ";
    for (auto it = iterList.rbegin(); it != iterList.rend(); ++it)
        cout << *it << " ";
    cout << endl;

    //foreach yy reverseforeach
    cout << "\nFOREACH / REVERSEFOREACH" << endl;
    cout << "  ForEach fwd:        ";
    iterList.ForEach([](T1 &v){ cout << v << " "; });
    cout << endl;
    cout << "  ReverseForEach bwd: ";
    iterList.ReverseForEach([](T1 &v){ cout << v << " "; });
    cout << endl;

    //operator[]
    cout << "\n7. OPERATOR[]" << endl;
    cout << "  [0]=" << iterList[0] << " [2]=" << iterList[2]
         << " [4]=" << iterList[4] << endl;

    //copy constructor
    cout << "\n8. COPY CONSTRUCTOR" << endl;
    DoubleLinkedList<AscendingDLLTrait<T1>> copied(iterList);
    cout << "  Original: " << iterList << endl;
    cout << "  Copiada:  " << copied   << endl;

    //move constructor
    cout << "\n9. MOVE CONSTRUCTOR" << endl;
    DoubleLinkedList<AscendingDLLTrait<T1>> moved(std::move(copied));
    cout << "  Moved: " << moved << endl;
    cout << "  Original tras move (size=0): " << copied.size() << endl;

    //archivos
    cout << "\n10. ESCRITURA / LECTURA ARCHIVOS" << endl;
    DoubleLinkedList<AscendingDLLTrait<T1>>  fileAsc;
    DemoList(fileAsc,  "AscDLL.txt");
    DoubleLinkedList<DescendingDLLTrait<T1>> fileDesc;
    DemoList(fileDesc, "DescDLL.txt");

    //operator>>
    cout << "\n11. OPERATOR>>" << endl;
    DoubleLinkedList<AscendingDLLTrait<T1>> streamList;
    stringstream ss("[(30, 3), (10, 1), (20, 2)]");
    ss >> streamList;
    cout << "  Stream desordenado -> lista: " << streamList << endl;

    //concurrencia
    cout << "\n12. CONCURRENCIA (5 hilos x 1000 push_front)" << endl;
    DoubleLinkedList<AscendingDLLTrait<T1>> concList;
    auto worker = [&concList](int id){
        for (int i = 0; i < 1000; i++) concList.push_front(i, id);
    };
    thread t1(worker,1), t2(worker,2), t3(worker,3), t4(worker,4), t5(worker,5);
    t1.join(); t2.join(); t3.join(); t4.join(); t5.join();
    cout << "  Tamano esperado 5000: " << concList.size() << endl;
}

//demo circularlinkedlist
void CircularLinkedListDemo(){
    //insercion ordenada
    cout << "1. INSERCION ORDENADA" << endl;
    CircularLinkedList<AscendingCLLTrait<T1>> asc;
    asc.insert(3,30); asc.insert(1,10); asc.insert(5,50);asc.insert(2,20);asc.insert(4,40);
    cout << "  Asc:  " << asc  << endl;

    CircularLinkedList<DescendingCLLTrait<T1>> desc;
    desc.insert(3,30); desc.insert(1,10); desc.insert(5,50);
    desc.insert(2,20); desc.insert(4,40);
    cout << "  Desc: " << desc << endl;

    //push front ,  push back , pop front,  pop_back
    cout << "\n2. PUSH FRONT / PUSH BACK / POP FRONT / POP BACK" << endl;
    CircularLinkedList<AscendingCLLTrait<T1>> pushList;
    pushList.push_back(20,2);pushList.push_back(30,3);pushList.push_front(10,1);pushList.push_front(5,0);
    cout << "  Despues de pushes: " << pushList << endl;
    auto [d1,r1] = pushList.pop_front();
    cout << "  pop_front -> (" << d1 << "," << r1 << ") | lista: " << pushList << endl;
    auto [d2,r2] = pushList.pop_back();
    cout << "  pop_back  -> (" << d2 << "," << r2 << ") | lista: " << pushList << endl;

    //naturaleza circular y circularForEach N vueltas
    cout << "\n3. NATURALEZA CIRCULAR circularForEach xx2 vueltas" << endl;
    CircularLinkedList<AscendingCLLTrait<T1>> circList;
    circList.insert(1,10);circList.insert(2,20);circList.insert(3,30);
    cout << "  Lista: " << circList << endl;
    cout << "  x2 vueltas: ";
    circList.circularForEach(2, [](T1 &v){ cout << v << " "; });
    cout << endl;

    //iterador forward
    cout << "\n4. ITERADOR FORWARD" << endl;
    CircularLinkedList<AscendingCLLTrait<T1>> iterList;
    iterList.insert(1,10); iterList.insert(2,20); iterList.insert(3,30);
    iterList.insert(4,40); iterList.insert(5,50);
    cout << "  ranged-for (1 vuelta exacta): ";
    for (auto &v : iterList) cout << v << " ";
    cout << endl;

    //forEach
    cout << "\n5. FOREACH" << endl;
    cout << "  ForEach: ";
    iterList.ForEach([](T1 &v){ cout << v << " "; });
    cout << endl;

    //operator[]
    cout << "\n6. OPERATOR[]" << endl;
    cout << "  [0]=" << iterList[0] << " [2]=" << iterList[2]
         << " [4]=" << iterList[4] << endl;

    //copy constructor y move constructor
    cout << "\n7. COPY / MOVE CONSTRUCTORS" << endl;
    CircularLinkedList<AscendingCLLTrait<T1>> orig;
    orig.insert(10,1); orig.insert(20,2); orig.insert(30,3);
    CircularLinkedList<AscendingCLLTrait<T1>> copied(orig);
    cout << "  Orig:   " << orig   << endl;
    cout << "  Copied: " << copied << endl;
    cout << "  Copied x2 vueltas: ";
    copied.circularForEach(2, [](T1 &v){ cout << v << " "; });
    cout << endl;
    CircularLinkedList<AscendingCLLTrait<T1>> moved(std::move(orig));
    cout << "  Moved:  " << moved << endl;
    cout << "  Orig tras move (size=0): " << orig.size() << endl;

    //archivos
    cout << "\n8. ESCRITURA / LECTURA ARCHIVOS" << endl;
    CircularLinkedList<AscendingCLLTrait<T1>>  fileAsc;
    DemoList(fileAsc,  "AscCLL.txt");
    CircularLinkedList<DescendingCLLTrait<T1>> fileDesc;
    DemoList(fileDesc, "DescCLL.txt");

    //operator>>
    cout << "\n9. OPERATOR>> DESDE STREAM (respeta Comp)" << endl;
    CircularLinkedList<AscendingCLLTrait<T1>> streamList;
    stringstream ss("[(30, 3), (10, 1), (20, 2)]");
    ss >> streamList;
    cout << "  Stream desordenado -> lista: " << streamList << endl;

    //concurrencia
    cout << "\n10. CONCURRENCIA (5 hilos x 1000 push_front)" << endl;
    CircularLinkedList<AscendingCLLTrait<T1>> concList;
    auto worker = [&concList](int id){
        for (int i = 0; i < 1000; i++) concList.push_front(i, id);
    };
    thread t1(worker,1), t2(worker,2), t3(worker,3), t4(worker,4), t5(worker,5);
    t1.join(); t2.join(); t3.join(); t4.join(); t5.join();
    cout << "  Tamano esperado 5000: " << concList.size() << endl;
    
}

//demo circulardoublelinkedlist
void CircularDoubleLinkedListDemo(){
    //insercion ordenada
    cout << "1. INSERCION ORDENADA" << endl;
    CircularDoubleLinkedList<AscendingCDLLTrait<T1>> asc;
    asc.insert(3,30); asc.insert(1,10); asc.insert(5,50);
    asc.insert(2,20); asc.insert(4,40);
    cout << "  Asc:  " << asc  << endl;

    CircularDoubleLinkedList<DescendingCDLLTrait<T1>> desc;
    desc.insert(3,30); desc.insert(1,10); desc.insert(5,50);
    desc.insert(2,20); desc.insert(4,40);
    cout << "  Desc: " << desc << endl;

    //push front , push back,pop front, pop back
    cout << "\n2. PUSH FRONT / PUSH BACK / POP FRONT / POP BACK" << endl;
    CircularDoubleLinkedList<AscendingCDLLTrait<T1>> pushList;
    pushList.push_back(20,2); pushList.push_back(30,3);
    pushList.push_front(10,1); pushList.push_front(5,0);
    cout << "  Despues de pushes: " << pushList << endl;
    auto [d1,r1] = pushList.pop_front();
    cout << "  pop_front -> (" << d1 << "," << r1 << ") | lista: " << pushList << endl;
    auto [d2,r2] = pushList.pop_back();
    cout << "  pop_back  -> (" << d2 << "," << r2 << ") | lista: " << pushList << endl;

    //fwd y bwd en N vueltas
    cout << "\n3. NATURALEZA CIRCULAR DOBLE (fwd y bwd x2 vueltas)" << endl;
    CircularDoubleLinkedList<AscendingCDLLTrait<T1>> circList;
    circList.insert(1,10); circList.insert(2,20); circList.insert(3,30);
    cout << "  Lista: " << circList << endl;
    cout << "  fwd x2: ";
    circList.circularForEach(2,  1, [](T1 &v){ cout << v << " "; });
    cout << endl;
    cout << "  bwd x2: ";
    circList.circularForEach(2, -1, [](T1 &v){ cout << v << " "; });
    cout << endl;

    //iteradores forward y backward
    cout << "\n4. ITERADORES FORWARD Y BACKWARD (ranged-for)" << endl;
    CircularDoubleLinkedList<AscendingCDLLTrait<T1>> iterList;
    iterList.insert(1,10); iterList.insert(2,20); iterList.insert(3,30);
    iterList.insert(4,40); iterList.insert(5,50);
    cout << "  ranged-for fwd: ";
    for (auto &v : iterList) cout << v << " ";
    cout << endl;
    cout << "  ranged-for bwd: ";
    for (auto it = iterList.rbegin(); it != iterList.rend(); ++it)
        cout << *it << " ";
    cout << endl;

    //foreach y reverseforeach
    cout << "\n5. FOREACH / REVERSEFOREACH CON LOCK" << endl;
    cout << "  ForEach fwd:        ";
    iterList.ForEach([](T1 &v){ cout << v << " "; });
    cout << endl;
    cout << "  ReverseForEach bwd: ";
    iterList.ReverseForEach([](T1 &v){ cout << v << " "; });
    cout << endl;

    //operator[]
    cout << "\n6. OPERATOR[]" << endl;
    cout << "  [0]=" << iterList[0] << " [2]=" << iterList[2]
         << " [4]=" << iterList[4] << endl;

    //copy constructor move constructor
    cout << "\n7. COPY / MOVE CONSTRUCTORS" << endl;
    CircularDoubleLinkedList<AscendingCDLLTrait<T1>> orig;
    orig.insert(10,1); orig.insert(20,2); orig.insert(30,3);
    CircularDoubleLinkedList<AscendingCDLLTrait<T1>> copied(orig);
    cout << "  Orig:   " << orig   << endl;
    cout << "  Copied: " << copied << endl;
    cout << "  Copied fwd x2: ";
    copied.circularForEach(2,  1, [](T1 &v){ cout << v << " "; });
    cout << endl;
    cout << "  Copied bwd x2: ";
    copied.circularForEach(2, -1, [](T1 &v){ cout << v << " "; });
    cout << endl;
    CircularDoubleLinkedList<AscendingCDLLTrait<T1>> moved(std::move(orig));
    cout << "  Moved:  " << moved << endl;
    cout << "  Orig tras move (size=0): " << orig.size() << endl;

    //archivos
    cout << "\n8. ESCRITURA / LECTURA ARCHIVOS" << endl;
    CircularDoubleLinkedList<AscendingCDLLTrait<T1>>  fileAsc;
    DemoList(fileAsc,  "AscCDLL.txt");
    CircularDoubleLinkedList<DescendingCDLLTrait<T1>> fileDesc;
    DemoList(fileDesc, "DescCDLL.txt");

    //operator>>
    cout << "\n9. OPERATOR>> DESDE STREAM" << endl;
    CircularDoubleLinkedList<AscendingCDLLTrait<T1>> streamList;
    stringstream ss("[(30, 3), (10, 1), (20, 2)]");
    ss >> streamList;
    cout << "  Stream desordenado -> lista: " << streamList << endl;

    //doncurrencia
    cout << "\n10. CONCURRENCIA" << endl;
    CircularDoubleLinkedList<AscendingCDLLTrait<T1>> concList;
    auto worker = [&concList](int id){
        for (int i = 0; i < 1000; i++) concList.push_front(i, id);
    };
    thread t1(worker,1), t2(worker,2), t3(worker,3), t4(worker,4), t5(worker,5);
    t1.join(); t2.join(); t3.join(); t4.join(); t5.join();
    cout << "  Tamano esperado 5000: " << concList.size() << endl;
}

void ListsDemo()
{
    cout << "pruebas linkedlist" << endl;
    LinkedListDemo();
    cout << "\n-----------------------------\n" << endl;
    cout << "pruebas doublelinkedlist" << endl;
    DoubleLinkedListDemo();
    cout << "\n-----------------------------\n" << endl;
    cout << "pruebas circularlinkedlist" << endl;
    CircularLinkedListDemo();
    cout << "\n-----------------------------\n" << endl;
    cout << "pruebas circulardoublelinkedlist" << endl;
    CircularDoubleLinkedListDemo();

    cout << "\nFIN DE LAS PRUEBAS" << endl;
}