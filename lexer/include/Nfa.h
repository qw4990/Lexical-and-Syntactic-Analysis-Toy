/**
    NFA类

 * 用正则式构建NFA的时候, 是根据下面的BFN式递归下降的构建
 * pattern      ->   pattern-unit | pattern
 * pattern-unit ->   factor pattern-unit
 * factor       ->   character || select || (pattern)
 * character    ->   CHAR || CHAR* || CHAR+ || CHAR? || CHAR || CHAR* || CHAR+ || CHAR?
 * select       ->   [~~]
 *
 */


#ifndef NFA_H
#define NFA_H

#include "Node.h"


class Nfa {
private:
    // 打印错误并exit
    void print_error(const string& msg) const;

    // 下面函数用于递归下降时匹配bfn式, 来构建NFA.
    bool match_pattern(const string& pattern, int& pos, const int& lim, Nfa& ret_nfa) const;
    bool match_pattern_unit(const string& pattern, int& pos, const int& lim, Nfa& ret_nfa) const;
    bool match_factor(const string &pattern, int &pos, const int& lim, Nfa& ret_nfa) const;
    bool match_character(const string& pattern, int& pos, const int& lim, Nfa& ret_nfa) const;
    bool match_select(const string& pattern, int& pos, const int& lim, Nfa& ret_nfa) const;
    bool match_repeat(const string &pattern, int& pos, const int &lim, Nfa &ret_nfa) const;


public:
    Nfa():_start(new Node()),
             _end(new Node()){}
    Nfa(Node* const start, Node* const end):
            _start(start), _end(end) {}

    Nfa(const string& pattern);

    Node* get_start() const { return _start; }
    Node* get_end() const { return _end; }
    void set_start(Node* const start) { _start = start; }
    void set_end(Node* const end) { _end = end; }

private:
    // 开始节点
    Node* _start;

    // 终止节点, 如果有多个终止节点, 则任意选一个
    Node* _end;
};

#endif // NFA_H
