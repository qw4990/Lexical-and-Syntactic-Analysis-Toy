#ifndef RULE_ALGO_H_INCLUDED
#define RULE_ALGO_H_INCLUDED

#include "types.h"
#include "utilities.h"

extern int Rule::s_id;

void print_rule(const vector<Rule>& rules, ostream& out){
    out<<"rules: "<<rules.size()<<endl;
    for(auto& rule : rules){
        out<<"\t"<<rule.nonterminal<<"->";
        for(auto& right : rule.right){
            out<<right<<" ";
        }
        out<<endl;
    }
    out<<endl<<endl<<endl;
}


/**
    @brief 解析规则
    @return 成功返回0, 失败返回-1.
*/
int parse_rule(vector<string>& lines, vector<Rule>& rules){
    for(auto& line : lines){
        if(line.size() == 0) continue;
        if(line[0]=='#') continue; // ignore comments
        if(line[0]=='$' || line[0]=='%') continue; // ignore lexical rules;
        auto pos = line.find("->");
        if(pos == std::string::npos) {
            warning("parse_rule", "rule: ", line, " is invalid");
            return -1;
        }

        // get the nonterminal on the left
        auto nonterminal = trim(line.substr(0, pos));

        // split the right string by '|', then split each result part by blank or \t.
        // finally each result part is a symbol (nonterminal or terminal).
        vector<string> parts_right;
        string right_str = line.substr(pos + 2);
        split(parts_right, right_str, "|");

        for(auto& part : parts_right){
            vector<Symbol> right;
            split(right, part, "\t ");
            if(right.begin()->length() == 0) right.erase(right.begin());
            if(right.back().length() == 0) right.erase(right.end()-1);

            rules.push_back(Rule{nonterminal, right, Rule::s_id ++});
        };
    };

    return 0;
}

#endif // RULE_ALGO_H_INCLUDED
