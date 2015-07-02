#include "Nfa.h"

void Nfa::print_error(const string &msg) const{
    printf("%s\n", msg.c_str());
    exit(-1);
}

bool Nfa::match_select(const string& pattern, int& pos, const int& lim, Nfa& ret_nfa) const{
    for(int i=pos;i<lim;i++){
        if(i+2<lim && pattern[i+1]=='-'){
            for(char c=pattern[i];c<=pattern[i+2];c++)
                ret_nfa.get_start()->insert_path(ret_nfa.get_end(), c);
        }else ret_nfa.get_start()->insert_path(ret_nfa.get_end(), pattern[i]);
    }
    return true;
}

bool Nfa::match_character(const string &pattern, int &pos, const int& lim, Nfa& ret_nfa) const {
    char path_char = pattern[pos ++];
    if(path_char == '\\') path_char = pattern[pos ++];

    if(path_char == '.') for(path_char=PATH_BEGIN;path_char<PATH_END;++path_char)
            ret_nfa.get_start()->insert_path(ret_nfa.get_end(), path_char);
    else ret_nfa.get_start()->insert_path(ret_nfa.get_end(), path_char);

    return true;
}

bool Nfa::match_repeat(const string &pattern, int& pos, const int &lim, Nfa &ret_nfa) const{
    if(pos >= lim) return true;

    char test_char = pattern[pos];
    if(test_char == '+'){
        ret_nfa.get_end()->insert_path(ret_nfa.get_start(), PATH_EPSILON);
        ++ pos;
    }else if(test_char == '?'){
        ret_nfa.get_start()->insert_path(ret_nfa.get_end(), PATH_EPSILON);
        ++ pos;
    }else if(test_char == '*'){
        ret_nfa.get_end()->insert_path(ret_nfa.get_start(), PATH_EPSILON);
        ret_nfa.get_start()->insert_path(ret_nfa.get_end(), PATH_EPSILON);
        ++ pos;
    }
    return true;
}

bool Nfa::match_factor(const string &pattern, int &pos, const int &lim, Nfa &ret_nfa) const {
    char next_char;
    if(pattern[pos] == '[') next_char = ']';
    else if(pattern[pos] == '(') next_char = ')';
    else {
        match_character(pattern, pos, lim, ret_nfa);
        match_repeat(pattern, pos, lim, ret_nfa);
        return true;
    }

    int p = pos;
    while(p<lim && pattern[p]!=next_char) ++ p;
    if(p == lim) print_error("failed in match_factor");

    if(next_char == ']') match_select(pattern, ++pos, p, ret_nfa);
    else match_pattern(pattern, ++pos, p, ret_nfa);

    pos = p+1;
    match_repeat(pattern, pos, lim, ret_nfa);

    return true;
}

bool Nfa::match_pattern_unit(const string &pattern, int &pos, const int& lim, Nfa& ret_nfa) const {
    Node* last_node = ret_nfa.get_start();
    do{
        Nfa tmp_nfa;
        match_factor(pattern, pos, lim, tmp_nfa);

        last_node->insert_path(tmp_nfa.get_start(), PATH_EPSILON);
        last_node = tmp_nfa.get_end();
    }while(pos<lim && pattern[pos]!='|');
    last_node->insert_path(ret_nfa.get_end(), PATH_EPSILON);
    return true;
}


bool Nfa::match_pattern(const string &pattern, int &pos, const int& lim, Nfa& ret_nfa) const {
    do{
        Nfa tmp_nfa;
        match_pattern_unit(pattern, pos, lim, tmp_nfa);
        tmp_nfa.get_end()->set_end(true);

        // ºÏ²¢NFA
        ret_nfa.get_start()->insert_path(tmp_nfa.get_start(), PATH_EPSILON);
        tmp_nfa.get_end()->insert_path(ret_nfa.get_end(), PATH_EPSILON);
    }while(pos<lim && pattern[pos++]=='|');
    return true;
}

Nfa::Nfa(const string &pattern):_start(new Node()), _end(new Node()) {
    const int lim = pattern.length();
    int pos = 0;
    Nfa nfa(_start, _end);
    match_pattern(pattern, pos, lim, nfa);
}
