#ifndef PARSER_LR1_H
#define PARSER_LR1_H

#include "Earley.h"

struct LR1Situation {
    char left;
    std::string right;
    size_t dot;
    char next;

    LR1Situation(char left, const std::string& right, size_t dot, char next) : left(left), right(right), dot(dot),
                                                                               next(next) {}

    LR1Situation(const LR1Situation& other) = default;

    bool operator==(const LR1Situation& other) const;

    bool operator<(const LR1Situation& other) const;
};

enum ActionNames {
    SHIFT,
    REDUCE,
    ACCEPT
};

struct Action {
    ActionNames name;
    size_t count;
    char reduce_case;

    Action(ActionNames name, size_t count = 0, char reduce_case = 0) : name(name), count(count),
                                                                   reduce_case(reduce_case) {};

    Action() = default;

    bool operator==(const Action& other) const;
};

bool compare_actions(const Action& first, const Action& second);

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

    void update_firsts(char C);

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

    const std::unordered_map<char, std::set<char>>& get_firsts() const;
    const std::vector<std::unordered_map<char, Action>>& get_table() const;
    const std::vector<Condition>& get_conditions() const;
};


#endif //PARSER_LR1_H
