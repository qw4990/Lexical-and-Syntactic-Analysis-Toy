#ifndef STATE_ALGO_H_INCLUDED
#define STATE_ALGO_H_INCLUDED

#include "types.h"

extern int State::s_state_id;

void print_state(const vector<Rule>& rules, const set<State*>& state_set, ostream& out){
    // firstly, sort states by state id
    vector<State*> state_vec(state_set.begin(), state_set.end());
    sort(state_vec.begin(), state_vec.end(), [](State* s0, State* s1){return s0->id < s1->id;});
    out<<"state size: "<<state_vec.size()<<endl;

    // function: print a item
    auto print_item = [&](const State::Item& item){
        auto& real_rule = rules[item.id];
        out<<"\t\t"<<real_rule.nonterminal<<" ->  ";
        for(int i=0;i<real_rule.right.size();++i){
            if(i == item.pos) out<<".";
            out<<real_rule.right[i];
            if(item.pos==real_rule.right.size() && i==real_rule.right.size()-1) out<<".";
            out<<" ";
        }
        out<<", ";
        for(auto& a : item.next_symbols) out<<a<<" ";
        out<<endl;
    };

    for(auto& pstate : state_vec){
        out<<"state id: "<<pstate->id<<endl;
        out<<"\trules : " <<pstate->items.size()<<endl;
        for(auto& item : pstate->items) print_item(item);
        out<<"\tedge : " << pstate->edges.size()<<endl;
        for(auto& edge : pstate->edges) out<<"\t\t"<<edge.first<<" -> "<<edge.second<<endl;
        out<<endl;
    }
    out<<endl<<endl<<endl;
}


/**
    @brief 用LALR(1)解析规则并生成DFA
    @return 0或-1
    PS: LALR(1)的核心部分, 这里写的过于紧密了, 不少小函数全部用匿名函数写在大函数内, 算是一个失败的地方.
*/
int parse_state(const vector<Rule>& rules, set<State*>& state_set){
    // create index for nonterminals.
    map<Symbol, set<int> > nonterminals_index;
    for(auto& rule : rules) nonterminals_index[rule.nonterminal].insert(rule.id);

    map<Symbol, set<Symbol>> first_map; //  first set of each symbol.
    // generate first set for each symbol
    while(true){
        bool flag = false;

        for(auto& rule : rules){
            auto nonterminal = rule.nonterminal;
            auto right = rule.right;

            for(auto& symbol : right){
                if(nonterminals_index.count(symbol)){ // if it is nonterminal
                    for(auto& sm : first_map[symbol])
                        if(first_map[nonterminal].count(sm) == 0){
                            flag = true;
                            first_map[nonterminal].insert(sm);
                        }
                    if(first_map[nonterminal].count(EPSILON) == 0) break;
                }else{
                    if(first_map[nonterminal].count(symbol) == 0){
                        flag = true;
                        first_map[nonterminal].insert(symbol);
                    }
                    break;
                }
            }
        };

        if(!flag) break;
    }

    // function: used to hash a item.
    auto hash_state_item = [](const State::Item& item){
        static const type_hash SEED = 13331;
        return item.id * SEED + item.pos;
    };
    // function: used to hash a state.
    auto hash_state = [&](const State& state){
        static const type_hash SEED = 13333331;
        type_hash ret = 0;
        for(auto& item : state.items) ret = ret*SEED + hash_state_item(item);
        return ret;
    };

    // function: used to combine item2's next_symbols to item1's
    auto combine_item = [&](State::Item& item1, const State::Item& item2){
        for(auto& h : item2.next_symbols) item1.next_symbols.insert(h);
    };

    // function: used to combine state2 to state1
    auto combine = [&](State& s1, State& s2){
        set<State::Item>::iterator it1 = s1.items.begin();
        for(auto& item : s2.items) combine_item(const_cast<State::Item&>(*it1++), item);
    };

    // function: generate state itself to a closure
    auto closure = [&](State& state){
        map<type_hash, State::Item*> item_set;    // store items contained by state.
        for(auto& item : state.items) item_set[hash_state_item(item)] = const_cast<State::Item*>(&item);

        // function: get next symbols of syms
        auto get_next_symbols = [&](vector<Symbol>& syms, set<Symbol>& next_symbols){
            if(syms.size() == 0){
                next_symbols.insert(EPSILON);
                return ;
            }
            next_symbols.clear();
            for(int i=0;i<syms.size();++i){
                auto& symbol = syms[i];

                if(nonterminals_index.count(symbol) == 0){ // if symbol is a terminal, break.
                    next_symbols.insert(symbol);
                    break;
                }

                // if symbols is a nonterminal
                bool has_epsilon = false;
                for(auto& sm : first_map[symbol]){
                    if(sm == EPSILON) {has_epsilon = true; continue;}
                    next_symbols.insert(sm);
                }
                if(!has_epsilon) break;
                // if each first set of symbols in syms contain EPSILON, next_symbols should contain EPSILON.
                if(has_epsilon && i==syms.size()-1) next_symbols.insert(EPSILON);
            }
        };

        while(true){    // update state until fail. If fail, then the closure is completed.
            // check each item and update
            for(auto& item : state.items){
                auto real_rule = rules[item.id];
                if(item.pos == real_rule.right.size()) continue;  // ignore complete item

                auto symbol = real_rule.right[item.pos]; // get next symbol

                vector<Symbol> syms;
                set<Symbol>next_symbols;
                for_each(real_rule.right.begin()+item.pos+1, real_rule.right.end(), [&](const Symbol& sm){syms.push_back(sm);});
                // get next symbols of this item
                get_next_symbols(syms, next_symbols);
                if(next_symbols.count(EPSILON)){
                    for(auto& sm : item.next_symbols) next_symbols.insert(sm);
                    next_symbols.erase(EPSILON);
                }

                for(auto& id : nonterminals_index[symbol]){
                    auto new_item = new State::Item{id, 0};
                    new_item->next_symbols = next_symbols;
                    auto code = hash_state_item(*new_item);

                    if(item_set.count(code)) { //  if it is contained by this state, do combine action
                        auto& next_symbols1 = (*item_set[code]).next_symbols;
                        auto& next_symbols2 = new_item->next_symbols;

                        if(includes(next_symbols1.begin(), next_symbols1.end(),
                                    next_symbols2.begin(), next_symbols2.end())) continue;

                        combine_item(*item_set[code], *new_item);
                        delete new_item;
                    }else  { // if this item is not contained by this state, add it to this state.
                        item_set[code] = new_item;
                    }

                    state.items.erase(*item_set[code]);
                    state.items.insert(*item_set[code]);
                    goto SUCCESS;
                }
            }
            break;

            SUCCESS:;
        }
    };

    // use bfs to create LALR(1) DFA
    State::Item start_item{rules[0].id, 0};
    start_item.next_symbols.insert(TERMINAL);
    State *start_state = new State;
    start_state->items.insert(start_item);
    start_state->id = State::s_state_id ++;
    // firstly, generate start_state itself to a closure
    closure(*start_state);

    queue<State*> state_que;
    map<type_hash, State*> state_map;
    state_map[hash_state(*start_state)] = start_state;
    state_que.push(start_state);

    set<Symbol> symbols;
    for(auto& rule : rules){
        symbols.insert(rule.nonterminal);
        for(auto& sym : rule.right)
            symbols.insert(sym);
    }

    while(!state_que.empty()){
        auto& now_state = *state_que.front();
        state_que.pop();

        // check each symbol to construct next state
        for(auto& next_sym : symbols){
            bool flag = 0;
            State *next_state = new State;

            for(auto& now_item : now_state.items){
                auto real_rule = rules[now_item.id];
                if(real_rule.right.size() == now_item.pos) continue; // ignore complete item
                if(next_sym != real_rule.right[now_item.pos]) continue;

                flag = 1;
                // add this item into next_state
                State::Item next_item = now_item;
                ++ next_item.pos;

                next_state->items.insert(next_item);
            }
            if(!flag){
                delete next_state;
                continue;
            }

            closure(*next_state);
            auto next_hash = hash_state(*next_state);

            if(state_map.count(next_hash)){  // if the next_state has already existed,  do combine action.
                bool contain = 1;
                {
                    auto &st1 = state_map[next_hash]->items;
                    auto &st2 = next_state->items;
                    contain = (includes(st1.begin(), st1.end(), st2.begin(), st2.end()));
                }
                if(contain){
                    auto it1 = state_map[next_hash]->items.begin();
                    auto it2 = next_state->items.begin();
                    while(it2 != next_state->items.end()){
                        auto &st1 = (*it1).next_symbols;
                        auto &st2 = (*it2).next_symbols;
                        contain = (includes(st1.begin(), st1.end(), st2.begin(), st2.end()));
                        if(!contain) break;
                        ++ it1;
                        ++ it2;
                    }
                }

                if(!contain){
                    combine(*state_map[next_hash], *next_state);
                    state_que.push(state_map[next_hash]);
                }
                delete next_state; // delete next_state to save space.
            }else{  // else create a new state.
                next_state->id = State::s_state_id ++;
                state_map[next_hash] = next_state;
                state_que.push(state_map[next_hash]);
            }

            now_state.edges[next_sym] = state_map[next_hash]->id; // add a edge into now_state
        }
    }

    for(auto& it : state_map) state_set.insert(it.second);

    return 0;
}

#endif // STATE_ALGO_H_INCLUDED
