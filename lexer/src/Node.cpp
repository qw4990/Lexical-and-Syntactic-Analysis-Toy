#include "Node.h"

void Node::next(const char path_char, set<Node *>& node_set, bool clear_set = true)  const {
    if(clear_set) node_set.clear();
    auto its = _path_map.equal_range(path_char);
    auto it = its.first;
    while(it != its.second){
        node_set.insert(it->second);
        ++ it;
    }
}

Node *const Node::next(char path_char) const {
    auto its = _path_map.equal_range(path_char);
    if(its.first == its.second) return nullptr;
    return its.first->second;
}

void Node::insert_path(Node* const next_node, const char path_char){
    _path_map.insert(make_pair(path_char, next_node));
}

void Node::erase_path(const char path_char){
    _path_map.erase(path_char);
}

void Node::set_path(Node *const next_node, const char path_char) {
    erase_path(path_char);
    insert_path(next_node, path_char);
}
