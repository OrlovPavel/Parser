#include "Earley.h"

bool is_nonterminal(char symbol) {
    return (symbol >= 'A' && symbol <= 'Z') || symbol == START;
}

std::istream& operator>>(std::istream& in, Grammar& grammar) {
    size_t size;
    std::string rule;
    in >> size;
    grammar.rules.clear();
    for (size_t i = 0; i < size; ++i) {
        in >> rule;
        rule += END;
        char left = rule[0];
        std::string right = rule.substr(3, rule.size());
        grammar.rules[left].push_back(right);
    }
    return in;
}

Grammar::Grammar(const std::vector<std::string>& rules) {
    for (auto& rule: rules) {
        std::string copy = rule + END;
        char left = copy[0];
        std::string right = copy.substr(3, copy.size());
        this->rules[left].push_back(right);
    }
}

Grammar& Grammar::operator=(const std::vector<std::string>& other) {
    rules.clear();
    for (auto& rule: other) {
        std::string copy = rule + END;
        char left = copy[0];
        std::string right = copy.substr(3, copy.size());
        rules[left].push_back(right);
    }
    return *this;
}

std::vector<std::string>& Grammar::operator[](char symbol) {
    return rules[symbol];
}


//Earley - implementation **********************************************************************************************

bool EarleySituation::operator<(const EarleySituation& other) const {
    if (left < other.left)
        return true;
    if (left == other.left && right < other.right)
        return true;
    if (left == other.left && right == other.right && dot < other.dot)
        return true;
    if (left == other.left && right == other.right && dot == other.dot && index < other.index)
        return true;
    return false;
}

void Earley::add_situation(std::set<EarleySituation>& set, const EarleySituation& situation) {
    if (set.insert(situation).second)
        is_changed = true;
}

const std::vector<Set>& Earley::scan(size_t set_index, char symbol) {
    if (set_index == 0 || D[set_index - 1].count(symbol) == 0) {
        return D;
    }
    for (auto& situation: D[set_index - 1][symbol]) {
        EarleySituation new_situation(situation);
        new_situation.dot += 1;
        char new_next = new_situation.right[new_situation.dot];
        add_situation(D[set_index][new_next], new_situation);
    }
    return D;
}

const std::vector<Set>& Earley::predict(size_t set_index) {
    for (auto& node: D[set_index]) {
        char after_dot = node.first;
        if (is_nonterminal(after_dot) && isPredicted.count(after_dot) == 0) {
            isPredicted.insert(after_dot);
            for (auto& right: rules[after_dot]) {
                EarleySituation new_situation(after_dot, right, 0, set_index);
                char new_next = new_situation.right[new_situation.dot];
                add_situation(D[set_index][new_next], new_situation);
            }
        }
    }
    return D;
}

const std::vector<Set>& Earley::complete(size_t set_index) {
    if (D[set_index].count(END) == 1) {
        for (auto& situation: D[set_index][END]) {
            char left = situation.left;
            size_t prev_index = situation.index;
            if (!situation.isCompleted && D[prev_index].count(left) == 1) {
                for (auto& prev_situation: D[prev_index][left]) {
                    EarleySituation new_situation(prev_situation);
                    ++new_situation.dot;
                    char new_next = new_situation.right[new_situation.dot];
                    add_situation(D[set_index][new_next], new_situation);
                }
            }
            const_cast<EarleySituation&>(situation).isCompleted = true;     //поле isCompleted никак не влияет на кастомный компаратор, так что каст безопасен с точки зрения сохранения структуры set'а
        }
    }
    return D;
}

bool Earley::predict(const std::string& word) {
    std::string copy = word + END;
    D.resize(copy.size());
    EarleySituation initial(START, std::string("S") + END, 0, 0);
    D[0]['S'].insert(initial);
    for (size_t j = 0; j < copy.size(); ++j) {
        scan(j, word[j - 1]);
        is_changed = true;
        isPredicted.clear();
        while (is_changed) {
            is_changed = false;
            complete(j);
            predict(j);
        }
    }
    ++initial.dot;
    if (D[copy.size() - 1][END].count(initial) == 1) {
        D.clear();
        return true;
    } else {
        D.clear();
        return false;
    }
}

void Earley::fit(const Grammar& grammar) {
    rules = grammar;
    rules[START].emplace_back(std::string("S") + END);
}

Earley::Earley(const Grammar& grammar) : rules(grammar) {
    rules[START].emplace_back(std::string("S") + END);
}

std::vector<Set>& Earley::get_D() {
    return D;
}




