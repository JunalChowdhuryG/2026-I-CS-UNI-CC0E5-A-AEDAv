
#include <ostream>
using namespace std;

template <typename Container>
void Print(Container& c, ostream &os){
    os << c << endl;
}

template <typename Container, typename Func>
void ForEach(Container& c, Func func){
    for(auto it = c.begin(); it != c.end(); ++it)
        func(*it);
}