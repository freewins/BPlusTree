//
// Created by Freewings on 2025/3/17.
//

#ifndef BPLUSTREE_HPP
#define BPLUSTREE_HPP
#include <functional>
#include <fstream>
#include <string>

template<
  class T,
  class Key,
  class Compare = std::less<T>,
  int degree = 10
>
class BPlusTree {
private:
  /**
   * 文件头
   */
  struct FileHeader {
    long long root_offset; //根节点偏移量
    int node_count; // 节点数量
    int height; //树高
    //long_long freefile
  };

  /**
   * 节点头 公用的内容
   */
  struct NodeHeader {
    bool is_leaf; //是否是叶节点
    long long next_node_offset; //下一个节点的偏移位置
    long long father_offset; // 父节点
    long long count_nodes; //存储节点数量
  };

  /**
   * 树中的内部节点，保存了头部和键以及键对应的子节点的位置
   */
  struct InternalNode {
    NodeHeader header; //节点头
    Key keys_[degree - 1]; //键值 为degree - 1
    long long children_offset[degree]; // 孩子的偏移值 ，标记了孩子节点的位置
  };
  /**
   *数据节点，存储了所有的叶节点对应的数据以及键值
   */
  struct LeafNode {
    NodeHeader header; // 节点头
    Key keys_[degree]; // 键值
    T values_[degree]; //数据
  };

  std::string path_;

};
#endif //BPLUSTREE_HPP
