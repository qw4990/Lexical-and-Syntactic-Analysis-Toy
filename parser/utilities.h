#ifndef UTILITIES_H_INCLUDED
#define UTILITIES_H_INCLUDED

#include "base.h"



// these functions used to log
template <typename T>
void print_(T t){
    std::cout<<t<<std::endl;
}
template <typename T, typename ... ARG>
void print_(T t, ARG... args){
    std::cout<<t<<" ";
    print_(args...);
}
template <typename ... ARG>
void error(ARG... args){
    std::cout<<"[ERROR]: ";
    print_(args...);
}
template <typename ... ARG>
void warning(ARG... args){
    std::cout<<"[warning]: ";
    print_(args...);
}


static string trim(const string& str){
    int l = 0, r = str.length();
    while(isspace(str[l])) ++l;
    while(isspace(str[r-1])) --r;
    return str.substr(l, r);
}


// split函数, 根据char_set内的字符切割
static void split(vector<string>& ret, const string& str, const string& char_set){
    auto contain = [&](const char& c) {return char_set.find(c) != string::npos;};
    auto get_head = [&](string &s){
        int l = 0;
        while(l<s.length() && contain(s[l])) ++l;
        if(l == s.length()) return false;
        int r = l;
        while(r<s.length() && !contain(s[r])) ++r;
        ret.push_back(s.substr(l, r-l));
        s = s.substr(r);
        return true;
    };

    string tmp = str;
    while(get_head(tmp));
};


static bool read_vect(const string& path, vector<string>& lines){
    ifstream fin(path.c_str());
    if(!fin.is_open()){
        error("read_vect: can't open", path);
        return false;
    }

    string line;
    while(getline(fin, line)) lines.push_back(line);

    fin.close();
    return true;
}

#endif // UTILITIES_H_INCLUDED
