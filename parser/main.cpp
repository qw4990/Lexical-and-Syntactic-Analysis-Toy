#include "base.h"
int parser(const string& bfn, const string& lex, const string& output);

int main(){
    parser("C:\\Users\\zhangyuanjia\\Desktop\\LALR1_lexer_parser\\data\\bfn.data",
                   "C:\\Users\\zhangyuanjia\\Desktop\\LALR1_lexer_parser\\data\\lex.data",
                   "C:\\Users\\zhangyuanjia\\Desktop\\LALR1_lexer_parser\\data\\parse.data");
}
