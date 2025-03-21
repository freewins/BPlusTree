//
// Created by Freewings on 2025/3/20.
//
#include "BPlusTree.hpp"
#ifdef DEUBG
#include <assert>
#endif
template<class T, class Key, class Compare, int degree>
long long BPlusTree<T, Key, Compare, degree>::getEndPos() {
  file_.seekg(0, std::ios::end);
  return file_.tellg();
}

template<class T, class Key, class Compare, int degree>
int BPlusTree<T, Key, Compare, degree>::Upper_Bound(const Key &key, const Key *key_values, const int size) const {
  int left = 0, right = size - 1;
  int mid = 0;
  while (left <= right) {
    mid = (left + right) / 2;
    if (compare_(key, key_values[mid])) {
      right = mid - 1;
    } else {
      left = mid + 1;
    }
  }
  return left;
}

template<class T, class Key, class Compare, int degree>
int BPlusTree<T, Key, Compare, degree>::Lower_Bound(const Key &key, const Key *key_values, const int size) const {
  int left = 0, right = size - 1;
  int mid = 0;
  while (left <= right) {
    mid = (left + right) / 2;
    if (compare_(key_values[mid], key)) {
      left = mid + 1;
    } else if (compare_(key, key_values[mid])) {
      right = mid - 1;
    } else {
      if (mid == 0)
        return -1;
      return -1 * mid;
    }
  }
  return left;
}

template<class T, class Key, class Compare, int degree>
void BPlusTree<T, Key, Compare, degree>::InsertKey(const Key &new_key, Key *keys_, const int &index, const int &size) {
  if (size >= index) {
    Key *cur_key = new Key[degree];
    memcpy(cur_key, keys_ + index, sizeof(Key) * (size - index));
    keys_[index] = new_key;
    memcpy(keys_ + index + 1, cur_key, sizeof(T) * (size - index));
    delete cur_key;
  }
}

template<class T, class Key, class Compare, int degree>
void BPlusTree<T, Key, Compare,
  degree>::InsertValue(const T &new_value, T *values_, const int &index, const int &size) {
  if (size >= index) {
    T *cur_value = new T[degree];
    memcpy(cur_value, values_ + index, sizeof(Key) * (size - index));
    values_[index] = new_value;
    memcpy(values_ + index + 1, cur_value, sizeof(T) * (size - index));
    delete cur_value;
  }
}

template<class T, class Key, class Compare, int degree>
void BPlusTree<T, Key, Compare, degree>::InsertChild(long long pos, long long *children, const int &index,
                                                     const int &size) {
  if (size >= index) {
    long long tmp_array[size];
    memcpy(tmp_array, children + index, sizeof(long long) * (size - index));
    children[index] = pos;
    memcpy(children + index + 1, tmp_array, sizeof(long long) * (size - index));
  } else {
    children[index] = pos;
  }
}
template<class T, class Key, class Compare, int degree>
void BPlusTree<T, Key, Compare, degree>::
ReadFileHeader(std::fstream &file, FileHeader *&file_header) {
  if (file.fail()) {
    return;
  }
  file.seekg(0, std::ios::beg);
  file.read(reinterpret_cast<char *>(file_header), sizeof(FileHeader));
}

template<class T, class Key, class Compare, int degree>
void BPlusTree<T, Key, Compare, degree>::
ReadNodeHeader(std::fstream &file, NodeHeader *&node_header, long long pos) {
  if (file.fail()) {
    return;
  }
  file.seekg(pos, std::ios::beg);
  file.read(reinterpret_cast<char *>(node_header), sizeof(NodeHeader));
}

template<class T, class Key, class Compare, int degree>
void BPlusTree<T, Key, Compare, degree>::ReadLeafNode(std::fstream &file, LeafNode *&leaf_node, long long pos) {
  if (file.fail()) {
    return;
  }
  file.seekg(pos, std::ios::beg);
  file.read(reinterpret_cast<char *>(leaf_node), sizeof(LeafNode));
}

template<class T, class Key, class Compare, int degree>
void BPlusTree<T, Key, Compare, degree>::ReadInternalNode(std::fstream &file, InternalNode *&internal_node,long long pos) {
  if (file.fail()) {
    return;
  }
  file.seekg(pos, std::ios::beg);
  file.read(reinterpret_cast<char *>(internal_node), sizeof(InternalNode));
}

template<class T, class Key, class Compare, int degree>
long long BPlusTree<T, Key, Compare, degree>::WriteInternalNode(std::fstream &file, InternalNode *&internal_node,long long pos) {
  file.seekp(pos, std::ios::beg);
  file.write(reinterpret_cast<char *>(&internal_node), sizeof(InternalNode));
  return pos;
}

template<class T, class Key, class Compare, int degree>
BPlusTree<T, Key, Compare, degree>::BPlusTree(const std::string &path):PATH_(path) {
  file_.open(PATH_, std::ios::binary | std::ios::in | std::ios::out);
  if (!file_.is_open()) {
    file_.open(PATH_, std::ios::binary | std::ios::out);
    file_.close();
    file_.open(PATH_, std::ios::binary | std::ios::in | std::ios::out);
    this->file_header_ = new FileHeader();
    this->node_header_root_ = new NodeHeader();
    this->node_header_root_->is_leaf = true;
    this->node_header_root_->offset = file_header_->root_offset;
    LeafNode * new_leaf_node = new LeafNode();
    new_leaf_node->header = *(this->node_header_root_);
    WriteLeafNode(file_,new_leaf_node,node_header_root_->offset);
  }
  else {
    this->ReadFileHeader(file_, file_header_);
    this->ReadNodeHeader(file_, node_header_root_, file_header_->root_offset);
  }

}

template<class T, class Key, class Compare, int degree>
bool BPlusTree<T, Key, Compare, degree>::Insert(const Key &key, const T &value) {
  //Solution 1 : 不需要把root 中内容存到内存中，直接从pos 开始循环 好处：可以进行良好的结构设计  坏处: 没法做成缓存，速度变慢
  //采用solution 1
  //Solution 2 : 把root 放在内存中，可以减少一定的硬盘 io 但是结构设计比较丑陋
  bool notDouble = true;
  NodeHeader *cur = this->node_header_root_;
  InternalNode *cur_internal_node = new InternalNode();
  while (!cur->is_leaf) {
    this->ReadInternalNode(file_, cur_internal_node, cur->offset);
    //找到大于它的最大值
    int index = Upper_Bound(key, cur_internal_node->keys_, cur->count_nodes);
    // |0| |1| |2| |3| |4|
    // |   |   |   |   |   \
    // |0| |1| |2| |3| |4|  |5|
    this->ReadNodeHeader(file_, cur, cur_internal_node->children_offset[index]);
  }
  delete cur_internal_node;
  //到达叶节点
  LeafNode *cur_leaf_node = new LeafNode();
  this->ReadLeafNode(file_, cur_leaf_node, cur->offset);
  int index = Lower_Bound(key, cur_leaf_node->keys_, cur->count_nodes);
  if (index < 0) {
    notDouble =  false;
  } else {
    InsertKey(key, cur_leaf_node->keys, index, cur->count_nodes);
    InsertValue(value, cur_leaf_node->values_, index, cur->count_nodes);
    ++cur->count_nodes;
    cur_leaf_node->header.count_nodes = cur->count_nodes;
    ++this->file_header_->count_nodes; //TODO is necessary???
    //TODO Split
    Split(file_, cur_leaf_node);
    WriteLeafNode(file_,cur_leaf_node,cur->offset);
  }
  this->ReadNodeHeader(file_, cur, this->file_header_->root_offset);
  delete cur_leaf_node;
  return notDouble;
}

template<class T, class Key, class Compare, int degree>
void BPlusTree<T, Key, Compare, degree>::Split(std::fstream &file, LeafNode *&leaf_node) {
  if (leaf_node->header.count_nodes < degree) {
    return;
  }
  int change_pos = (leaf_node->header.count_nodes / 2);
  int index = 0;
  //父节点
  InternalNode *cur_internal_node = new InternalNode();
  //父节点不为空
  // |1|*|4| |7|      |1| |3| |4| |7|
  // |   |   |  \ ->  |   |   |   |  \
  // |1| |2| |3| |4|  |1| |5| |2| |3| |4|

  if (leaf_node->header.father_offset != -1) {
    ReadInternalNode(file_, cur_internal_node, leaf_node->header.father_offset);
    index = Upper_Bound(leaf_node->keys_[change_pos], cur_internal_node->keys_, cur_internal_node->count_nodes);
    InsertKey(leaf_node->keys_[change_pos], cur_internal_node->keys_, index, cur_internal_node->count_nodes);
  } else {
    //根数据,创建新的内部节点
    cur_internal_node->header.is_leaf = false;
    cur_internal_node->header.count_nodes = 0;
    cur_internal_node->header.father_offset = -1;
    cur_internal_node->header.offset = getEndPos();
    cur_internal_node->keys_[0] = leaf_node->keys_[change_pos];
    cur_internal_node->children_offset[0] = leaf_node ->header.offset;
    index = 0;
    WriteInternalNode(leaf_node, cur_internal_node, cur_internal_node->header.offset);
    this->file_header_->root_offset = cur_internal_node->header.offset;
  }
  LeafNode *new_leaf_node = new LeafNode();
  //初始化新叶子节点
  new_leaf_node->header.is_leaf = true;
  new_leaf_node->header.offset = getEndPos();
  new_leaf_node->header.father_offset = cur_internal_node->header.offset;
  new_leaf_node->header.count_nodes = leaf_node->header.count_nodes - change_pos;
  memcpy(new_leaf_node->keys_,leaf_node->keys_ + change_pos,sizeof(Key)*(leaf_node->header.count_nodes - change_pos));
  memcpy(new_leaf_node->values_,leaf_node->values_ + change_pos,sizeof(T)*(leaf_node->header.count_nodes - change_pos));
  new_leaf_node->next_node_offset = leaf_node->next_node_offset;
  new_leaf_node->pre_node_offset = leaf_node->header.offset;
  if (new_leaf_node->next_node_offset != -1) {
    LeafNode * next_leaf_node = new LeafNode();
    ReadLeafNode(file_,next_leaf_node,new_leaf_node->next_node_offset);
    next_leaf_node->pre_node_offset = new_leaf_node->header.offset;
    WriteLeafNode(file_,next_leaf_node,next_leaf_node->header.offset);
  }
  leaf_node->next_node_offset = new_leaf_node->header.offset;
  //执行插入
  InsertChild(new_leaf_node->header.offset, cur_internal_node->children_offset, index + 1,
                cur_internal_node->header.count_nodes + 1);
  ++cur_internal_node->header.count_nodes;
  //
  delete new_leaf_node;
  Split(file_,cur_internal_node);
  WriteInternalNode(file_,cur_internal_node,cur_internal_node->header.offset);
  delete cur_internal_node;
}

template<class T, class Key, class Compare, int degree>
void BPlusTree<T, Key, Compare, degree>::Split(std::fstream &file, InternalNode *internal_node) {
  if (internal_node->header.count_nodes < degree) {
    return;
  }
  InternalNode *father_internal_node = new InternalNode();
  int index = 0;
  int change_pos = (internal_node->header.count_nodes / 2);
  if (internal_node->header.father_offset != -1) {
    ReadInternalNode(file_, father_internal_node, internal_node->header.father_offset);
    index = Upper_Bound(internal_node->keys_[change_pos], father_internal_node->keys_,
                        father_internal_node->count_nodes);
    InsertKey(internal_node->keys_[change_pos], father_internal_node->keys_, index,
              father_internal_node->header.count_nodes);
  } else {
    father_internal_node->header.offset = getEndPos();
    father_internal_node->header.count_nodes = 0;
    father_internal_node->header.is_leaf = false;
    father_internal_node->header.father_offset = -1;
    father_internal_node->children_offset[0] = internal_node->header.offset;
    this -> file_header_ ->root_offset = father_internal_node->header.offset;
    father_internal_node->keys_[0] = internal_node->keys_[change_pos];
    father_internal_node->children_offset[0] = internal_node->header.offset;
    index = 0;
    //多进行一次写入，防止后面插入新节点的时候会出问题
    WriteInternalNode(file_, father_internal_node, index);

  }
  //创建新节点，并更新key和child
  InternalNode *new_internal_node = new InternalNode();
  //更新key
  memcpy(new_internal_node->keys_, internal_node->keys_ + change_pos + 1,
         sizeof(Key) * (internal_node->header.count_nodes - change_pos - 1));
  //更新child
  memcpy(new_internal_node->children_offset, internal_node->children_offset + change_pos + 1,
         sizeof(long long) * (internal_node->header.count_nodes - change_pos));
  new_internal_node->header.count_nodes = internal_node->header.count_nodes - change_pos - 1;
  new_internal_node->header.father_offset = father_internal_node->header.offset;
  new_internal_node->header.is_leaf = false;
  new_internal_node->header.offset = getEndPos();
  WriteInternalNode(file_, new_internal_node, new_internal_node->header.offset);
  internal_node->header.count_nodes = change_pos;
  WriteInternalNode(file_, father_internal_node, internal_node->header.offset);
  //插入内部节点的child要在index+1的位置插入
  InsertChild(new_internal_node->header.offset, father_internal_node->children_offset, index + 1,
              father_internal_node->header.count_nodes + 1);
  ++father_internal_node->header.count_nodes;
  WriteInternalNode(file_, father_internal_node, father_internal_node->header.offset);
  delete new_internal_node;
  Split(file_,father_internal_node);
  delete father_internal_node;
  return;
}

template<class T, class Key, class Compare, int degree>
void BPlusTree<T, Key, Compare, degree>::RemoveKey(Key *keys_, const int &index, const int &size) {
  if (index < 0 || index >= size) {
#ifdef DEBUG
    throw std::invalid_argument("Invalid index");
#endif
    return;
  }
  if (index == size - 1) { //当删除最后一个元素的时候，直接退出
    return;
  }
  Key * tmp_keys_ = new Key[size];
  memcpy(tmp_keys_,keys_ + index + 1,sizeof(Key) * (size - index - 1));
  memcpy(keys_ + index ,tmp_keys_ ,sizeof(Key) * (size - index - 1));
}

template<class T, class Key, class Compare, int degree>
void BPlusTree<T, Key, Compare, degree>::RemoveChild(long long *children, const int &index, const int &size) {
  if (index < 0 || index >= size) {
#ifdef DEBUG
    throw std::invalid_argument("Invalid index");
#endif
    return;
  }
  if (index == size - 1) {
    return;
  }
  long long tmp_children[size];
  memcpy(tmp_children,children + index + 1, sizeof(long long) * (size -index - 1));
  memcpy(children + index ,tmp_children ,sizeof(long long) * (size -index - 1));
}

template<class T, class Key, class Compare, int degree>
void BPlusTree<T, Key, Compare, degree>::RemoveValue(T *values_, const int &index, const int &size) {
  if (index < 0 || index >= size)
  {
      #ifdef DEBUG
      throw std::invalid_argument("Invalid index");
      #endif
    return;
  }
  if (index == size - 1) {
    return;
  }
  T * tmp_values = new T[size];
  memcpy(tmp_values,values_ + index + 1,sizeof(T) * (size - index - 1));
  memcpy(values_+ index,tmp_values,sizeof(T) * (size - index - 1));
}


template<class T, class Key, class Compare, int degree>
bool BPlusTree<T, Key, Compare, degree>::Remove(const Key &key) {
  NodeHeader *cur = this->node_header_root_;
  InternalNode *cur_internal_node = new InternalNode();
  while (!cur->is_leaf) {
    this->ReadInternalNode(file_, cur_internal_node, cur->offset);
    //找到大于它的最大值
    int index = Upper_Bound(key, cur_internal_node->keys_, cur->count_nodes);
    // |0| |1| |2| |3| |4|
    // |   |   |   |   |   \
    // |0| |1| |2| |3| |4|  |5|
    this->ReadNodeHeader(file_, cur, cur_internal_node->children_offset[index]);
  }
  delete cur_internal_node;
  LeafNode *cur_leaf_node = new LeafNode();
  ReadLeafNode(file_,cur_leaf_node,cur->offset);
  int index = Lower_Bound(key,cur_leaf_node->keys_,cur_leaf_node->count_nodes);
  if (index < 0) {
    index = -index;
    RemoveKey(cur_leaf_node->keys_,index,cur_leaf_node->count_nodes);
    RemoveValue(cur_leaf_node->values_,index,cur_leaf_node->count_nodes);

  }
  else {
    ReadNodeHeader(file_,this->node_header_root_,this->file_header_->root_offset);
    delete cur_leaf_node;
    return false;
  }
}

template<class T, class Key, class Compare, int degree>
long long BPlusTree<T, Key, Compare, degree>::WriteFileHeader(std::fstream &file, FileHeader *&file_header) {
  file_.seekp(0,std::ios::beg);
  file.write(reinterpret_cast<char *>(file_header_),sizeof(FileHeader));
  return file.tellp();
}

template<class T, class Key, class Compare, int degree>
long long BPlusTree<T, Key, Compare, degree>::WriteLeafNode(std::fstream &file, LeafNode *&leaf_node, long long pos) {
  file.seekp(pos,std::ios::beg);
  file.write(reinterpret_cast<char *>(leaf_node),sizeof(LeafNode));
  return file.tellp();
}

template<class T, class Key, class Compare, int degree>
void BPlusTree<T, Key, Compare, degree>::Merge(std::fstream &file, InternalNode *internal_node) {

}

template<class T, class Key, class Compare, int degree>
void BPlusTree<T, Key, Compare, degree>::Merge(std::fstream &file, LeafNode *&leaf_node) {
  if (CheckMerge(&(leaf_node ->header))) {
    bool left_,right_;
    left_ = right_ = false;
    NodeHeader *left_cur = new NodeHeader();
    NodeHeader *right_cur = new NodeHeader();
    if (leaf_node->pre_node_offset!= -1) {
      ReadNodeHeader(file_,left_cur,leaf_node->pre_node_offset);
      left_ = true;
    }
    if (leaf_node->next_node_offset!= -1) {
      ReadNodeHeader(file_,right_cur,leaf_node->next_node_offset);
      right_ = true;
    }
    if (left_ &&left_cur->count_nodes > LIMIT) { //优先左
      LeafNode * left_node = new LeafNode(); //左兄弟的叶节点
      ReadLeafNode(file_,left_node,left_cur->offset);
      //左节点最大插入到内部最小
      InsertKey(left_node->keys_[left_node->header.count_nodes - 1],leaf_node->keys_,0,leaf_node->count_nodes);
      InsertValue(left_node->values_[left_node->header.count_nodes - 1],leaf_node -> values_,0,leaf_node->header.count_nodes);
      //更新父节点 如果根为叶不会进行merge
      InternalNode *internal_node = new InternalNode();
      ReadInternalNode(file_,internal_node,leaf_node -> header.father_offset);
      int index = Lower_Bound(leaf_node->keys_[leaf_node->header.count_nodes - 1],internal_node->keys_,internal_node->header.count_nodes);
      if (index >= 0) {
        internal_node->keys_[index] = leaf_node->keys_[leaf_node->header.count_nodes - 1];
      }
      WriteInternalNode(file_,internal_node,internal_node->offset);
      delete internal_node;
      RemoveKey(left_node->keys_, left_node->header.count_nodes - 1,left_node->header.count_nodes);
      RemoveValue(left_node->values_,left_cur->header.count_nodes - 1,left_node->header.count_nodes);
      --left_node->header.count_nodes;
      ++leaf_node->header.count_nodes;
      WriteLeafNode(file_,left_node,left_node->header.offset);
      WriteLeafNode(file_,leaf_node,leaf_node->header.offset);
    }
    else if (right_ && right_cur->count_nodes > LIMIT) {
      LeafNode * right_node = new LeafNode(); //左兄弟的叶节点
      ReadLeafNode(file_,right_node,right_cur->offset);
      //右节点最小插入到尾部
      InsertKey(right_node->keys_[0],leaf_node -> keys,leaf_node ->header.count_nodes,leaf_node ->header.count_nodes);
      InsertValue(right_node->values_[0],leaf_node -> values,leaf_node ->header.count_nodes,leaf_node ->header.count_nodes);
      //更新父节点
      InternalNode *internal_node = new InternalNode();
      ReadInternalNode(file_,internal_node,leaf_node->header.father_offset);
      int index = Lower_Bound(leaf_node->keys_[0],internal_node->keys_,internal_node->header.count_nodes);
      if (index >= 0) {
        internal_node->keys_[index] = right_node->keys_[1];
      }
      WriteInternalNode(file_,internal_node,internal_node->offset);
      delete internal_node;
      RemoveKey(right_node->keys_,0,right_node->header.count_nodes);
      RemoveValue(right_node->values_,0,right_node->header.count_nodes);
      --right_node->header.count_nodes;
      ++leaf_node->header.count_nodes;
      WriteLeafNode(file_,right_node,right_node->header.offset);
      WriteLeafNode(file_,internal_node,internal_node->offset);
    }
    else { //进行合并向左
      if (left_) {
        LeafNode * left_node = new LeafNode();
        ReadLeafNode(file_,left_node,left_cur->offset);

      }
      else if (right_){

      }else {

      }
    }
  }
  else {
    return;
  }
}

template<class T, class Key, class Compare, int degree>
bool BPlusTree<T, Key, Compare, degree>::CheckMerge(NodeHeader *cur_node_header) {
  if (cur_node_header -> is_leaf) {
    if (cur_node_header -> father_offset != -1) {
      return cur_node_header ->count_nodes < LIMIT;
    }
    else {
      //根节点为叶节点，删除没有merge的可能
      return false;
    }
  }
  else {
    if (cur_node_header ->father_offset != -1) {
      //孩子数要>=Limit
      return cur_node_header -> count_nodes+ 1 < LIMIT;
    }
    else {
      //根节点 孩子>=2
      return cur_node_header -> count_nodes < 1;
    }
  }
}
