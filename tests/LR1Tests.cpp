#include "LR1Tests.h"
#include "../LR1.h"

TEST_F(LR1Tests, test_LR1) {
    Grammar G({"S->aSbS", "S->"});
    LR1 parser(G);
    ASSERT_TRUE(parser.predict("abab"));
    ASSERT_TRUE(parser.predict(""));
    ASSERT_TRUE(parser.predict("aabbababaabb"));
    ASSERT_FALSE(parser.predict("aabbababbaba"));
    ASSERT_FALSE(parser.predict("ba"));
    ASSERT_FALSE(parser.predict("ababa"));

    G = {"S->aB", "B->bc", "B->c"};
    parser.fit(G);
    ASSERT_TRUE(parser.predict("abc"));
    ASSERT_TRUE(parser.predict("ac"));
    ASSERT_FALSE(parser.predict("abb"));
    ASSERT_FALSE(parser.predict("a"));
    ASSERT_FALSE(parser.predict("aba"));
    ASSERT_FALSE(parser.predict("aa"));

    G = {"S->CC", "C->cC", "C->d"};
    parser.fit(G);
    ASSERT_TRUE(parser.predict("dd"));
    ASSERT_TRUE(parser.predict("dcccd"));
    ASSERT_TRUE(parser.predict("cdcd"));
    ASSERT_TRUE(parser.predict("ccccdd"));
    ASSERT_TRUE(parser.predict("cccccdccccd"));
    ASSERT_FALSE(parser.predict("cccccddccccd"));
    ASSERT_FALSE(parser.predict("cccccddccccdccd"));
    ASSERT_FALSE(parser.predict("ddc"));
    ASSERT_FALSE(parser.predict("dcdcd"));
    ASSERT_FALSE(parser.predict("ccc"));
}


TEST_F(LR1Tests, unit_firsts) {
    Grammar G({"S->aSbS", "S->"});
    LR1 parser(G);
    parser.fill_firsts();
    ASSERT_TRUE( parser.get_firsts().at('S') == std::set<char>({'a', END}));

    G = {"S->aB", "B->bc", "B->c"};
    parser.fit(G);
    parser.fill_firsts();
    ASSERT_TRUE(parser.get_firsts().at('S') == std::set<char>({'a'}));
    ASSERT_TRUE(parser.get_firsts().at('B') == std::set<char>({'b', 'c'}));

    G = {"S->+S", "S->(S)", "S->a", "S->b", "S->"};
    parser.fit(G);
    parser.fill_firsts();
    ASSERT_TRUE(parser.get_firsts().at('S') == std::set<char>({'a', 'b', END, '+', '(', '+'}));

    G = {"S->CC", "C->cC", "C->d"};
    parser.fit(G);
    parser.fill_firsts();
    ASSERT_TRUE(parser.get_firsts().at('S') == std::set<char>({'c', 'd'}));
    ASSERT_TRUE(parser.get_firsts().at('C') == std::set<char>({'c', 'd'}));
}

TEST_F(LR1Tests, unit_conditions) {
    Grammar G({"S->SaSb", "S->"});
    LR1 parser(G);
    auto& conditions = parser.get_conditions();
    std::set<LR1Situation> situations = {LR1Situation('S', std::string("SaSb") + END, 1, END), LR1Situation('S', std::string("SaSb") + END, 1, 'a')};
    size_t index = parser.add_condition(situations);
    Condition right_condition = {{'S', {LR1Situation('S', std::string("SaSb") + END, 2, END), LR1Situation('S', std::string("SaSb") + END, 2, 'a'),
                                               LR1Situation('S', std::string("SaSb") + END, 0, 'b'), LR1Situation('S', std::string("SaSb") + END, 0, 'a')}},
                                 {END, {LR1Situation('S', std::string("") + END, 0, 'b'), LR1Situation('S', std::string("") + END, 0, 'a')}}};
    ASSERT_TRUE(conditions[index] == right_condition);

    situations = {LR1Situation('S', std::string("SaSb") + END, 2, 'b'), LR1Situation('S', std::string("SaSb") + END, 2, END)};
    index = parser.add_condition(situations);
    right_condition = {{'b', {LR1Situation('S', std::string("SaSb") + END, 3, 'b'), LR1Situation('S', std::string("SaSb") + END, 3, END)}}};
    ASSERT_TRUE(conditions[index] == right_condition);

    G = {"S->CC", "C->cC", "C->d"};
    parser.fit(G);

    situations = {LR1Situation('S', std::string("CC") + END, 0, END)};
    index = parser.add_condition(situations);
    right_condition = {{'C', {LR1Situation('S', std::string("CC") + END, 1, END)}},
                       {'c', {LR1Situation('C', std::string("cC") + END, 0, END)}},
                       {'d', {LR1Situation('C', std::string("d") + END,  0, END)}}};
    ASSERT_TRUE(conditions[index] == right_condition);

    situations = {LR1Situation('C', std::string("cC") + END, 0, 'c'), LR1Situation('C', std::string("cC") + END, 0, 'd')};
    index  = parser.add_condition(situations);
    right_condition = {{'C', {LR1Situation('C', std::string("cC") + END, 1, 'c'), LR1Situation('C', std::string("cC") + END, 1, 'd')}},
                       {'c', {LR1Situation('C', std::string("cC") + END, 0, 'c'), LR1Situation('C', std::string("cC") + END, 0, 'd')}},
                       {'d', {LR1Situation('C', std::string("d") + END, 0, 'c'), LR1Situation('C', std::string("d") + END, 0, 'd')}}};
    ASSERT_TRUE(conditions[index] == right_condition);
}

TEST_F(LR1Tests, unit_table) {
    Grammar G({"S->SaSb", "S->"});
    LR1 parser(G);
    std::vector<std::unordered_map<char, Action>> right_table = {{{'a', Action(REDUCE, 0, 'S')}, {END, Action(REDUCE, 0, 'S')}, {'S', Action(SHIFT, 1)}},
                                                                 {{'a', Action(SHIFT, 2)}, {END, Action(ACCEPT)}},
                                                                 {{'a', Action(REDUCE, 0, 'S')}, {'b', Action(REDUCE, 0, 'S')}, {'S', Action(SHIFT, 3)}},
                                                                 {{'a', Action(SHIFT, 5)}, {'b', Action(SHIFT, 4)}},
                                                                 {{'a', Action(REDUCE, 4, 'S')}, {END, Action(REDUCE, 4, 'S')}},
                                                                 {{'a', Action(REDUCE, 0, 'S')}, {'b', Action(REDUCE, 0, 'S')}, {'S', Action(SHIFT, 6)}},
                                                                 {{'a', Action(SHIFT, 5)}, {'b', Action(SHIFT, 7)}},
                                                                 {{'a', Action(REDUCE, 4, 'S')}, {'b', Action(REDUCE, 4, 'S')}}};
    auto& table1 = parser.get_table();
    ASSERT_TRUE(table1.size() == right_table.size());
    for(auto& right_line: right_table) {
        bool inTable = false;
        for(auto& line: table1) {
            if(right_line.size() == line.size()) {
                inTable = true;
                for (auto& node: right_line) {
                    inTable = (line.count(node.first) == 1) && compare_actions(line.at(node.first), node.second);
                    if(!inTable) {
                        break;
                    }
                }
                if(inTable) {
                    break;
                }
            }
        }
        ASSERT_TRUE(inTable);
    }
    G = {"S->CC", "C->cC", "C->d"};
    parser.fit(G);
    right_table = {{{'S', Action(SHIFT, 1)}, {'C', Action(SHIFT, 2)}, {'c', Action(SHIFT, 3)}, {'d', Action(SHIFT, 4)}},
                   {{END, Action(ACCEPT)}},
                   {{'C', Action(SHIFT, 5)}, {'c', Action(SHIFT, 6)}, {'d', Action(SHIFT, 7)}},
                   {{'C', Action(SHIFT, 8)}, {'c', Action(SHIFT, 3)}, {'d', Action(SHIFT, 4)}},
                   {{'c', Action(REDUCE, 1, 'C')}, {'d', Action(REDUCE, 1, 'C')}},
                   {{END, Action(REDUCE, 2, 'S')}},
                   {{'C', Action(SHIFT, 9)}, {'c', Action(SHIFT, 6)}, {'d', Action(SHIFT, 7)}},
                   {{END, Action(REDUCE, 1, 'C')}},
                   {{'c', Action(REDUCE, 2, 'C')}, {'d', Action(REDUCE, 2, 'C')}},
                   {{END, Action(REDUCE, 2, 'C')}}};
    auto& table2 = parser.get_table();
    ASSERT_TRUE(table2.size() == right_table.size());
    for(auto& right_line: right_table) {
        bool inTable = false;
        for(auto& line: table2) {
            if(right_line.size() == line.size()) {
                inTable = true;
                for (auto& node: right_line) {
                    inTable = (line.count(node.first) == 1) && compare_actions(line.at(node.first), node.second);
                    if(!inTable) {
                        break;
                    }
                }
                if(inTable) {
                    break;
                }
            }
        }
        ASSERT_TRUE(inTable);
    }
}

TEST_F(LR1Tests, unit_closure) {
    Grammar G({"S->SaSb", "S->"});
    LR1 parser(G);
    Condition testing = {{'S', {LR1Situation('S', std::string("SaSb") + END, 2, END), LR1Situation('S', std::string("SaSb") + END, 2, 'a')}}};
    parser.closure(testing);
    Condition right_condition = {{'S', {LR1Situation('S', std::string("SaSb") + END, 2, END), LR1Situation('S', std::string("SaSb") + END, 2, 'a'),
                                        LR1Situation('S', std::string("SaSb") + END, 0, 'b'), LR1Situation('S', std::string("SaSb") + END, 0, 'a')}},
                                 {END, {LR1Situation('S', std::string("") + END, 0, 'b'), LR1Situation('S', std::string("") + END, 0, 'a')}}};
    ASSERT_TRUE(right_condition == testing);

    testing = {{'a', {LR1Situation('S', std::string("SaSb") + END, 1, 'a')}}};
    parser.closure(testing);
    right_condition = {{'a', {LR1Situation('S', std::string("SaSb") + END, 1, 'a')}}};
    ASSERT_TRUE(right_condition == testing);

    testing = {{'S', {LR1Situation('S', std::string("SaSb") + END, 2, 'b'), LR1Situation('S', std::string("SaSb") + END, 2, 'a')}}};
    parser.closure(testing);
    right_condition = {{'S', {LR1Situation('S', std::string("SaSb") + END, 2, 'b'), LR1Situation('S', std::string("SaSb") + END, 2, 'a'),
                              LR1Situation('S', std::string("SaSb") + END, 0, 'b'), LR1Situation('S', std::string("SaSb") + END, 0, 'a')}},
                       {END, {LR1Situation('S', std::string("") + END, 0, 'b'), LR1Situation('S', std::string("") + END, 0, 'a')}}};
    ASSERT_TRUE(right_condition == testing);

    G = {"S->CC", "C->cC", "C->d"};
    parser.fit(G);
    testing = {{'C', {LR1Situation('S', std::string("CC") + END, 1, END)}}};
    parser.closure(testing);
    right_condition = {{'C', {LR1Situation('S', std::string("CC") + END, 1, END)}},
                       {'c', {LR1Situation('C', std::string("cC") + END, 0, END)}},
                       {'d', {LR1Situation('C', std::string("d") + END, 0, END)}}};
    ASSERT_TRUE(right_condition == testing);

    testing = {{'C', {LR1Situation('C', std::string("cC") + END, 1, 'c'), LR1Situation('C', std::string("cC") + END, 1, 'd')}}};
    parser.closure(testing);
    right_condition = {{'C', {LR1Situation('C', std::string("cC") + END, 1, 'c'), LR1Situation('C', std::string("cC") + END, 1, 'd')}},
                       {'c', {LR1Situation('C', std::string("cC") + END, 0, 'c'), LR1Situation('C', std::string("cC") + END, 0, 'd')}},
                       {'d', {LR1Situation('C', std::string("d") + END, 0, 'c'), LR1Situation('C', std::string("d") + END, 0, 'd')}}};
    ASSERT_TRUE(right_condition == testing);

    testing = {{'C', {LR1Situation('C', std::string("cC") + END, 1, END)}}};
    parser.closure(testing);
    right_condition = {{'C', {LR1Situation('C', std::string("cC") + END, 1, END)}},
                       {'c', {LR1Situation('C', std::string("cC") + END, 0, END)}},
                       {'d', {LR1Situation('C', std::string("d") + END, 0, END)}}};
    ASSERT_TRUE(right_condition == testing);
}

TEST_F(LR1Tests, throw_test) {
    Grammar G({"S->S+S", "S->S*S", "S->(S)", "S->a", "S->b", "S->c"});
    ASSERT_ANY_THROW(LR1 parser(G));
}

