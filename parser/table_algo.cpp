#ifndef TABLE_ALGO_H_INCLUDED
#define TABLE_ALGO_H_INCLUDED

#include "types.h"

/**
    Éú³É×´Ì¬±í
*/
void parse_table(const vector<Rule>& rules, const set<State*>& states, vector<vector<Movement>>& table, vector<Symbol>&column){
    // get terminals and nonterminals
    set<Symbol> nonterminals_set, terminals_set;
    for(auto& state : states){
        for(auto& item : state->items){
            auto& real_rule = rules[item.id];
            nonterminals_set.insert(real_rule.nonterminal);
            terminals_set.insert(real_rule.nonterminal);

            for(auto& sm : real_rule.right) terminals_set.insert(sm);
        }
    }
    terminals_set.insert(TERMINAL);

    // push symbol into column
    for(auto& sm : terminals_set)  column.push_back(sm);
    sort(column.begin(), column.end(),  // sort column to make anti-end symbols are behind end symbols
         [&](const Symbol& s0, const Symbol& s1){return nonterminals_set.count(s0)<nonterminals_set.count(s1);});
    map<Symbol, int> symbol_index;
    for(int i=0;i<column.size();++i) symbol_index[column[i]] = i;

    // get state vector and sort it by state id
    vector<State*> state_vec(states.begin(), states.end());
    sort(state_vec.begin(), state_vec.end(), [](State* s0, State* s1){return s0->id < s1->id;});

    // generate table
    for(auto& st : state_vec){
        table.push_back(vector<Movement>());
        for(int i=0;i<column.size();++i) table.back().push_back(Movement{Movement::ERROR});
        for(auto& it : st->edges){
            auto& sm = it.first;
            auto& next_state = it.second;
            auto col = symbol_index[sm];
            table.back()[col].type = Movement::SHIFT;
            table.back()[col].num = next_state;
        }

        for(auto& rule : st->items){
            auto real_rule = rules[rule.id];
            if(real_rule.right.size() != rule.pos) continue;
            for(auto& next_symbols : rule.next_symbols){
                auto col = symbol_index[next_symbols];
                table.back()[col].num = rule.id;
                if(rule.id) table.back()[col].type = Movement::REDUCE;
                else table.back()[col].type = Movement::ACCEPT;
            }
        }
    }
}

void print_table(vector<vector<Movement>>& table, vector<Symbol>&column, ostream& out){
    char buf[1024], rb[1024];
    sprintf(rb, "%-12s", "state :move\t");
    out<<rb;

    for(auto& sm : column){
        sprintf(rb, "%7s\t", sm.c_str());
        out<<rb;
    }

    out<<endl;
    int state_num = 0;
    for(auto& line : table){
        sprintf(rb, "state%-7d\t", state_num++);
        out<<rb;
        for(auto& mv : line){
            if(mv.type == Movement::SHIFT) sprintf(buf, "s%d", mv.num);
            else if(mv.type == Movement::REDUCE) sprintf(buf, "r%d", mv.num);
            else if(mv.type == Movement::ACCEPT) sprintf(buf, "ac");
            else sprintf(buf, " ");
            sprintf(rb, "%7s\t", buf);
            out<<rb;
        }
        out<<endl;
    }
}

#endif // TABLE_ALGO_H_INCLUDED
