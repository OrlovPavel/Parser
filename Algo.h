#ifndef PARSER_ALGO_H
#define PARSER_ALGO_H
#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>

const char END = '$';
const char START = '#'; // #->S instead of S'->S

bool is_nonterminal(char symbol);

struct Configuration {
    char left;
    std::string right;
    size_t dot;
    size_t index = 0;
    Configuration(char left, const std::string& right, size_t dot, size_t index) : left(left), right(right), dot(dot), index(index) {}
    Configuration(const Configuration& other)  = default;

    bool operator<(const Configuration& other) const;
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


using Set = std::unordered_map<char, std::set<Configuration>>;

class Algo {
private:
    Grammar rules;
    std::vector<Set> D;
    bool is_changed = false;

    void add(std::set<Configuration>&, const Configuration&);
public:
    Algo() = default;

    Algo(const Grammar& grammar);

    void fit(const Grammar& grammar);

    bool predict(const std::string& word);

    const std::vector<Set>& scan(size_t set_index, char);

    const std::vector<Set>& predict(size_t set_index);

    const std::vector<Set>& complete(size_t set_index);

    std::vector<Set>& get_D();
};



#endif //PARSER_ALGO_H
