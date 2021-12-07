#include "Algo.h"

int main() {
    Grammar G({"S->K", "T->a", "T->b"});
    Algo parser((Grammar()));
    auto& D = parser.get_D();
    D.resize(1);
    D[0]['S'].emplace('S', std::string("S+S") + END, 2, 1);
    D[0]['K'].emplace('S', std::string("K+S") + END, 0, 1);
    D[0]['T'].emplace('S', std::string("S+T") + END, 2, 2);
    D[0]['S'].emplace('S', std::string("T++S+T") + END, 3, 3);
    parser.predict(0);
    /*Grammar G;
    std::cin >> G;
    Algo parser(G);
    std::string word;
    std::cin >> word;
    std::cout << parser.predict(word);*/
    return 0;
}
