#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include "../types.h"
#include "btree.h"
#include "traits.h"
#include "DemoUtils.h"
using namespace std;

using Trait = Tree34Trait<TypeBTree>;
using BT    = BTree<Trait>;

//concurrencia
static void concurrencyWorker(BT& tree, Ref workerId) {
    for (Size i = 0; i < 200; ++i)
        tree.insert(static_cast<TypeBTree>('a' + ((workerId * 7 + i) % 26)), workerId);
}

void DemoBTree() {
    printHeader("BTREE");

    //insert
    printSection("insert");
    BT bt;
    const string keys = "D1XJ2xTg8zKL9AhijOPQcEowRSp0NbW567BUfCqrs4FdtYZakHIuvGV3eMylmn";
    for (Size i = 0; i < keys.size(); ++i)
        bt.insert(keys[i], static_cast<Ref>(i * i));
    cout << "  size=" << bt.size() << "  height=" << bt.height() << "  order=" << bt.order() << "\n";

    //search
    printSection("search");
    try {
        auto [val, ref] = bt.search('Z');
        cout << "  search('Z') -> encontrado  valor=" << val << "  ref=" << ref << "\n";
    } catch (const runtime_error& e) {
        cout << "  search('Z') -> " << e.what() << "\n";
    }
    try {
        bt.search('!');
    } catch (const runtime_error& e) {
        cout << "  search('!') -> " << e.what() << "\n";
    }

    //ForEach variadic
    printSection("forEach (variadic, sin Print helper)");
    Size letterCount = 0;
    bt.forEach([](BT::Entry& e, Level, Size& count) {
        if (isalpha(static_cast<unsigned char>(e.m_data))) ++count;
    }, letterCount);
    cout << "  letras en el arbol: " << letterCount << "\n";

    //FirstThat variadic
    printSection("firstThat (variadic)");
    auto* entry = bt.firstThat([](BT::Entry& e, Level, TypeBTree target) {
        return e.m_data == target;
    }, TypeBTree('M'));
    cout << "  firstThat('M') -> " << (entry ? "encontrado" : "no encontrado");
    if (entry) cout << " ref=" << entry->m_ref;
    cout << "\n";

    //remove
    printSection("remove");
    Size beforeRemove = bt.size();
    auto [removedVal, removedRef] = bt.remove('A');
    cout << "  remove('A') -> eliminado: valor=" << removedVal << " ref=" << removedRef
         << "  size antes=" << beforeRemove << "  size despues=" << bt.size() << "\n";

    //iterador begin/end, for-range
    printSection("for (auto& entry : bt) — iterador inorder");
    string inorderKeys;
    for (auto& entry : bt) inorderKeys += entry.m_data;
    cout << "  claves en orden: " << inorderKeys << "\n";

    //useCount()
    printSection("useCount() — contador de accesos por clave");
    bt.search('B'); bt.search('B'); bt.search('B');
    bt.search('C');                              
    for (auto& entry : bt)
        if (entry.m_data == 'B' || entry.m_data == 'C')
            cout << "  '" << entry.m_data << "' useCount=" << entry.useCount() << "\n";

    //operator<< operator>>
    testIO(bt);

    //copy / move constructor ──────────────────────────────────────────
    testCopyMove(bt, [](BT& c) { c.insert('!', 999); });

    //concurrencia
    printSection("Concurrencia");
    BT concurrentTree;
    const Size kThreads = 5, kInsertsPerThread = 200;
    vector<thread> threads;
    threads.reserve(kThreads);
    for (Size i = 0; i < kThreads; ++i)
        threads.emplace_back(concurrencyWorker, std::ref(concurrentTree), static_cast<Ref>(i + 1));
    for (auto& t : threads) t.join();
    cout << "  inserciones concurrentes lanzadas: " << (kThreads * kInsertsPerThread) << "\n";
    cout << "  size final (sin corrupcion, <= 26 claves unicas): " << concurrentTree.size() << "\n";

    printFooter("BTREE");
}