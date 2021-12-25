#include "SimpleTests.h"
#include "../Earley.h"
#include <iostream>

TEST_F(SimpleTests, test_earley) {
    Grammar G({"S->aSbS", "S->"});
    Earley parser(G);
    ASSERT_TRUE(parser.predict("abab"));
    ASSERT_TRUE(parser.predict(""));
    ASSERT_TRUE(parser.predict("aabbababaabb"));
    ASSERT_FALSE(parser.predict("aabbababbaba"));
    ASSERT_FALSE(parser.predict("ba"));
    ASSERT_FALSE(parser.predict("ababa"));

    G = {"S->S+S", "S->S*S", "S->(S)", "S->a", "S->b", "S->c"};
    parser.fit(G);
    ASSERT_TRUE(parser.predict("(a+b)*c"));
    ASSERT_TRUE(parser.predict("a*b+b*c+c*a"));
    ASSERT_TRUE(parser.predict("(a+b)*(a+c)*(b+c)"));
    ASSERT_FALSE(parser.predict("(a+b)*(a+c))*(b+c)"));
    ASSERT_FALSE(parser.predict("(a))"));
    ASSERT_FALSE(parser.predict("(a+(b)+(c)"));


    G = {"S->T+S", "S->T", "T->F*T", "T->F", "F->(S)", "F->a"};
    parser.fit(G);
    ASSERT_TRUE(parser.predict("(a+a)"));
    ASSERT_TRUE(parser.predict("(a)"));
    ASSERT_TRUE(parser.predict("(a+a)*a"));
    ASSERT_TRUE(parser.predict("a+(a*(a+a))+a"));
    ASSERT_FALSE(parser.predict("a+a*(a+a))+a"));
    ASSERT_FALSE(parser.predict("(a"));
    ASSERT_FALSE(parser.predict("(b+b)"));
}

TEST_F(SimpleTests, unit_scan) {
    Earley parser((Grammar()));
    auto& D = parser.get_D();
    D.resize(2);
    D[0]['+'].emplace('S', std::string("S+S") + END, 1, 1);
    D[0]['+'].emplace('S', std::string("S+T") + END, 1, 2);
    D[0]['+'].emplace('S', std::string("T+T") + END, 1, 3);
    D[0]['+'].emplace('S', std::string("T+T+S") + END, 3, 3);
    parser.scan(1, '+');
    ASSERT_EQ(D[1]['S'].size(), 2);
    ASSERT_EQ(D[1]['T'].size(), 2);
    ASSERT_TRUE(D[1]['T'].count(EarleySituation('S', std::string("T+T") + END, 2, 3)));
    ASSERT_TRUE(D[1]['T'].count(EarleySituation('S', std::string("S+T") + END, 2, 2)));
    ASSERT_TRUE(D[1]['S'].count(EarleySituation('S', std::string("S+S") + END, 2, 1)));
    ASSERT_TRUE(D[1]['S'].count(EarleySituation('S', std::string("T+T+S") + END, 4, 3)));
}

TEST_F(SimpleTests, unit_predict) {
    Grammar G({"S->K", "T->a", "T->b"});
    Earley parser(G);
    auto& D = parser.get_D();
    D.resize(1);
    D[0]['S'].emplace('S', std::string("S+S") + END, 2, 1);
    D[0]['K'].emplace('S', std::string("K+S") + END, 0, 1);
    D[0]['T'].emplace('S', std::string("S+T") + END, 2, 2);
    D[0]['S'].emplace('S', std::string("T++S+T") + END, 3, 3);
    parser.predict(0);
    ASSERT_EQ(D[0]['K'].size(), 2);
    ASSERT_EQ(D[0]['a'].size(), 1);
    ASSERT_EQ(D[0]['b'].size(), 1);
    ASSERT_TRUE(D[0]['K'].count(EarleySituation('S', std::string("K") + END, 0, 0)));
    ASSERT_TRUE(D[0]['a'].count(EarleySituation('T', std::string("a") + END, 0, 0)));
    ASSERT_TRUE(D[0]['b'].count(EarleySituation('T', std::string("b") + END, 0, 0)));
}

TEST_F(SimpleTests, unit_complete) {
    Earley parser((Grammar()));
    auto& D = parser.get_D();
    D.resize(3);
    D[1]['S'].emplace('S', std::string("S+S+S") + END, 0, 0);
    D[1]['S'].emplace('S', std::string("S+S+S") + END, 2, 0);
    D[1]['S'].emplace('S', std::string("S+S+S") + END, 4, 0);
    D[2][END].emplace('S', std::string("a") + END, 1, 1);
    D[2][END].emplace('S', std::string("b") + END, 1, 1);
    parser.complete(2);
    ASSERT_EQ(D[2]['+'].size(), 2);
    ASSERT_EQ(D[2][END].size(), 3);
    ASSERT_TRUE(D[2]['+'].count(EarleySituation('S', std::string("S+S+S") + END, 1, 0)));
    ASSERT_TRUE(D[2]['+'].count(EarleySituation('S', std::string("S+S+S") + END, 3, 0)));
    ASSERT_TRUE(D[2][END].count(EarleySituation('S', std::string("S+S+S") + END, 5, 0)));
}


TEST_F(SimpleTests, input_test) {
    std::stringstream in;
    in << "2 S->aSbS S->";
    Grammar G;
    in >> G;
    Earley parser(G);
    ASSERT_TRUE(parser.predict("ab"));
}

TEST_F(SimpleTests, test_LR1) {
    Grammar G({"S->aSbS", "S->"});
    LR1 parser(G);
    ASSERT_TRUE(parser.predict("abab"));
    ASSERT_TRUE(parser.predict(""));
    ASSERT_TRUE(parser.predict("aabbababaabb"));
    ASSERT_FALSE(parser.predict("aabbababbaba"));
    ASSERT_FALSE(parser.predict("ba"));
    ASSERT_FALSE(parser.predict("ababa"));
}




