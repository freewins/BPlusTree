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
#include "pair.hpp"
//TODO 引入一个标准常数 避免出现溢出
template<
  class T,
  class Key,
  int degree = 10,
 class Compare = std::less<Key>,
 class Compare_ = std::less<T>
>
class BPlusTree {
private:

  using Kp = sjtu::pair<Key,T>;
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
    long long father_offset; // 父节点
    long long count_nodes; //存储节点数量

    NodeHeader() {
      is_leaf = false;
      offset = count_nodes = 0;
      father_offset = -1;
    }

    NodeHeader &operator=(const NodeHeader& rhs) = default;
  };

  /**
   * 树中的内部节点，保存了头部和键以及键对应的子节点的位置
   */
  struct InternalNode {
    NodeHeader header; //节点头
    Kp keys_[degree]; //键值 为degree - 1
    long long children_offset[degree + 1]={}; // 孩子的偏移值 ，标记了孩子节点的位置
    InternalNode() {
      memset(keys_, 0, sizeof(keys_));
    }
  };
  /**
   *数据节点，存储了所有的叶节点对应的数据以及键值
   */
  struct LeafNode {
    NodeHeader header; // 节点头
    long long pre_node_offset; //上一个节点的偏移位置
    long long next_node_offset; //下一个节点的偏移位置
    Kp values[degree];


    LeafNode() {
      memset(values, 0, sizeof(values));
      header.is_leaf = true;
      pre_node_offset = next_node_offset = -1;
    }
  };

  const std::string PATH_;
  const int LIMIT =  (degree + 1) / 2;
  std::fstream file_;
  FileHeader * file_header_;
  NodeHeader * node_header_root_;
  Compare compare_;
  Compare_ comp_;

  long long getEndPos();

  void InsertPair(const Key & new_key,const T & min_value, Kp  * keys_,const int & index,const int & size);


  /**
   * 注意这里移除后不会进行size--的操作，因为后面还会进行value或者child的操作，要自己进行
   * @param keys_
   * @param index
   * @param size
   */
  void RemovePair(Kp * keys_,const int & index, const int & size);


  void RemoveChild(long long * children,const int & index,const int & size);
  /**
   *
   * @param pos
   * @param children
   * @param index
   * @param size 在进行内部节点split时，会出现size < index的情况，这时候要直接进行插入
   */
  void InsertChild(long long pos, long long * children,const int & index,const int & size);
  /**
   * 返回大于当前的值的位置 用于internal的内部查找
   * @param key 查找值
   * @param size key数组大小
   * @param key_values 查找的数组
   * @return 返回索引值 如果大于最大值 返回值为size
   */
  int Upper_Bound(const Key & key,const T& value,const Kp * key_values, const int size) const;

  /**
   * 返回大于等于当前值的位置
   * @param key 查找值
   * @param size key_values 数组大小
   * @param key_values 查找的数组
   * @param find 是否找到相等的值
   * @return 返回索引值，大于最大值 返回 size
   */
  int Lower_Bound(const Key & key,const T & value, const Kp * key_values, const int size,bool & find) const;

  int Upper_Bound_Key(const Key & key,const Kp * key_values, const int size) const;

  int Lower_Bound_Key(const Key & key, const Kp * key_values, const int size,bool & find) const;
  /**
   * 用于Remove中寻找对应value的位置并修改传入指针的内容，确保其指向真正删除的位置 同时在进行删除的时候，可以确保index返回最适合进行插入的位置
   * 如果在中间，就直接进行插入，如果在文件头部，会选择前面文件的尾部 若前面没有文件，就在当前文件的尾部
   * @param value 要删除的值
   * @param key 用于寻找上一个块
   * @param leaf_node 叶指针
   * @param find 是否找到对应位置
   * @return 返回对应的index
   */
  int GetIndexOfValue(const Key & key,const T & value,LeafNode * leaf_node,bool & find);

  /**
   * 找到该节点在父节点中的位置
   * @param offset
   * @param internal_node
   * @param key
   * @return
   */
  int GetIndexOfOffset(const Key & key,const T & value,long long & offset,InternalNode * internal_node);

  void ReadNodeHeader(NodeHeader *&node_header, long long pos);

  void ReadFileHeader(FileHeader *&file_header);
   /**
   * @param file 
   * @param internal_node 
   * @param pos 
   */
  void ReadInternalNode(InternalNode * & internal_node,long long pos) ;

  /**
   * 读入叶子节点
   * @param file 文件流
   * @param leaf_node 要存入的叶节点内存
   * @param pos 读取的位置
   */
  void ReadLeafNode(LeafNode * & leaf_node,long long pos);

  /**
   * 写入文件头
   * @param file 文件流
   * @param file_header 文件头指针
   * @return 返回插入指针位置
   */
  long long WriteFileHeader(FileHeader * & file_header);

  /**
   *
   * @param file
   * @param node_header
   * @param pos
   * @return
   */
  long long WriteNodeHeader(NodeHeader * & node_header,long long pos);
  /**
   * 在每次写入前，请确认是否同步了 header指针和internal 里面的值，虽然本质上他们是一个东西
   * @param file
   * @param internal_node
   * @param pos 若是小于0 为末尾添加模式
   * @return 返回写入指针位置
   */
  long long  WriteInternalNode(InternalNode * & internal_node,long long pos) ;

  /**
   * 在每次写入前，请确认是否同步了 header指针和internal 里面的值，虽然本质上他们是一个东西
   * @param file
   * @param leaf_node
   * @param pos
   * @return 返回写入指针位置
   */
  long long WriteLeafNode(LeafNode * & leaf_node,long long pos);

  //split the leaf node
  void Split(LeafNode * & leaf_node);

  //split the internal node
  void Split(InternalNode * internal_node);

  void ChangeFather(long long * children, int size_,long long father_offset_);
  /**
   * 检测是否merge
   * @param cur_node_header
   * @return true 进行 false 不进行
   */
  bool CheckMerge(NodeHeader * cur_node_header);
  /**
   * 用于进行remove后的叶节点合并
   * @param file 传入文件头 是否有必要？？
   * @param leaf_node 叶节点
   */
  void Merge(LeafNode * & leaf_node);

  /**
   * 用于进行内部节点的合并,注意这里面我们对于传入节点的内存不能进行删除，不然会造成double free
   * @param file
   * @param internal_node
   */
  void Merge(InternalNode * internal_node);


public:

  BPlusTree()=delete;

  BPlusTree(const std::string& path);

  ~BPlusTree();

  bool Insert(const Key & key, const T & value);

  bool Remove(const Key & key,const T & value);

  bool Update(const Key & key,const T & value);

  std::vector<T> Search(const Key & key,bool & find);
};

#include "BPlusTree.tcc"

#endif //BPLUSTREE_HPP
