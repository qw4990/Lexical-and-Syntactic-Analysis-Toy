/**
    语法分析部分
*/

#include "types.h"
#include "utilities.h"

void print_rule(const vector<Rule>& rules, ostream&);
int parse_rule(vector<string>& lines, vector<Rule>& rules);
void print_state(const vector<Rule>& rules, const set<State*>& state_set, ostream&);
int parse_state(const vector<Rule>& rules, set<State*>& state_set);
void parse_table(const vector<Rule>& rules, const set<State*>& states, vector<vector<Movement>>& table, vector<Symbol>&column);
void print_table(vector<vector<Movement>>& table, vector<Symbol>&column, ostream&);

// 用状态表table, 对词法分析结果(lex)进行语法分析
// 成功返回0, 失败返回-1.
int parse_lex(vector<Rule>& rules, vector<vector<Movement>>& table, vector<Symbol>& column, const string& lex){
    // token类型
    struct Token{
        int line_no;
        Symbol type, symbol; // token type and it content.
    };

    // 读取lex文件
    deque<Token> tokens;
    vector<string> lines;
    read_vect(lex,  lines);

    // 先查看是否有词法错误
    for(auto& line : lines){
        vector<string> str;
        split(str, line, " \n\r\t");
        if(str[1] == "ERROR"){
            error("line no:", line);
            return -1;
        }

        tokens.push_back(Token{atoi(str[0].c_str()), str[1], str[2]});
    }
    tokens.push_back(Token{tokens.size(), "UNKNOW", TERMINAL});

    map<Symbol, int> symbol_index;
    for(size_t i=0;i<column.size();++i) symbol_index[column[i]] = i;

    // state_sk为状态栈, 用来模拟状态表的解析过程
    stack<int> state_sk;
    state_sk.push(0);
    Symbol next_tok;
    while(!tokens.empty()){
        int state = state_sk.top();
        Movement mov;
        mov.type = Movement::ERROR;

        next_tok = tokens.front().type;
        if(symbol_index.count(next_tok)) mov = table[state][symbol_index[next_tok]];

        if(mov.type == Movement::ERROR && symbol_index.count(tokens.front().symbol)){
            next_tok = tokens.front().symbol;
            mov = table[state][symbol_index[next_tok]];
        }

        if(mov.type == Movement::ERROR){
            error("line no: ", tokens.front().line_no, " ; error token: ", next_tok);
            return -1;
        }else if(mov.type == Movement::SHIFT){
            state_sk.push(mov.num);
            tokens.pop_front();
        }else if(mov.type == Movement::REDUCE){
            int rule_id = mov.num;
            int t = rules[rule_id].right.size();
            while(t --) state_sk.pop();

            tokens.push_front(Token{tokens.front().line_no, "UNKNOW", rules[rule_id].nonterminal});
        }else if(mov.type == Movement::ACCEPT){
            puts("SUCCESS!!");
            return 0;
        }
    }

    return -1;
}

/**
    @brief  根据bfn的规则, 对词法分析结果(lex)进行语法分析parse
    @return 成功返回0, 失败返回-1
*/
int parser(const string& bfn, const string& lex, const string& output){
    ofstream fout(output);
    if(!fout.is_open()){
        error("can't open ", output);
        return -1;
    }

    // 读取并解析bfn文件规则
    vector<string> lines;
    read_vect(bfn,  lines);
    vector<Rule> rules;
    parse_rule(lines, rules);

    // print rules
    print_rule(rules, fout);

    // 用LALR(1)算法， 解析出DFA
    set<State*> state_set;
    parse_state(rules, state_set);

    // print states in this DFA
    print_state(rules, state_set, fout);

    // 生成状态表
    vector<vector<Movement>>table;
    vector<Symbol>column;
    parse_table(rules, state_set, table, column);

    // print state table
    print_table(table, column, fout);

    //  进行语法解析
    return parse_lex(rules, table, column, lex);
}
