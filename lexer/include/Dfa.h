/**
    DFA¿‡
*/

#ifndef DFA_H
#define DFA_H

#include "../base.h"
#include "Node.h"
#include "Nfa.h"
#include "ctype.h"

class Dfa {
private:
    // package a set of nodes, used to represent current state.
    typedef set<Node*> State;
    // the type of hash result
    typedef unsigned long long HashCodeType;

    // if this state contain any end node.
    bool contain_end_node(const State& state) const;

    // hash a state.
    HashCodeType hash_code(State &st) const;

    // the nodes witch can be arrived from start node or start state.
    // node: must ignore PATH_EPSILON
    void next(Node *const start_node, State &ret_node_set, const char path_char, bool clear_set) const;
    void next(const State &start_state, State &ret_node_set, const char path_char) const;

    // get a closure of a node or a state.
    void closure(const State& start_state, State& ret_node_set) const;
    void closure(Node* const start, State& ret_node_set, bool clear_set) const;

    // translate a NFA to a DFA
    void tr_nfa_to_dfa(const Nfa &nfa);

public:
    Dfa():_start(new Node()){}
    Dfa(Node* const start):_start(start){}
    // construct a DFA by a regular expression.
    Dfa(const string& pattern);
    // construct a DFA by a NFA
    Dfa(const Nfa& nfa);

    // get the start node
    Node *get_start() const;

    // 1 if this DFA can match str, 0 otherwise.
    bool match(const string &str);

    // set a string stored by this DFA.
    void set_str(const string& str);
    // get next token by longest match. If fail, return false and roll back to eliminate the influence of this match.
    int get_next_token_longest(string& ret_token);
    // if there is next token.
    bool has_next_token();
    // skip next character. return false if in end of file.
    bool skip_next_char();
    // skip spaces following current position.
    bool skip_space();


private:
    // the start node.
    Node* _start;

    // the string stored in this DFA. used to get its token.
    string _str;
    // the position of current match
    int _pos = 0;
    // the length of _str
    int _len = 0;
};
#endif // DFA_H
