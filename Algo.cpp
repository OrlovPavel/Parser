#include "Algo.h"

bool is_nonterminal(char symbol) {
    return (symbol >= 'A' && symbol <= 'Z') || symbol == START;
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


//Earley - implementation **********************************************************************************************

bool EarleySituation::operator<(const EarleySituation& other) const{
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

void Earley::add(std::set<EarleySituation>& set, const EarleySituation& situation) {
    size_t prev_size = set.size();
    set.insert(situation);
    if(prev_size != set.size())
        is_changed = true;
}

const std::vector<Set>& Earley::scan(size_t set_index, char symbol) {
    if(set_index == 0 || D[set_index - 1].count(symbol) == 0) {
        return D;
    }
    for(auto& situation: D[set_index - 1][symbol]) {
        EarleySituation new_conf(situation);
        new_conf.dot += 1;
        char new_next = new_conf.right[new_conf.dot];
        add(D[set_index][new_next], new_conf);
    }
    return D;
}

const std::vector<Set>& Earley::predict(size_t set_index) {
    for(auto& node: D[set_index]) {
        char next = node.first;
        if (is_nonterminal(next)) {
            for (auto& right: rules[next]) {
                EarleySituation new_conf(next, right, 0, set_index);
                char new_next = new_conf.right[new_conf.dot];
                add(D[set_index][new_next], new_conf);
            }
        }
    }
    return D;
}

const std::vector<Set>& Earley::complete(size_t set_index) {
    if(D[set_index].count(END) == 1) {
        for (auto& situation: D[set_index][END]) {
            char left = situation.left;
            size_t prev_index = situation.index;
            if (D[prev_index].count(left) == 1) {
                for (auto& prev_conf: D[prev_index][left]) {
                    EarleySituation new_conf(prev_conf);
                    ++new_conf.dot;
                    char new_next = new_conf.right[new_conf.dot];
                    add(D[set_index][new_next], new_conf);
                }
            }
        }
    }
    return D;
}

bool Earley::predict(const std::string& word) {
    std::string copy = word + END;
    D.resize(copy.size());
    EarleySituation initial(START, std::string("S") + END, 0, 0);
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

//LR1 implementation ***************************************************************************************************

bool LR1Situation::operator<(const LR1Situation& other) const {
    if(left < other.left)
        return true;
    if(left == other.left && right < other.right)
        return true;
    if(left == other.left && right == other.right && dot < other.dot)
        return true;
    if(left == other.left && right == other.right && dot == other.dot && next < other.next)
        return true;
    return false;
}

bool LR1Situation::operator==(const LR1Situation& other) const {
    return left == other.left && right == other.right && dot == other.dot && next == other.next;
}

bool Rule::operator<(const Rule& other) const {
    if(left < other.left)
        return true;
    if(left == other.left && right < other.right)
        return true;
    return false;
}

bool Action::operator==(const Action& other) const {
    return name == other.name && count == other.count && reduce_case == other.reduce_case;
}

void LR1::fill_firsts() {
    for(auto& node: grammar.rules) {
        char left = node.first;
        for(auto& right: node.second) {
            for(char C: right) {
                if (is_nonterminal(C)) {
                    std::set<char>& closure = firsts[C];
                    closure.insert(C);
                    bool changed = true;
                    while (changed) {
                        changed = false;
                        for (char first: closure) {
                            if (is_nonterminal(first)) {
                                for (auto& rule: grammar[first]) {
                                    if (closure.count(rule[0]) == 0) {
                                        changed = true;
                                        closure.insert(rule[0]);
                                    }
                                }
                            }
                        }
                    }
                    firsts[left].insert(closure.begin(), closure.end());
                    if(closure.count(END) == 0) {
                        break;
                    }
                } else {
                    firsts[left].insert(C);
                    break;
                }
            }
        }
    }
    for(auto& node: firsts) {
        std::set<char>& symbols = node.second;
        for(auto it = symbols.begin(); it != symbols.end();) {
            if(is_nonterminal(*it)) {
                it = symbols.erase(it);
            } else {
                ++it;
            }
        }
    }
}

std::set<char> LR1::get_first(const std::string& word) {
    std::set<char> res;
    for(char c: word) {
        if(is_nonterminal(c)) {
            res.insert(firsts[c].begin(), firsts[c].end());
            if(firsts[c].count(END)) {
                break;
            }
        } else {
            if(c != END) {
                res.insert(c);
                break;
            }
        }
    }
    if(res.empty()) {
        res.insert(END);
    }
    return res;
}



void LR1::closure(Condition& condition) {
    bool changed = true;
    while(changed) {
        changed = false;
        for(auto& node: condition) {
            char after_dot = node.first;
            if(is_nonterminal(after_dot)) {
                for(auto& situation: node.second) {
                    std::string suff = situation.right.substr(situation.dot + 1, situation.right.size() - 1) + situation.next;
                    std::set<char> first = get_first(suff);
                    for(auto& right: grammar[after_dot]) {
                        for(char next: first) {
                            LR1Situation new_situation(after_dot, right, 0, next);
                            char key = new_situation.right[new_situation.dot];
                            if(condition.count(key) == 0 || condition[key].count(new_situation) == 0) {
                                changed = true;
                                condition[key].emplace(new_situation);
                            }
                        }
                    }
                }
            }
        }
    }
}

size_t LR1::add_condition(const std::set<LR1Situation>& prev) {
    Condition new_condition;
    for(auto& situation: prev) {
        LR1Situation new_situation = situation;
        ++new_situation.dot;
        char next = new_situation.right[new_situation.dot];
        new_condition[next].emplace(new_situation);
    }
    closure(new_condition);
    for(size_t i = 0; i < conditions.size(); ++i) {
        if(conditions[i] == new_condition) {
            return i;
        }
    }
    conditions.push_back(std::move(new_condition));
    edges.emplace_back();
    is_changed = true;
    return conditions.size() - 1;
}

void LR1::fill_conditions() {
    conditions.emplace_back();
    edges.emplace_back();
    conditions[0]['S'].emplace(START, std::string("S") + END, 0, END);
    closure(conditions[0]);
    is_changed = true;
    size_t cur = 0;
    while(cur < conditions.size() || is_changed) {
        is_changed = false;
        for(auto& node: conditions[cur]) {
            if(node.first != END) {
                edges[cur][node.first] = add_condition(node.second);
            }
        }
        ++cur;
    }
}

void LR1::insert_table(size_t condition, char symbol, const Action& value) {
    if(table[condition].count(symbol) == 1) {
        if(table[condition][symbol] == value) {
            return;
        } else {
            std::cerr << "Wrong grammar!";
            throw std::bad_exception();
        }
    }
    table[condition][symbol] = value;
}

void LR1::fill_table() {
    table.resize(conditions.size());
    for(size_t i = 0; i < table.size(); ++i) {
        for(auto& node: conditions[i]) {
            char after_dot = node.first;
            if(is_nonterminal(after_dot)) {
                insert_table(i, after_dot, Action('s', edges[i][after_dot]));
            } else{
                for(auto& situation: node.second) {
                    char next = situation.next;
                    if(after_dot == END) {
                        if(situation == LR1Situation(START, std::string("S") + END, 1, END)) {
                            insert_table(i, next, Action('a', 0));
                        } else {
                            insert_table(i, next, Action('r', situation.right.size() - 1, situation.left));
                        }
                    } else {
                        insert_table(i, after_dot, Action('s', edges[i][after_dot]));
                    }
                }
            }
        }
    }
}

LR1::LR1(const Grammar& grammar) : grammar(grammar){
    init();
}

void LR1::fit(const Grammar& grammar) {
    firsts.clear();
    conditions.clear();
    edges.clear();
    table.clear();
    this->grammar = grammar;
    init();
}

void LR1::init() {
    fill_firsts();
    fill_conditions();
    fill_table();
    firsts.clear();
    firsts.clear();
    conditions.clear();
}

bool LR1::predict(std::string word) {
    word += END;
    std::stack<size_t> way;
    way.push(0);
    for(size_t i = 0; i < word.size();) {
        size_t condition = way.top();
        if(table[condition].count(word[i]) == 0) {
            return false;
        } else {
            Action action = table[condition][word[i]];
            if(action.name == 'r') {
                for(size_t j = 0; j < action.count; ++j) {
                    way.pop();
                    way.pop();
                }
                condition = way.top();
                if(table[condition].count(action.reduce_case) == 0) {
                    return false;
                } else{
                    way.push((unsigned char)action.reduce_case);
                    way.push(table[condition][action.reduce_case].count);
                }
            }
            if(action.name == 's') {
                way.push((unsigned char)word[i]);
                way.push(action.count);
                ++i;
            }
            if(action.name == 'a') {
                return true;
            }
        }
    }
    return false;
}



