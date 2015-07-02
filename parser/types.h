#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include "base.h"

// hash函数的返回值
typedef unsigned long long type_hash;

// 表示终结符或非终结符的类型
typedef string Symbol;

// 结束符
const Symbol TERMINAL = "$";

// 代表一条空边
const Symbol EPSILON = "@";

// represent grammar rule
struct Rule{
    Symbol nonterminal;

    vector<Symbol> right;
    int id;
    // id counter
    static int s_id;
};


// represent state during LALR(1) parse
struct State{
    // LALR(1) Item
    struct Item{
        int id; // the rule id
        int pos; // the distinguished position
        set<Symbol> next_symbols; // the set of next symbols

        bool operator < (const Item& s) const{
            if(id == s.id) return pos < s.pos;
            return id < s.id;
        }
    };

    int id;
    // the items of this state
    set <Item> items;
    // the edges of this state
    map<Symbol, int> edges;

    // id counter
    static int s_state_id;
};

// Movement in state table
struct Movement{
    enum {ERROR, SHIFT, REDUCE, ACCEPT};
    int type, num;
};

#endif // TYPES_H_INCLUDED
