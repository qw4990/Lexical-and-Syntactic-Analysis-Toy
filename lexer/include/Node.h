/**
    节点类, 抽象DFA和NFA中的节点信息
*/

#ifndef REGNODE_H
#define REGNODE_H

#include "../base.h"

class Node {

public:
    Node():_is_end(false){};
    Node(bool is_end):_is_end(is_end){};

    // 增加一条边
    void insert_path(Node* const next_node, const char path_char = PATH_EPSILON);
    // 重置一类边
    void set_path(Node* const next_node, const char path_char);
    // 删除一类边
    void erase_path(const char path_char);

    bool is_end_node() const { return _is_end; }
    void set_end(bool is_end) { _is_end = is_end; }

    // 返回一个点集, 点集中所有点能从这个节点开始, 通过path_char类边能访问到
    void next(char const path_char, set<Node *> &node_set, bool clear_set) const;

    // 返回从当前节点通过path_char能访问到的任意一个节点
    // 如果不存在, 返回null
    Node* const next(char path_char) const;


private:
    // 用来保存节点的边集, 因为在NFA中同一个字符代表的边(一类边)可能有多条, 所以用multimap而不是map
    multimap<char, Node *> _path_map;

    // 当前节点是否为终结节点.
    bool _is_end;
};

#endif // REGNODE_H
