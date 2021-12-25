#ifndef PARSER_ALGO_H
#define PARSER_ALGO_H
#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <stack>

const char END = '$';
const char START = '#'; // #->S instead of S'->S

bool is_nonterminal(char symbol);

struct EarleySituation {
    char left;
    std::string right;
    size_t dot;
    size_t index = 0;
    EarleySituation(char left, const std::string& right, size_t dot, size_t index) : left(left), right(right), dot(dot), index(index) {}
    EarleySituation(const EarleySituation& other)  = default;

    bool operator<(const EarleySituation& other) const;
};

struct Grammar {
    std::unordered_map<char, std::vector<std::string>> rules;
    Grammar() = default;
    Grammar(const std::vector<std::string>&);
    Grammar& operator=(const std::vector<std::string>&);
    Grammar& operator=(const Grammar& other) = default;

    std::vector<std::string>& operator[](char symbol);
};

std::istream& operator>>(std::istream& in, Grammar& grammar);


using Set = std::unordered_map<char, std::set<EarleySituation>>;

class Earley {
private:
    Grammar rules;
    std::vector<Set> D;
    bool is_changed = false;

    void add_situation(std::set<EarleySituation>&, const EarleySituation&);
public:
    Earley() = default;

    Earley(const Grammar& grammar);

    void fit(const Grammar& grammar);

    bool predict(const std::string& word);

    const std::vector<Set>& scan(size_t set_index, char);

    const std::vector<Set>& predict(size_t set_index);

    const std::vector<Set>& complete(size_t set_index);

    std::vector<Set>& get_D();
};


struct LR1Situation {
    char left;
    std::string right;
    size_t dot;
    char next;
    LR1Situation(char left, const std::string& right, size_t dot, char next) : left(left), right(right), dot(dot), next(next) {}
    LR1Situation(const LR1Situation& other)  = default;

    bool operator==(const LR1Situation& other) const;
    bool operator<(const LR1Situation& other) const;
};

struct Action {
    char name;
    size_t count;
    char reduce_case;
    Action(char name, size_t count, char reduce_case = 0) : name(name), count(count), reduce_case(reduce_case) {};
    Action() = default;

    bool operator==(const Action& other) const;
};

using Condition = std::unordered_map<char, std::set<LR1Situation>>;

class LR1 {
private:
    Grammar grammar;
    std::unordered_map<char, std::set<char>> firsts;
    std::vector<Condition> conditions;
    std::vector<std::unordered_map<char, size_t>> edges;
    std::vector<std::unordered_map<char, Action>> table;
    bool is_changed = true;

    void init();
    void insert_table(size_t condition, char symbol, const Action& value);
public:
    LR1() = default;

    LR1(const Grammar& grammar);

    void fit(const Grammar& grammar);

    bool predict(std::string);

    void fill_firsts();
    void fill_conditions();
    void fill_table();

    size_t add_condition(const std::set<LR1Situation>& prev);


    void closure(Condition& condition);

    std::set<char> get_first(const std::string& word);
};



#endif //PARSER_ALGO_H
