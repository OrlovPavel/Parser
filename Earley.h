#ifndef PARSER_EARLEY_H
#define PARSER_EARLEY_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
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
    bool isCompleted = false;

    EarleySituation(char left, const std::string& right, size_t dot, size_t index, bool isCompleted = false) : left(left), right(right), dot(dot),
                                                                                     index(index), isCompleted(isCompleted) {}

    EarleySituation(const EarleySituation& other) = default;

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
    std::unordered_set<char> isPredicted;
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


#endif //PARSER_EARLEY_H
