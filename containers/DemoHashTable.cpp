#include <iostream>
#include <sstream>
#include "../types.h"
#include "hashtable.h"
#include "DemoUtils.h"
using namespace std;

//KVTrait
using HT = HashTable<KVTrait<T1, string>>;

void DemoHashTable() {
    printHeader("HASHTABLE");

    //operator[]
    printSection("operator[]");
    HT m;
    m[10] = "Diez";
    m[25] = "Veinticinco";
    m[3]  = "Tres";
    m[10] = "Diez Modificado";   //actualiza no duplicar
    cout << "  m[10] = " << m[10] << "\n";
    cout << "  m[25] = " << m[25] << "\n";
    cout << "  m[3]  = " << m[3]  << "\n";
    cout << "  size  = " << m.size() << " (esperado 3, no 4)\n";

    //for (const auto& [key, value] : m)
    for (const auto& [key, value] : m)
        cout << "  key=" << key << "  value=" << value << "\n";

    //operator<<
    printSection("operator<<");
    cout << "  " << m << "\n";

    //operator>>
    printSection("operator>>");
    ostringstream oss;
    oss << m;
    HT m2;
    istringstream iss(oss.str());
    iss >> m2;
    cout << "  serializado  : " << oss.str() << "\n";
    cout << "  deserializado: " << m2 << "\n";
    cout << "  sizes iguales: " << (m.size() == m2.size() ? "SI" : "NO") << "\n";

    //Copy constructor
    printSection("Copy constructor");
    HT copia(m);
    copia[99] = "NuevoEnCopia";
    cout << "  original (sin 99) : size=" << m.size()     << "  " << m    << "\n";
    cout << "  copia    (con 99) : size=" << copia.size() << "  " << copia << "\n";
    cout << "  independientes    : " << (!m.contains(99) ? "SI" : "NO") << "\n";

    //Move constructor
    printSection("Move constructor");
    HT movida(move(copia));
    cout << "  movida           : size=" << movida.size() << "  " << movida << "\n";
    cout << "  fuente tras move : size=" << copia.size()  << " (esperado 0)\n";

    printFooter("HASHTABLE");
}