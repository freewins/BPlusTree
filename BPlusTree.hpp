//
// Created by Freewings on 2025/3/17.
//

#ifndef BPLUSTREE_HPP
#define BPLUSTREE_HPP
#include <cstring>
#include <functional>
#include <fstream>
#include <string>
#include <memory>

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

    FileHeader() {
      root_offset = sizeof(FileHeader);
      node_count = 0;
      height = 0;
    }
  };

  /**
   * 节点头 公用的内容
   */
  struct NodeHeader {
    bool is_leaf; //是否是叶节点
    long long offset; //自己所在的位置
    long long next_node_offset; //下一个节点的偏移位置
    long long father_offset; // 父节点
    long long count_nodes; //存储节点数量

    NodeHeader() {
      is_leaf = false;
      offset = next_node_offset = count_nodes = 0;
      father_offset = -1;
    }

    NodeHeader &operator=(const NodeHeader& rhs) = default;
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

  const std::string path_;
  std::fstream file_;
  FileHeader * file_header_;
  NodeHeader * node_header_root_;
  InternalNode * internal_node_root_;
  LeafNode * leaf_node_root_;
  Compare compare_;

  //
  /**
   * 返回大于当前的值的位置 用于internal的内部查找
   * @param key 查找值
   * @param size key数组大小
   * @param key_values 查找的数组
   * @return 返回索引值 如果大于最大值 返回值为size
   */
  int Upper_Bound(const Key & key,const Key * key_values, const int size) const;

  /**
   * 返回大于等于当前值的位置 ， 出现等于 返回 -1 * index
   * @param key 查找值
   * @param size key_values 数组大小
   * @param key_values 查找的数组
   * @return 返回索引值，大于最大值 返回 size
   */
  int Lower_Bound(const Key & key,const Key * key_values, const int size) const;
  //从文件中读入文件头
  void ReadFileHeader(std::fstream & file,FileHeader * & file_header) ;

  void ReadNodeHeader(std::fstream & file,NodeHeader * & node_header,long long pos) ;

  void ReadInternalNode(std::fstream & file,InternalNode * & internal_node,long long pos) ;

  void ReadLeafNode(std::fstream & file,LeafNode * & leaf_node,long long pos) ;

public:
  BPlusTree(const std::string& path);

  bool insert(const Key & key, const T & value);
};

template<class T, class Key, class Compare, int degree>
void BPlusTree<T, Key, Compare, degree>::
ReadFileHeader(std::fstream &file, FileHeader *&file_header) {
  if (file.fail()) {
    return;
  }
  file.seekg(0, std::ios::beg);
  file.read(reinterpret_cast<char*>(file_header), sizeof(FileHeader));
}

template<class T, class Key, class Compare, int degree>
void BPlusTree<T, Key, Compare, degree>::
ReadNodeHeader(std::fstream &file, NodeHeader *&node_header,long long pos) {
  if (file.fail()) {
    return;
  }
  file.seekg(pos, std::ios::beg);
  file.read(reinterpret_cast<char*>(node_header), sizeof(NodeHeader));

}


template<class T, class Key, class Compare, int degree>
void BPlusTree<T, Key, Compare, degree>::ReadLeafNode(std::fstream &file, LeafNode *&leaf_node, long long pos) {
  if (file.fail()) {
    return;
  }
  file.seekg(pos, std::ios::beg);
  file.read(reinterpret_cast<char*>(leaf_node), sizeof(LeafNode));
}
template<class T, class Key, class Compare, int degree>
void BPlusTree<T, Key, Compare, degree>::ReadInternalNode(std::fstream &file, InternalNode *&internal_node, long long pos) {
  if (file.fail()) {
    return;
  }
  file.seekg(pos, std::ios::beg);
  file.read(reinterpret_cast<char*>(internal_node), sizeof(InternalNode));
}


template<class T, class Key, class Compare, int degree>
int BPlusTree<T, Key, Compare, degree>::Upper_Bound(const Key &key, const Key * key_values,const int size) const {
  int left = 0 , right = size - 1;
  int mid = 0;
  while (left <= right) {
    mid = (left + right) / 2;
    if (compare_(key,key_values[mid])) {
      right = mid - 1;
    }
    else {
      left = mid + 1;
    }
  }
  return left;
}

template<class T, class Key, class Compare, int degree>
int BPlusTree<T, Key, Compare, degree>::Lower_Bound(const Key &key, const Key * key_values, const int size) const {
  int left = 0,right = size -1;
  int mid = 0;
  while (left <= right) {
    mid = (left + right) / 2;
    if (compare_(key_values[mid],key)) {
      left = mid + 1;
    }
    else if (compare_(key,key_values[mid])) {
      right = mid - 1;
    }
    else {
      if (mid == 0)
        return -1;
      return -1 * mid;
    }
  }
  return left;
}



template<class T, class Key, class Compare, int degree>
BPlusTree<T, Key, Compare, degree>::BPlusTree(const std::string &path) {
  file_.open(path_,std::ios::binary|std::ios::in|std::ios::out);
  if (!file_.is_open()) {
    file_.open(path_,std::ios::binary|std::ios::out);
    file_.close();
    file_.open(path_,std::ios::binary|std::ios::in|std::ios::out);
    this -> file_header_ = new FileHeader();
    this -> node_header_root_ = new NodeHeader();
    this -> node_header_root_ -> is_leaf = true;
    this -> node_header_root_ -> offset = file_header_ -> root_offset;
  }
  //TODO 在这里一定要写一个为空时写入叶子节点的数据
  else {
    this->ReadFileHeader(file_,file_header_);
    this->ReadNodeHeader(file_,node_header_root_,file_header_->root_offset);
  }
  if (this -> node_header_root_ -> is_leaf) {
    this -> ReadLeafNode(file_,leaf_node_root_,file_header_->root_offset);
  }
  else {
    this -> ReadInternalNode(file_,internal_node_root_,file_header_->root_offset);
  }
}



template<class T, class Key, class Compare, int degree>
bool BPlusTree<T, Key, Compare, degree>::insert(const Key &key, const T &value) {
  //Solution 1 : 不需要把root 中内容存到内存中，直接从pos 开始循环 好处：可以进行良好的结构设计  坏处: 没法做成缓存，速度变慢
  //采用solution 1
  //Solution 2 : 把root 放在内存中，可以减少一定的硬盘 io 但是结构设计比较丑陋
  NodeHeader * cur = this -> node_header_root_;
  InternalNode * cur_internal_node = new InternalNode();
  while (!cur -> is_leaf) {

    this -> ReadInternalNode(file_,cur_internal_node,cur->offset);
    //找到大于等于它的最大值
    int index = Upper_Bound(key,cur_internal_node->keys_,cur->count_nodes);
    this -> ReadNodeHeader(file_,cur,cur_internal_node -> children_offset[index]);
  }
  delete cur_internal_node;
  //到达叶节点
  LeafNode * cur_leaf_node  = new LeafNode();
  this -> ReadLeafNode(file_, cur_leaf_node,cur->offset);
  int index = Lower_Bound(key,cur_leaf_node->keys_,cur->count_nodes);
  if (index < 0) {
    delete cur_leaf_node;
    return false;
  }
  else {
    Key * cur_key = new Key[degree];
    T * cur_value = new T[degree];
    memcpy(cur_key,cur_leaf_node->keys_+index,sizeof(Key) * (cur -> count_nodes - index));
    memcpy(cur_value,cur_leaf_node->values_+index,sizeof(T) * (cur -> count_nodes - index));
    cur_leaf_node -> keys_[index] = key;
    cur_leaf_node -> values_[index] = value;
    memcpy(cur_leaf_node->values_+index + 1,cur_value,sizeof(T) * (cur -> count_nodes - index));
    memcpy(cur_leaf_node->keys_+index + 1,cur_key,sizeof(T) * (cur -> count_nodes - index));
    delete cur_value;
    delete cur_key;
    ++cur->count_nodes;
    ++this->file_header_->count_nodes; //TODO is necessary???
    //TODO Split
  }
  if (cur != this -> node_header_root_) {
    this -> ReadNodeHeader(file_,cur,this->file_header_->root_offset);
  }
  delete cur_leaf_node;
  return true;
}

#endif //BPLUSTREE_HPP
