#include "Dfa.h"

bool Dfa::contain_end_node(const State& state) const {
    for(auto& node : state)
        if(node->is_end_node())
            return true;
    return false;
}

typename Dfa::HashCodeType Dfa::hash_code(State &st) const {
    static const HashCodeType seed = 13331;
    HashCodeType ret = 0;
    for(auto& node : st){
        ret = ret*seed + reinterpret_cast<HashCodeType>(node);
    }
    return ret;
}

void Dfa::next(Node *const start, State &ret_state, const char path_char, bool clear_set) const {
    if(clear_set)   ret_state.clear();
    State start_state;
    closure(start, start_state, false);
    for(auto& node : start_state)
        if(node->next(path_char))
            closure(node->next(path_char), ret_state, false);
}

void Dfa::next(const State &start_state, State &ret_state, const char path_char) const {
    ret_state.clear();
    for(auto& node : start_state)
        next(node, ret_state, path_char, false);
}

void Dfa::closure(const State& start_state, State& ret_state) const {
    ret_state.clear();
    for(auto& tmp_node : start_state)
        closure(tmp_node, ret_state, false);
}

void Dfa::closure(Node* const start, State& ret_state, bool clear_set) const {
    if(clear_set)   ret_state.clear();
    queue<Node*> tmp_q;
    State tmp_set;
    tmp_q.push(start);
    ret_state.insert(start);

    while(!tmp_q.empty()){
        auto t_node = tmp_q.front();
        tmp_q.pop();
        t_node->next(PATH_EPSILON, tmp_set, true);
        for(auto& tmp_node : tmp_set){
            if(ret_state.end() != ret_state.find(tmp_node))
                continue;
            ret_state.insert(tmp_node);
            tmp_q.push(tmp_node);
        }
    }
}

void Dfa::tr_nfa_to_dfa(const Nfa &nfa){
    State start_state;
    closure(nfa.get_start(), start_state, true);
    auto start_hash_code = hash_code(start_state);

    map<HashCodeType, State*> node_set_map;
    map<HashCodeType, Node*> node_map;
    queue<HashCodeType> node_set_queue;
    node_set_map[start_hash_code] = &start_state;
    node_map[start_hash_code] = new Node();
    node_set_queue.push(start_hash_code);

    while(!node_set_queue.empty()){
        auto code_now = node_set_queue.front();
        node_set_queue.pop();
        auto set_now = node_set_map[code_now];
        auto node_now = node_map[code_now];

        for(char path_char=PATH_BEGIN;path_char<PATH_END;++path_char){
            auto set_next = new State();
            next(*set_now, *set_next, path_char);
            if(set_next->size() == 0) continue;
            auto code_next = hash_code(*set_next);

            bool is_new_set = false;
            if(node_map.end() == node_map.find(code_next)){
                node_map[code_next] = new Node(contain_end_node(*set_next));
                node_set_map[code_next] = set_next;
                node_set_queue.push(code_next);
                is_new_set = true;
            }

            auto node_next = node_map[code_next];
            node_now->set_path(node_next, path_char);

            if(!is_new_set){
                safe_delete(set_next);
            }
        }
    }

    _start = node_map[start_hash_code];
}

Dfa::Dfa(const Nfa &nfa) {
    tr_nfa_to_dfa(nfa);
}

Dfa::Dfa(const string &pattern) {
    tr_nfa_to_dfa(Nfa(pattern));
}

bool Dfa::match(const string &str) {
    Node* u = get_start();
    auto len = str.length();
    decltype(len) i;
    for(i=0;i<len;++i){
        char c = static_cast<char>(str[i]);
        if(nullptr == u->next(c)) return false;
        u = u->next(c);
    }
    return u->is_end_node();
}

Node *Dfa::get_start() const {
    return _start;
}

void Dfa::set_str(const string &str) {
    _str = str;
    _pos = 0;
    _len = _str.length();
}

int Dfa::get_next_token_longest(string &ret_token) {
    ret_token == "";
    if(!skip_space()) return 1;
    const Node* node = _start;
    auto start_pos = _pos;
    while(_pos < _len){
        char path_char = static_cast<char>(_str[_pos]);

        if(node->next(path_char) == nullptr){
            if(node->is_end_node()){
                ret_token = _str.substr(start_pos, _pos - start_pos);
                return 0;
            }else {
                _pos = start_pos;
                return -1;
            }
        }

        node = node->next(path_char);
        ++_pos;
    }

    if(node->is_end_node()){
        ret_token = _str.substr(start_pos, _pos - start_pos);
        return 0;
    }else {
        _pos = start_pos;
        return -1;
    }
}

bool Dfa::has_next_token() {
    return _pos < _len;
}

bool Dfa::skip_next_char() {
    if(_pos == _len) return false;
    ++ _pos;
    return true;
}

bool Dfa::skip_space(){
    while(has_next_token()){
        char path_char = static_cast<char>(_str[_pos]);
        if(isspace(path_char)) skip_next_char();
        else return true;
    }
    return false;
}

