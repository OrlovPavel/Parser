#include "Algo.h"

int main() {
    Grammar G({"S->SaSb", "S->"});
    LR1 parser(G);
    std::cout << parser.predict("aabbab");
    return 0;
}
