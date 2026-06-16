#ifndef __DEMO_UTILS_H__
#define __DEMO_UTILS_H__

#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <string>
using namespace std;

inline void printSection(const string& title) {
    cout << "\n[" << title << "]\n";
}
inline void printHeader(const string& name) {
    cout << "\n             PRUEBAS " << name << "             \n";
}
inline void printFooter(const string& name) {
    cout << "\n             FIN " << name << "             \n";
}

//copy / Move
template<typename Container, typename InsertFn>
void testCopyMove(Container& original, InsertFn insertExtra) {
    printSection("Copy constructor");
    Container copia(original);
    insertExtra(copia);
    cout << "  Original : size=" << original.size() << "  " << original << "\n";
    cout << "  Copia    : size=" << copia.size()    << "  " << copia    << "\n";

    printSection("Move constructor");
    Container movida(std::move(copia));
    cout << "  Movida   : size=" << movida.size() << "  " << movida << "\n";
    cout << "  Fuente tras move: size=" << copia.size() << "\n";
}

//operator<< operator>>
template<typename Container>
void testIO(const Container& c) {
    printSection("operator<< / operator>>");
    ostringstream oss;
    oss << c;
    cout << "  Serializado : " << oss.str() << "\n";

    Container c2;
    istringstream iss(oss.str());
    iss >> c2;
    cout << "  Deserializado: " << c2 << "\n";
}

//concurrencia
//N hillos ejecutando y verifica tamaño final sea esperado
template<typename Container, typename WorkerFn>
void testConcurrency(Container& c, WorkerFn workerFn,
                     size_t nThreads, size_t expectedSize,
                     const string& label = "size") {
    printSection("Concurrencia");
    vector<thread> threads;
    threads.reserve(nThreads);
    for (size_t i = 0; i < nThreads; ++i)
        threads.emplace_back(workerFn, ref(c), (int)i + 1);
    for (auto& t : threads) t.join();
    cout << "  " << label << " (esperado " << expectedSize << "): " << c.size() << "\n";
    if (c.size() == expectedSize)
        cout << "  EXITO — sin condiciones de carrera\n";
    else
        cout << "  FALLO — corrupcion detectada\n";
}

//forEach range-for
template<typename Container>
void testIteration(Container& c) {
    printSection("forEach / range-for");
    cout << "  range-for: ";
    for (auto& item : c) cout << item << " ";
    cout << "\n";
}

#endif // __DEMO_UTILS_H__