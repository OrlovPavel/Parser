#include "LR1.h"

int main() {
    Grammar G({"S->SaSb", "S->"});
    LR1 parser(G);
    std::cout << parser.predict("ab");
    return 0;
}
