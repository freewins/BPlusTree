//
// Created by Freewings on 2025/3/17.
//

#ifndef BPLUSTREE_HPP
#define BPLUSTREE_HPP
#define DEBUG
#include <cstring>
#include <functional>
#include <fstream>
#include <string>
#include <memory>
//TODO 引入一个标准常数 避免出现溢出
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
    Key keys_[degree]; //键值 为degree - 1
    long long children_offset[degree + 1]; // 孩子的偏移值 ，标记了孩子节点的位置
  };
  /**
   *数据节点，存储了所有的叶节点对应的数据以及键值
   */
  struct LeafNode {
    NodeHeader header; // 节点头
    long long pre_node_offset; //上一个节点的偏移位置
    long long next_node_offset; //下一个节点的偏移位置
    Key keys_[degree]; // 键值
    T values_[degree]; //数据


    LeafNode() {
      header.is_leaf = true;
      pre_node_offset = next_node_offset = -1;
    }
  };

  const std::string PATH_;
  const int LIMIT =  (degree + 1) / 2;;
  std::fstream file_;
  FileHeader * file_header_;
  NodeHeader * node_header_root_;
  Compare compare_;

  long long getEndPos();

  void InsertKey(const Key & new_key, Key  * keys_,const int & index,const int & size);

  void InsertValue(const T & new_value,T * values_,const int & index,const int & size);

  /**
   * 注意这里移除后不会进行size--的操作，因为后面还会进行value或者child的操作，要自己进行
   * @param keys_
   * @param index
   * @param size
   */
  void RemoveKey(Key * keys_,const int & index, const int & size);

  //TODO 考虑将删除封装成 internal 和 leaf 的这样进行size--的操作就比较方便，封闭性好
  void RemoveValue(T * values_,const int & index,const int & size);

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

  /**
   * 读入叶子节点
   * @param file 文件流
   * @param leaf_node 要存入的叶节点内存
   * @param pos 读取的位置
   */
  void ReadLeafNode(std::fstream & file,LeafNode * & leaf_node,long long pos);

  /**
   * 写入文件头
   * @param file 文件流
   * @param file_header 文件头指针
   * @return 返回插入指针位置
   */
  long long WriteFileHeader(std::fstream & file,FileHeader * & file_header) ;

  /**
   * 在每次写入前，请确认是否同步了 header指针和internal 里面的值，虽然本质上他们是一个东西
   * @param file
   * @param internal_node
   * @param pos 若是小于0 为末尾添加模式
   * @return 返回写入指针位置
   */
  long long  WriteInternalNode(std::fstream & file,InternalNode * & internal_node,long long pos) ;

  /**
   * 在每次写入前，请确认是否同步了 header指针和internal 里面的值，虽然本质上他们是一个东西
   * @param file
   * @param leaf_node
   * @param pos
   * @return 返回写入指针位置
   */
  long long WriteLeafNode(std::fstream & file,LeafNode * & leaf_node,long long pos);


  //split the leaf node
  void Split(std::fstream & file,LeafNode * & leaf_node);

  //split the internal node
  void Split(std::fstream & file,InternalNode * internal_node);

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
  void Merge(std::fstream & file,LeafNode * & leaf_node);

  /**
   * 用于进行内部节点的合并
   * @param file
   * @param internal_node
   */
  void Merge(std::fstream & file,InternalNode * internal_node);


public:
  BPlusTree(const std::string& path);

  bool Insert(const Key & key, const T & value);

  bool Remove(const Key & key);
};




#endif //BPLUSTREE_HPP
