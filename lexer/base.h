#ifndef REG_BASE_H_INCLUDED
#define REG_BASE_H_INCLUDED


#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <queue>
#include <iostream>
#include <ctype.h>
#include <fstream>
using namespace std;

// 代表一条空边
constexpr char PATH_EPSILON = 0;
// 程序中字符集的下限
constexpr char PATH_BEGIN = 0;
// 程序组字符集的上限
constexpr char PATH_END = 127;


template <typename T>
void safe_delete(T* const p){
    if(p) delete p;
}


#endif // REG_BASE_H_INCLUDED
