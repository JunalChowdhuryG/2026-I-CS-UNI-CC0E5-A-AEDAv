#include "linkedlist.h"
#include <thread>
#include <vector>
#include <sstream>

using namespace std;

void TestBasicos() {
    cout << "\nTEST DE ESTRUCTURA LINKED LIST" << endl;
    LinkedList<DescendingLinkedListTrait<T1>> list;
    
    list.insert(2, 200);
    list.insert(1, 100);
    list.insert(3, 300);
    cout << "Original (Descendente): " << list << endl;

    // Probando Copy Constructor
    LinkedList<DescendingLinkedListTrait<T1>> listCopy(list);
    cout << "Copia profunda:         " << listCopy << endl;

    // Probando Move Constructor
    LinkedList<DescendingLinkedListTrait<T1>> listMove(std::move(list));
    cout << "Lista Movida:           " << listMove << endl;
    cout << "Original tras move:     " << list << " (Debe estar vacia por el std::exchange)" << endl;

    // Desempaquetamos los multiples parametros de retorno directamente
    auto [data_front, ref_front] = listMove.pop_front();
    cout << "\n[POP FRONT] Dato: " << data_front << " | Metadato (Ref): " << ref_front << endl;
    
    auto [data_back, ref_back] = listMove.pop_back();
    cout << "[POP BACK]  Dato: " << data_back << " | Metadato (Ref): " << ref_back << endl;
    
    cout << "Lista despues de pops:  " << listMove << endl;
}

void TestConcurrencia() {
    cout << "\nTEST DE CONCURRENCIA" << endl;
    LinkedList<AscendingLinkedListTrait<T1>> list;

    // 5 hilos van a intentar meter 1000 elementos cada uno al mismo tiempo
    auto worker = [&list](int thread_id) {
        for(int i = 0; i < 1000; i++) {
            list.push_front(i, thread_id);
        }
    };

    thread t1(worker, 1);
    thread t2(worker, 2);
    thread t3(worker, 3);
    thread t4(worker, 4);
    thread t5(worker, 5);

    t1.join(); t2.join(); t3.join(); t4.join(); t5.join();

    cout << "Se lanzaron 5 hilos insertando 1000 elementos simultaneamente." << endl;
    cout << "Tamano de la lista (Esperado 5000): " << list.size() << endl;
    if(list.size() == 5000) {
        cout << "ESTADO: EXITO - El shared_mutex previno condiciones de carrera perfectamente." << endl;
    } else {
        cout << "ESTADO: FALLO - Hubo corrupcion de memoria." << endl;
    }
}
void TestOperators() {
    cout << "\nTEST DE OPERADORES" << endl;
    LinkedList<AscendingLinkedListTrait<T1>> list;
    
    // 1. Probamos operator>> (Lectura)
    cout << "Simulando lectura desde formato: [(10, 100), (20, 200), (30, 300)]" << endl;
    stringstream simulador_input("[(10, 100), (20, 200), (30, 300)]");
    simulador_input >> list;

    // 2. Probamos operator<< (Escritura)
    cout << "Lista luego de la lectura (operator<<): " << list << endl;
    
    // 3. Probamos operator[] (Acceso seguro por indice)
    cout << "Accediendo al indice [0] (operator[]): Dato -> " << list[0] << endl;
    cout << "Accediendo al indice [2] (operator[]): Dato -> " << list[2] << endl;
    
    // Probamos la excepcion del operator[] (Descomentar para probar)
    // cout << "Probando fuera de rango: " << list[5] << endl; // Lanzara la excepcion
}
void ListsDemo(){
    TestBasicos();
    TestConcurrencia();
    TestOperators();
    cout << "\n=== FIN DE LAS PRUEBAS ===" << endl;
}