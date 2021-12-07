#include "Algo.h"

bool is_nonterminal(char symbol) {
    return (symbol >= 'A' && symbol <= 'Z') || symbol == START;
}

bool Configuration::operator<(const Configuration& other) const{
    if(left < other.left)
        return true;
    if(left == other.left && right < other.right)
        return true;
    if(left == other.left && right == other.right && dot < other.dot)
        return true;
    if(left == other.left && right == other.right && dot == other.dot && index < other.index)
        return true;
    return false;
}

std::istream& operator>>(std::istream& in, Grammar& grammar) {
    size_t size;
    std::string rule;
    in >> size;
    grammar.rules.clear();
    for(size_t i = 0; i < size; ++i) {
        in >> rule;
        rule += END;
        char left = rule[0];
        std::string right = rule.substr(3, rule.size());
        grammar.rules[left].push_back(right);
    }
    return in;
}

Grammar::Grammar(const std::vector<std::string>& rules) {
    for(auto& rule: rules) {
        std::string copy = rule + END;
        char left = copy[0];
        std::string right = copy.substr(3, copy.size());
        this->rules[left].push_back(right);
    }
}

Grammar& Grammar::operator=(const std::vector<std::string>& other){
    rules.clear();
    for(auto& rule: other) {
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


void Algo::add(std::set<Configuration>& set, const Configuration& conf) {
    size_t prev_size = set.size();
    set.insert(conf);
    if(prev_size != set.size())
        is_changed = true;
}

const std::vector<Set>& Algo::scan(size_t set_index, char symbol) {
    if(set_index == 0 || D[set_index - 1].count(symbol) == 0) {
        return D;
    }
    for(auto& conf: D[set_index - 1][symbol]) {
        Configuration new_conf(conf);
        new_conf.dot += 1;
        char new_next = new_conf.right[new_conf.dot];
        add(D[set_index][new_next], new_conf);
    }
    return D;
}

const std::vector<Set>& Algo::predict(size_t set_index) {
    for(auto& node: D[set_index]) {
        char next = node.first;
        if (is_nonterminal(next)) {
            for (auto& right: rules[next]) {
                Configuration new_conf(next, right, 0, set_index);
                char new_next = new_conf.right[new_conf.dot];
                add(D[set_index][new_next], new_conf);
            }
        }
    }
    return D;
}

const std::vector<Set>& Algo::complete(size_t set_index) {
    if(D[set_index].count(END) == 1) {
        for (auto& conf: D[set_index][END]) {
            char left = conf.left;
            size_t prev_index = conf.index;
            if (D[prev_index].count(left) == 1) {
                for (auto& prev_conf: D[prev_index][left]) {
                    Configuration new_conf(prev_conf);
                    ++new_conf.dot;
                    char new_next = new_conf.right[new_conf.dot];
                    add(D[set_index][new_next], new_conf);
                }
            }
        }
    }
    return D;
}

bool Algo::predict(const std::string& word) {
    std::string copy = word + END;
    D.resize(copy.size());
    Configuration initial(START, std::string("S") + END, 0, 0);
    D[0]['S'].insert(initial);
    for(size_t j = 0; j < copy.size(); ++j) {
        scan(j, word[j - 1]);
        is_changed = true;
        while(is_changed) {
            is_changed = false;
            complete(j);
            predict(j);
        }
    }
    ++initial.dot;
    if(D[copy.size() - 1][END].count(initial) == 1) {
        D.clear();
        return true;
    } else {
        D.clear();
        return false;
    }
}

void Algo::fit(const Grammar& grammar) {
    rules = grammar;
    rules[START].emplace_back(std::string("S") + END);
}

Algo::Algo(const Grammar& grammar) : rules(grammar) {
    rules[START].emplace_back(std::string("S") + END);
}

std::vector<Set>& Algo::get_D() {
    return D;
}

