#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

#include "../types.h"
#include "linkedlist.h"
// #include "doublelinkedlist.h"
// #include "circularlinkedlist.h"
// #include "circulardoublelinkedlist.h"

using namespace std;

template <typename Container>
void DemoList(Container& list, string fileName){
    list.insert(28, 15);
    list.insert(17, 25);
    list.insert(8, 35);
    list.insert(4, 45);
    list.insert(35, 55);
    cout << "Lista Original: " << list << endl;
    ofstream os(fileName);
    os << list << endl;
    os.close();
    Container listFromFile;
    ifstream is(fileName);
    is >> listFromFile;
    cout << "Lista Leida de Archivo: " << listFromFile << endl;
}

void LinkedListDemo(){
    
    LinkedList<DescendingLinkedListTrait<T1>> list;
    list.insert(2, 200);
    list.insert(1, 100);
    list.insert(3, 300);
    LinkedList<DescendingLinkedListTrait<T1>> listMove(std::move(list));
    auto [data_front, ref_front] = listMove.pop_front();
    cout << "[POP FRONT EXITOSO] Dato: " << data_front << " | Metadato (Ref): " << ref_front << endl;


    cout << "\nTEST DE CONCURRENCIA" << endl;
    LinkedList<AscendingLinkedListTrait<T1>> listconc;
    auto worker = [&listconc](int thread_id) {
        for(int i = 0; i < 1000; i++) {
            listconc.push_front(i, thread_id);
        }
    }; 
    thread t1(worker, 1);
    thread t2(worker, 2);
    thread t3(worker, 3);
    thread t4(worker, 4);
    thread t5(worker, 5);
    t1.join(); t2.join(); t3.join(); t4.join(); t5.join();
    cout << "Se lanzaron 5 hilos insertando 1000 elementos simultaneamente." << endl;
    cout << "Tamano de la lista (Esperado 5000): " << listconc.size() << endl;
    if(listconc.size() == 5000) {
        cout << "ESTADO: EXITO - El shared_mutex previno condiciones de carrera." << endl;
    } else {
        cout << "ESTADO: FALLO - Hubo corrupcion de memoria." << endl;
    }


    cout << "\nESCRITURA/LECTURA ARCHIVOS" << endl;
    LinkedList<AscendingLinkedListTrait<T1>> list1;
    DemoList(list1, "AscLL.txt");
    LinkedList<DescendingLinkedListTrait<T1>> list2;
    DemoList(list2, "DescLL.txt");


    cout << "\nTEST DE OPERADORES" << endl;
    LinkedList<AscendingLinkedListTrait<T1>> listOp;
    cout << "Simulando lectura: [(10, 100), (20, 200), (30, 300)]" << endl;
    stringstream simulador_input("[(10, 100), (20, 200), (30, 300)]");
    simulador_input >> listOp;
    cout << "Lista luego de la lectura (operator<<): " << listOp << endl;
    cout << "Accediendo al indice [0] (operator[]): Dato -> " << listOp[0] << endl;
    cout << "Accediendo al indice [2] (operator[]): Dato -> " << listOp[2] << endl;
}

void DoubleLinkedListDemo(){
    //Completar
}

void CircularLinkedListDemo(){
    //Completar
}

void CircularDoubleLinkedListDemo(){
    //Completar
}


void ListsDemo(){
    cout << "\n===Pruebas de LinkedList===\n" << endl;
    LinkedListDemo();

    cout << "\n===Pruebas de DoubleLinkedList===\n" << endl;
    DoubleLinkedListDemo();

    cout << "\n===Pruebas de CircularLinkedList===\n" << endl;
    CircularLinkedListDemo();

    cout << "\n===Pruebas de CircularDoubleLinkedList===\n" << endl;
    CircularDoubleLinkedListDemo();

    cout << "\nFIN DE LAS PRUEBAS" << endl;
}