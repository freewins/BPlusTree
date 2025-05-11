//
// Created by Freewings on 2025/3/20.
//
#ifndef BPLUSTREE_TCC
#define BPLUSTREE_TCC
#include "BPlusTree.hpp"

template<class T, class Key, int degree, class Compare, class Compare_>
long long BPlusTree<T, Key, degree, Compare, Compare_>::getEndPos() {
  file_.seekg(0, std::ios::end);
  return file_.tellg();
}

template<class T, class Key, int degree, class Compare, class Compare_>
int BPlusTree<T, Key, degree, Compare, Compare_>::Lower_Bound_Key(const Key &key, const Kp *key_values, const int size,
                                                                  bool &find) const {
  if (key_values[size - 1].first < key) {
    find = false;
    return size;
  }
  if (key < key_values[0].first ) {
    find = false;
    return 0;
  }
  int left = 0, right = size - 1;
  int mid = 0;
  find = false;
  while (left <= right) {
    mid = left + right >> 1;
    if (key_values[mid].first < key) {
      left = mid + 1;
    } else if (key < key_values[mid].first) {
      right = mid - 1;
    } else {
      right = mid - 1;
      find = true;
    }
  }
  return left;
}

template<class T, class Key, int degree, class Compare, class Compare_>
int BPlusTree<T, Key, degree, Compare, Compare_>::Upper_Bound_Key(const Key &key, const Kp *key_values,
                                                                  const int size) const {
  if (key_values[size - 1].first < key) {
    return size;
  }
  if ( key < key_values[0].first ) {
    return 0;
  }
  int left = 0, right = size - 1;
  int mid = 0;
  while (left <= right) {
    mid = left + right >> 1;
    if (key < key_values[mid].first) {
      right = mid - 1;
    } else {
      left = mid + 1;
    }
  }
  return left;
}

template<class T, class Key, int degree, class Compare, class Compare_>
int BPlusTree<T, Key, degree, Compare, Compare_>::Upper_Bound(const Key &key, const T &value, const Kp *key_values,
                                                              const int size) const {
  sjtu::pair<Key, T> tmp(key, value);
  int left = 0, right = size - 1;
  int mid = 0;
  while (left <= right) {
    mid = (left + right) >> 1;
    if (tmp < key_values[mid]) {
      right = mid - 1;
    } else {
      left = mid + 1;
    }
  }
  return left;
}

template<class T, class Key, int degree, class Compare, class Compare_>
int BPlusTree<T, Key, degree, Compare, Compare_>::Lower_Bound(const Key &key, const T &value, const Kp *key_values,
                                                              const int size,
                                                              bool &find) const {
  Kp tmp(key, value);
  int left = 0, right = size - 1;
  int mid = 0;
  find = false;
  while (left <= right) {
    mid = (left + right)  >> 1;
    if (key_values[mid] < tmp) {
      left = mid + 1;
    } else if (tmp < key_values[mid]) {
      right = mid - 1;
    } else {
      right = mid - 1;
      find = true;
    }
  }
  return left;
}

template<class T, class Key, int degree, class Compare, class Compare_>
void BPlusTree<T, Key, degree, Compare, Compare_>::InsertPair(const Key &new_key, const T &min_value, Kp *keys_,
                                                              const int &index, const int &size) {
  if (size >= index) {
    Kp *cur_key = new Kp[degree];
    memcpy(cur_key, keys_ + index, sizeof(Kp) * (size - index));
    keys_[index] = Kp(new_key, min_value);
    memcpy(keys_ + index + 1, cur_key, sizeof(Kp) * (size - index));
    delete[] cur_key;
  }
}

template<class T, class Key, int degree, class Compare, class Compare_>
void BPlusTree<T, Key, degree, Compare, Compare_>::RemovePair(Kp *keys_, const int &index, const int &size) {
  if (index < 0 || index >= size) {
#ifdef DEBUG
    throw std::invalid_argument("Invalid index");
#endif
    return;
  }
  if (index == size - 1) {
    return;
  }
  Kp *tmp_kp = new Kp[size];
  memcpy(tmp_kp, keys_ + index + 1, sizeof(Kp) * (size - index - 1));
  memcpy(keys_ + index, tmp_kp, sizeof(Kp) * (size - index - 1));
  delete [] tmp_kp;
}


// template<class T, class Key, int degree, class Compare, class Compare_>
// void BPlusTree<T, Key, degree, Compare, Compare_>::InsertKey(const Key &new_key, Key *keys_, const int &index,
//                                                              const int &size) {
//   if (size >= index) {
//     Key *cur_key = new Key[degree];
//     memcpy(cur_key, keys_ + index, sizeof(Key) * (size - index));
//     keys_[index] = new_key;
//     memcpy(keys_ + index + 1, cur_key, sizeof(Key) * (size - index));
//     delete[] cur_key;
//   }
// }
//

template<class T, class Key, int degree, class Compare, class Compare_>
void BPlusTree<T, Key, degree, Compare, Compare_>::InsertChild(long long pos, long long *children, const int &index,
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

template<class T, class Key, int degree, class Compare, class Compare_>
void BPlusTree<T, Key, degree, Compare, Compare_>::
ReadFileHeader(FileHeader *&file_header) {
  if (file_.fail()) {
    return;
  }
  file_.seekg(0, std::ios::beg);
  file_.read(reinterpret_cast<char *>(file_header), sizeof(FileHeader));
}

template<class T, class Key, int degree, class Compare, class Compare_>
void BPlusTree<T, Key, degree, Compare, Compare_>::
ReadNodeHeader(NodeHeader *&node_header, long long pos) {
  if (file_.fail()) {
    file_.clear();
  }
  file_.seekg(pos, std::ios::beg);
  file_.read(reinterpret_cast<char *>(node_header), sizeof(NodeHeader));
}

template<class T, class Key, int degree, class Compare, class Compare_>
void BPlusTree<T, Key, degree, Compare,
  Compare_>::ReadLeafNode(LeafNode *&leaf_node, long long pos) {
  if (file_.fail()) {
    file_.clear();
  }
  file_.seekg(pos, std::ios::beg);
  file_.read(reinterpret_cast<char *>(leaf_node), sizeof(LeafNode));
}

template<class T, class Key, int degree, class Compare, class Compare_>
void BPlusTree<T, Key, degree, Compare, Compare_>::ReadInternalNode(InternalNode *&internal_node,
                                                                    long long pos) {
  if (file_.fail()) {
    file_.clear();
  }
  file_.seekg(pos, std::ios::beg);
  file_.read(reinterpret_cast<char *>(internal_node), sizeof(InternalNode));
}

template<class T, class Key, int degree, class Compare, class Compare_>
long long BPlusTree<T, Key, degree, Compare, Compare_>::WriteInternalNode(InternalNode *&internal_node,
                                                                          long long pos) {
  if (file_.fail()) {
    file_.clear();
  }
  file_.seekp(pos, std::ios::beg);
  file_.write(reinterpret_cast<char *>(internal_node), sizeof(InternalNode));
  return file_.tellp();
}


template<class T, class Key, int degree, class Compare, class Compare_>
void BPlusTree<T, Key, degree, Compare, Compare_>::Split(LeafNode *&leaf_node) {
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
    ReadInternalNode(cur_internal_node, leaf_node->header.father_offset);
    bool find_ = false;
    index = GetIndexOfOffset(leaf_node->values[change_pos].first, leaf_node->values[change_pos].second,
                             leaf_node->header.offset, cur_internal_node);
    // if (find_) {
    //   ++index;
    // }
    //index = Upper_Bound(leaf_node->keys_[change_pos], cur_internal_node->keys_, cur_internal_node->header.count_nodes);
    InsertPair(leaf_node->values[change_pos].first, leaf_node->values[change_pos].second, cur_internal_node->keys_,
               index, cur_internal_node->header.count_nodes);
  } else {
    //根数据,创建新的内部节点
    cur_internal_node->header.is_leaf = false;
    cur_internal_node->header.count_nodes = 0;
    cur_internal_node->header.father_offset = -1;
    cur_internal_node->header.offset = getEndPos();
    cur_internal_node->keys_[0] = leaf_node->values[change_pos];
    cur_internal_node->children_offset[0] = leaf_node->header.offset;
    index = 0;
    //这一步是必须要有，不然后面的新节点找不到文件末尾
    WriteInternalNode(cur_internal_node, cur_internal_node->header.offset);
    this->file_header_->root_offset = cur_internal_node->header.offset;
    //ReadNodeHeader(this->node_header_root_,this->file_header_->root_offset);
  }
  LeafNode *new_leaf_node = new LeafNode();
  //初始化新叶子节点
  new_leaf_node->header.is_leaf = true;
  new_leaf_node->header.offset = getEndPos();
  //更新父节点
  new_leaf_node->header.father_offset = cur_internal_node->header.offset;
  leaf_node->header.father_offset = cur_internal_node->header.offset;
  //
  new_leaf_node->header.count_nodes = leaf_node->header.count_nodes - change_pos;
  memcpy(new_leaf_node->values, leaf_node->values + change_pos,
         sizeof(Kp) * (leaf_node->header.count_nodes - change_pos));
  new_leaf_node->next_node_offset = leaf_node->next_node_offset;
  new_leaf_node->pre_node_offset = leaf_node->header.offset;
  if (new_leaf_node->next_node_offset != -1) {
    LeafNode *next_leaf_node = new LeafNode();
    ReadLeafNode(next_leaf_node, new_leaf_node->next_node_offset);
    next_leaf_node->pre_node_offset = new_leaf_node->header.offset;
    WriteLeafNode(next_leaf_node, next_leaf_node->header.offset);
    delete next_leaf_node;
  }
  leaf_node->next_node_offset = new_leaf_node->header.offset;
  leaf_node->header.count_nodes = change_pos;
  //执行插入
  InsertChild(new_leaf_node->header.offset, cur_internal_node->children_offset, index + 1,
              cur_internal_node->header.count_nodes + 1);
  ++cur_internal_node->header.count_nodes;
  WriteLeafNode(new_leaf_node, new_leaf_node->header.offset);
  WriteLeafNode(leaf_node, leaf_node->header.offset);
  delete new_leaf_node;
  WriteInternalNode(cur_internal_node, cur_internal_node->header.offset);
  Split(cur_internal_node);
  delete cur_internal_node;
}

template<class T, class Key, int degree, class Compare, class Compare_>
void BPlusTree<T, Key, degree, Compare, Compare_>::Split(InternalNode *internal_node) {
  if (internal_node->header.count_nodes < degree) {
    return;
  }
  InternalNode *father_internal_node = new InternalNode();
  int index = 0;
  int change_pos = (internal_node->header.count_nodes / 2);
  if (internal_node->header.father_offset != -1) {
    ReadInternalNode(father_internal_node, internal_node->header.father_offset);
    index = GetIndexOfOffset(internal_node->keys_[change_pos].first, internal_node->keys_[change_pos].second,
                             internal_node->header.offset, father_internal_node);
    //Upper_Bound(internal_node->keys_[change_pos], father_internal_node->keys_,father_internal_node->header.count_nodes);
    InsertPair(internal_node->keys_[change_pos].first, internal_node->keys_[change_pos].second,
               father_internal_node->keys_, index,
               father_internal_node->header.count_nodes);
  } else {
    father_internal_node->header.offset = getEndPos();
    father_internal_node->header.count_nodes = 0;
    father_internal_node->header.is_leaf = false;
    father_internal_node->header.father_offset = -1;
    this->file_header_->root_offset = father_internal_node->header.offset;
    father_internal_node->keys_[0] = internal_node->keys_[change_pos];
    father_internal_node->children_offset[0] = internal_node->header.offset;
    WriteInternalNode(father_internal_node, father_internal_node->header.offset);
    index = 0;
  }
  //创建新节点，并更新key和child
  InternalNode *new_internal_node = new InternalNode();
  //更新key
  memcpy(new_internal_node->keys_, internal_node->keys_ + change_pos + 1,
         sizeof(Kp) * (internal_node->header.count_nodes - change_pos - 1));
  //更新child
  memcpy(new_internal_node->children_offset, internal_node->children_offset + change_pos + 1,
         sizeof(long long) * (internal_node->header.count_nodes - change_pos));
  //修改新节点
  new_internal_node->header.count_nodes = internal_node->header.count_nodes - change_pos - 1;
  new_internal_node->header.father_offset = father_internal_node->header.offset;
  new_internal_node->header.is_leaf = false;
  new_internal_node->header.offset = getEndPos();
  //更新子节点的父亲
  ChangeFather(new_internal_node->children_offset, new_internal_node->header.count_nodes + 1,
               new_internal_node->header.offset);
  //修改初始节点
  internal_node->header.count_nodes = change_pos;
  internal_node->header.father_offset = father_internal_node->header.offset;
  WriteInternalNode(new_internal_node, new_internal_node->header.offset);
  WriteInternalNode(internal_node, internal_node->header.offset);
  //插入内部节点的child要在index+1的位置插入
  InsertChild(new_internal_node->header.offset, father_internal_node->children_offset, index + 1,
              father_internal_node->header.count_nodes + 1);
  ++father_internal_node->header.count_nodes;
  //写入
  WriteInternalNode(father_internal_node, father_internal_node->header.offset);
  delete new_internal_node;
  Split(father_internal_node);
  delete father_internal_node;
  return;
}


template<class T, class Key, int degree, class Compare, class Compare_>
void BPlusTree<T, Key, degree, Compare, Compare_>::RemoveChild(long long *children, const int &index, const int &size) {
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
  memcpy(tmp_children, children + index + 1, sizeof(long long) * (size - index - 1));
  memcpy(children + index, tmp_children, sizeof(long long) * (size - index - 1));
}

template<class T, class Key, int degree, class Compare, class Compare_>
long long BPlusTree<T, Key, degree, Compare, Compare_>::WriteNodeHeader(NodeHeader *&node_header,
                                                                        long long pos) {
  if (file_.fail()) {
    file_.clear();
  }
  file_.seekg(pos, std::ios::beg);
  file_.write(reinterpret_cast<char *>(node_header), sizeof(NodeHeader));
  return file_.tellp();
}


template<class T, class Key, int degree, class Compare, class Compare_>
long long BPlusTree<T, Key, degree, Compare, Compare_>::WriteFileHeader(FileHeader *&file_header) {
  if (file_.fail()) {
    file_.clear();
  }
  file_.seekp(0, std::ios::beg);
  file_.write(reinterpret_cast<char *>(file_header_), sizeof(FileHeader));
  return file_.tellp();
}

template<class T, class Key, int degree, class Compare, class Compare_>
long long BPlusTree<T, Key, degree, Compare, Compare_>::WriteLeafNode(LeafNode *&leaf_node,
                                                                      long long pos) {
  if (file_.fail()) {
    file_.clear();
  }
  file_.seekp(pos, std::ios::beg);
  file_.write(reinterpret_cast<char *>(leaf_node), sizeof(LeafNode));
  return file_.tellp();
}

template<class T, class Key, int degree, class Compare, class Compare_>
int BPlusTree<T, Key, degree, Compare, Compare_>::GetIndexOfValue(const Key &key, const T &value, LeafNode *leaf_node,
                                                                  bool &find) {
  if (leaf_node == nullptr || leaf_node->header.count_nodes == 0) {
    find = false;
    return 0;
  }
  int pre_index = Lower_Bound(key, value, leaf_node->values, leaf_node->header.count_nodes, find);
  return pre_index;
}

template<class T, class Key, int degree, class Compare, class Compare_>
int BPlusTree<T, Key, degree, Compare, Compare_>::GetIndexOfOffset(const Key &key, const T &value, long long &offset,
                                                                   InternalNode *internal_node) {
  bool find_key = false;
  int next_index = Upper_Bound_Key(key, internal_node->keys_, internal_node->header.count_nodes);
  int pre_index = Lower_Bound_Key(key, internal_node->keys_, internal_node->header.count_nodes, find_key);
  if (find_key) {
    for (int i = pre_index; i <= next_index; i++) {
      if (internal_node->children_offset[i] == offset) {
        return i;
      }
    }
    return next_index;
  } else {
    return next_index;
  }
}


template<class T, class Key, int degree, class Compare, class Compare_>
bool BPlusTree<T, Key, degree, Compare, Compare_>::CheckMerge(NodeHeader *cur_node_header) {
  if (cur_node_header->is_leaf) {
    if (cur_node_header->father_offset != -1) {
      return cur_node_header->count_nodes < LIMIT;
    } else {
      //根节点为叶节点，删除没有merge的可能
      return false;
    }
  } else {
    if (cur_node_header->father_offset != -1) {
      //孩子数要>=Limit
      return cur_node_header->count_nodes + 1 < LIMIT;
    } else {
      //根节点 孩子>=2
      return cur_node_header->count_nodes < 1;
    }
  }
}

template<class T, class Key, int degree, class Compare, class Compare_>
void BPlusTree<T, Key, degree, Compare, Compare_>::ChangeFather(long long *children, int size_,
                                                                long long father_offset_) {
  NodeHeader *cur = new NodeHeader();
  for (int i = 0; i < size_; i++) {
    ReadNodeHeader(cur, children[i]);
    cur->father_offset = father_offset_;
    WriteNodeHeader(cur, children[i]);
  }
  delete cur;
}


template<class T, class Key, int degree, class Compare, class Compare_>
void BPlusTree<T, Key, degree, Compare, Compare_>::Merge(InternalNode *internal_node) {
  if (CheckMerge((&internal_node->header))) {
    if (internal_node->header.father_offset != -1) {
      //外部new就在外部统一delete
      InternalNode *father_node = new InternalNode();
      ReadInternalNode(father_node, internal_node->header.father_offset);
      NodeHeader *left_cur = new NodeHeader();
      NodeHeader *right_cur = new NodeHeader();
      bool right_ = false, left_ = false;
      //找到该节点在父节点中的位置
      int index = GetIndexOfOffset(internal_node->keys_[0].first, internal_node->keys_[0].second,
                                   internal_node->header.offset, father_node);
      //int index = Upper_Bound(internal_node->keys_[0], father_node->keys_, father_node->header.count_nodes);
      if (index > 0) {
        //有左兄弟
        left_ = true;
        ReadNodeHeader(left_cur, father_node->children_offset[index - 1]);
      } else if (index < father_node->header.count_nodes) {
        //有右兄弟
        right_ = true;
        ReadNodeHeader(right_cur, father_node->children_offset[index + 1]);
      } else {
        //只剩自己一个节点
        //RE
      }
      if (left_ && left_cur->count_nodes >= LIMIT) {
        //从左兄借节点
        //1 向自己头部插入左兄弟最后的节点和child
        InternalNode *left_node = new InternalNode();
        ReadInternalNode(left_node, left_cur->offset);
        //插入父亲的key 兄弟的child
        //要-1 因为是自己的upperbound
        InsertPair(father_node->keys_[index - 1].first, father_node->keys_[index - 1].second, internal_node->keys_, 0,
                   internal_node->header.count_nodes);
        InsertChild(left_node->children_offset[left_cur->count_nodes], internal_node->children_offset, 0,
                    internal_node->header.count_nodes + 1);
        //左节点插入的位置的父节点进行修改
        NodeHeader *change_cur = new NodeHeader();
        ReadNodeHeader(change_cur, internal_node->children_offset[0]);
        change_cur->father_offset = internal_node->header.offset;
        WriteNodeHeader(change_cur, change_cur->offset);
        delete change_cur;

        father_node->keys_[index - 1] = left_node->keys_[left_cur->count_nodes - 1];
        --left_node->header.count_nodes;
        ++internal_node->header.count_nodes;
        WriteInternalNode(left_node, left_node->header.offset);
        WriteInternalNode(internal_node, internal_node->header.offset);
        delete left_node;
        WriteInternalNode(father_node, father_node->header.offset);
      } else if (right_ && right_cur->count_nodes >= LIMIT) {
        //从右兄弟借节点
        //1 向自己尾部插入父亲第一个节点和child
        InternalNode *right_node = new InternalNode();
        ReadInternalNode(right_node, right_cur->offset);
        //父亲的key 插入到自己的尾部 这里不用-1
        InsertPair(father_node->keys_[index].first, father_node->keys_[index].second, internal_node->keys_,
                   internal_node->header.count_nodes,
                   internal_node->header.count_nodes);
        InsertChild(right_node->children_offset[0], internal_node->children_offset,
                    internal_node->header.count_nodes + 1, internal_node->header.count_nodes + 1);
        //右节点插入的孩子的父节点进行修改
        NodeHeader *change_cur = new NodeHeader();
        ReadNodeHeader(change_cur, internal_node->children_offset[internal_node->header.count_nodes + 1]);
        change_cur->father_offset = internal_node->header.offset;
        WriteNodeHeader(change_cur, change_cur->offset);
        delete change_cur;

        father_node->keys_[index] = right_node->keys_[0];
        RemoveChild(right_node->children_offset, 0, right_node->header.count_nodes + 1);
        RemovePair(right_node->keys_, 0, right_node->header.count_nodes);
        --right_node->header.count_nodes;
        ++internal_node->header.count_nodes;
        WriteInternalNode(right_node, right_node->header.offset);
        WriteInternalNode(internal_node, internal_node->header.offset);
        delete right_node;
        WriteInternalNode(father_node, father_node->header.offset);
      } else {
        //只能合并
        InternalNode *left_node, *right_node;
        if (left_) {
          //自己是右节点
          right_node = internal_node;
          left_node = new InternalNode();
          --index;
          ReadInternalNode(left_node, father_node->children_offset[index]);
        } else if (right_) {
          //自己是左节点
          left_node = internal_node;
          right_node = new InternalNode();
          ReadInternalNode(right_node, father_node->children_offset[index + 1]);
        } else {
          //re
        }
        //确保index 指向左节点位置
        //1下移父节点
        InsertPair(father_node->keys_[index].first, father_node->keys_[index].second, left_node->keys_,
                   left_node->header.count_nodes,
                   left_node->header.count_nodes);
        //2移动右兄弟 这两处都是+1，因为只复制了key 没复制child_offset
        memcpy(left_node->keys_ + left_node->header.count_nodes + 1, right_node->keys_,
               sizeof(Kp) * right_node->header.count_nodes);
        memcpy(left_node->children_offset + left_node->header.count_nodes + 1, right_node->children_offset,
               sizeof(long long) * (right_node->header.count_nodes + 1));
        ChangeFather(left_node->children_offset + left_node->header.count_nodes + 1, right_node->header.count_nodes + 1,
                     left_node->header.offset);
        left_node->header.count_nodes += (right_node->header.count_nodes + 1);
        RemovePair(father_node->keys_, index, father_node->header.count_nodes);
        RemoveChild(father_node->children_offset, index + 1, father_node->header.count_nodes + 1);
        --father_node->header.count_nodes;
        right_node->header.count_nodes = 0;
        right_node->header.father_offset = -1;
        //写入父节点和左节点右节点
        WriteInternalNode(father_node, father_node->header.offset);
        WriteInternalNode(left_node, left_node->header.offset);
        WriteInternalNode(right_node, right_node->header.offset);
        if (left_) {
          delete left_node;
        } else {
          delete right_node;
        }
        //进行父节点的检测
        Merge(father_node);
      }
      delete father_node;
      delete left_cur;
      delete right_cur;
    } else {
      //根节点如果需要merge则说明要降低树的高度
      //步骤:把根节点指向子节点
      this->file_header_->root_offset = internal_node->children_offset[0];
      NodeHeader *change_cur = new NodeHeader();
      ReadNodeHeader(change_cur, internal_node->children_offset[0]);
      change_cur->father_offset = -1;
      WriteNodeHeader(change_cur, change_cur->offset);
      delete change_cur;
      return;
    }
  } else {
    return;
  }
}

template<class T, class Key, int degree, class Compare, class Compare_>
void BPlusTree<T, Key, degree, Compare, Compare_>::Merge(LeafNode *&leaf_node) {
  if (CheckMerge(&(leaf_node->header))) {
    bool left_, right_;
    left_ = right_ = false;
    NodeHeader *left_cur = new NodeHeader();
    NodeHeader *right_cur = new NodeHeader();
    if (leaf_node->pre_node_offset != -1) {
      ReadNodeHeader(left_cur, leaf_node->pre_node_offset);
      if (left_cur->father_offset == leaf_node->header.father_offset) {
        left_ = true;
      }
    }
    if (leaf_node->next_node_offset != -1) {
      ReadNodeHeader(right_cur, leaf_node->next_node_offset);
      if (right_cur->father_offset == leaf_node->header.father_offset) {
        right_ = true;
      }
    }
    if (left_ && left_cur->count_nodes >= LIMIT) {
      //优先左
      LeafNode *left_node = new LeafNode(); //左兄弟的叶节点
      ReadLeafNode(left_node, left_cur->offset);
      //左节点最大插入到内部最小
      InsertPair(left_node->values[left_node->header.count_nodes - 1].first,
                 left_node->values[left_node->header.count_nodes - 1].second, leaf_node->values, 0,
                 leaf_node->header.count_nodes);
      //更新父节点 如果根为叶不会进行merge
      InternalNode *internal_node = new InternalNode();
      ReadInternalNode(internal_node, leaf_node->header.father_offset);
      //必须找左节点的位置
      //TODO 先设为0 因为肯定寻找的是大于的位置
      int index = GetIndexOfOffset(left_node->values[0].first, left_node->values[0].second, left_node->header.offset,
                                   internal_node);
      //int index = Upper_Bound(left_node->keys_[left_node->header.count_nodes - 1], internal_node->keys_,internal_node->header.count_nodes);

      internal_node->keys_[index] = left_node->values[left_node->header.count_nodes - 1];

      WriteInternalNode(internal_node, internal_node->header.offset);
      delete internal_node;
      RemovePair(left_node->values, left_node->header.count_nodes - 1, left_node->header.count_nodes);
      --left_node->header.count_nodes;
      ++leaf_node->header.count_nodes;
      WriteLeafNode(left_node, left_node->header.offset);
      WriteLeafNode(leaf_node, leaf_node->header.offset);
      delete left_node;
    } else if (right_ && right_cur->count_nodes >= LIMIT) {
      LeafNode *right_node = new LeafNode(); //右兄弟的叶节点
      ReadLeafNode(right_node, right_cur->offset);
      //右节点最小插入到尾部
      InsertPair(right_node->values[0].first, right_node->values[0].second, leaf_node->values,
                 leaf_node->header.count_nodes, leaf_node->header.count_nodes);
      //更新父节点
      InternalNode *internal_node = new InternalNode();
      ReadInternalNode(internal_node, leaf_node->header.father_offset);
      //用右节点来找，避免左节点为空的时候找到的位置出现错误
      int index = GetIndexOfOffset(leaf_node->values[0].first, leaf_node->values[0].second, leaf_node->header.offset,
                                   internal_node);
      //int index = Upper_Bound(right_node->keys_[0], internal_node->keys_, internal_node->header.count_nodes);
      internal_node->keys_[index] = right_node->values[1];

      WriteInternalNode(internal_node, internal_node->header.offset);
      delete internal_node;
      RemovePair(right_node->values, 0, right_node->header.count_nodes);
      --right_node->header.count_nodes;
      ++leaf_node->header.count_nodes;
      WriteLeafNode(right_node, right_node->header.offset);
      WriteLeafNode(leaf_node, leaf_node->header.offset);
      delete right_node;
    } else {
      //进行合并向左
      LeafNode *left_node, *right_node;
      if (left_) {
        left_node = new LeafNode();
        right_node = leaf_node;
        ReadLeafNode(left_node, left_cur->offset);
      } else if (right_) {
        left_node = leaf_node;
        right_node = new LeafNode();
        ReadLeafNode(right_node, right_cur->offset);
      } else {
        //RE
        throw std::invalid_argument("Invalid node type");
      }
      memcpy(left_node->values + left_node->header.count_nodes, right_node->values,
             sizeof(Kp) * right_node->header.count_nodes);
      left_node->header.count_nodes += right_node->header.count_nodes;
      left_node->next_node_offset = right_node->next_node_offset;
      //右节点失效
      right_node->next_node_offset = right_node->pre_node_offset = -1;
      right_node->header.count_nodes = 0;
      InternalNode *internal_node = new InternalNode();
      ReadInternalNode(internal_node, right_node->header.father_offset);
      //find 是用来寻找下界，以为了确保找到正确的位置
      int index = GetIndexOfOffset(right_node->values[0].first, right_node->values[0].second, right_node->header.offset,
                                   internal_node);
      //
      RemovePair(internal_node->keys_, index - 1, internal_node->header.count_nodes);
      RemoveChild(internal_node->children_offset, index, internal_node->header.count_nodes + 1);
      --internal_node->header.count_nodes;
      WriteInternalNode(internal_node, internal_node->header.offset);
      if (left_node->next_node_offset != -1) {
        LeafNode *next_node = new LeafNode();
        ReadLeafNode(next_node, left_node->next_node_offset);
        next_node->pre_node_offset = left_node->header.offset;
        WriteLeafNode(next_node, next_node->header.offset);
        delete next_node;
      }
      WriteLeafNode(left_node, left_node->header.offset);
      WriteLeafNode(right_node, right_node->header.offset);
      if (left_) {
        delete left_node;
      } else {
        delete right_node;
      }
      //TODO 执行对父节点的合并
      Merge(internal_node);
      delete internal_node;
    }
    delete left_cur;
    delete right_cur;
  } else {
    return;
  }
}


template<class T, class Key, int degree, class Compare, class Compare_>
BPlusTree<T, Key, degree, Compare, Compare_>::BPlusTree(const std::string &path): PATH_(path) {
  file_.open(PATH_, std::ios::binary | std::ios::in | std::ios::out);
  this->file_header_ = new FileHeader();
  this->node_header_root_ = new NodeHeader();
  if (!file_.is_open()) {
    file_.open(PATH_, std::ios::binary | std::ios::out);
    file_.close();
    file_.open(PATH_, std::ios::binary | std::ios::in | std::ios::out);
    this->node_header_root_->is_leaf = true;
    this->node_header_root_->offset = file_header_->root_offset;
    this->file_header_->root_offset = this->node_header_root_->offset;
    LeafNode *new_leaf_node = new LeafNode();
    new_leaf_node->header = *(this->node_header_root_);
    WriteLeafNode(new_leaf_node, node_header_root_->offset);
    delete new_leaf_node;
  } else {
    this->ReadFileHeader(file_header_);
    this->ReadNodeHeader(node_header_root_, file_header_->root_offset);
  }
}

template<class T, class Key, int degree, class Compare, class Compare_>
BPlusTree<T, Key, degree, Compare, Compare_>::~BPlusTree() {
  WriteFileHeader(file_header_);
  delete file_header_;
  delete node_header_root_;
  file_.close();
}

template<class T, class Key, int degree, class Compare, class Compare_>
bool BPlusTree<T, Key, degree, Compare, Compare_>::Remove(const Key &key, const T &value) {
  NodeHeader *cur = new NodeHeader(*(this->node_header_root_));
  InternalNode *cur_internal_node = new InternalNode();
  this->ReadInternalNode(cur_internal_node, cur->offset);//先读入一个internal node
  while (!cur_internal_node->header.is_leaf) {
    //找到大于它的最大值
    int index = Upper_Bound(key, value, cur_internal_node->keys_, cur_internal_node->header.count_nodes);
    // |0| |1| |2| |3| |4|
    // |   |   |   |   |   \
    // |0| |1| |2| |3| |4|  |5|
    // this->ReadNodeHeader(cur, cur_internal_node->children_offset[index]);
    this->ReadInternalNode(cur_internal_node, cur_internal_node->children_offset[index]);
  }
  LeafNode *cur_leaf_node = new LeafNode();
  ReadLeafNode(cur_leaf_node, cur_internal_node->header.offset);
  delete cur_internal_node;

  bool found = false;
  int index = GetIndexOfValue(key, value, cur_leaf_node, found);
  //int index = Lower_Bound(key, cur_leaf_node->keys_, cur_leaf_node->header.count_nodes,found);
  if (found) {
    RemovePair(cur_leaf_node->values, index, cur_leaf_node->header.count_nodes);
    --cur_leaf_node->header.count_nodes;
    WriteLeafNode(cur_leaf_node, cur_leaf_node->header.offset);
    Merge(cur_leaf_node);
    --this->file_header_->node_count;
  }
  if (this->node_header_root_->offset != this->file_header_->root_offset) {
    ReadNodeHeader(this->node_header_root_, this->file_header_->root_offset);
  }
  delete cur;
  delete cur_leaf_node;
  return found;
}

template<class T, class Key, int degree, class Compare, class Compare_>
bool BPlusTree<T, Key, degree, Compare, Compare_>::Insert(const Key &key, const T &value) {
  //Solution 1 : 不需要把root 中内容存到内存中，直接从pos 开始循环 好处：可以进行良好的结构设计  坏处: 没法做成缓存，速度变慢
  //采用solution 1
  //Solution 2 : 把root 放在内存中，可以减少一定的硬盘 io 但是结构设计比较丑陋
  bool notDouble = true;
  NodeHeader *cur = new NodeHeader(*(this->node_header_root_));
  InternalNode *cur_internal_node = new InternalNode();
  this->ReadInternalNode(cur_internal_node, cur->offset);
  while (!cur_internal_node->header.is_leaf) {
    //找到大于它的最大值
    int index = Upper_Bound(key, value, cur_internal_node->keys_, cur_internal_node->header.count_nodes);
    // |0| |1| |2| |3| |4|
    // |   |   |   |   |   \
    // |0| |1| |2| |3| |4|  |5|
    this->ReadInternalNode(cur_internal_node, cur_internal_node->children_offset[index]);
  }
  LeafNode *cur_leaf_node = new LeafNode();
  this->ReadLeafNode(cur_leaf_node, cur_internal_node->header.offset);
  delete cur_internal_node;
  //到达叶节点

  bool found = false;
  int index = GetIndexOfValue(key, value, cur_leaf_node, found);
  if (!found) {
    InsertPair(key, value, cur_leaf_node->values, index, cur_leaf_node->header.count_nodes);
    ++cur_leaf_node->header.count_nodes;
    ++this->file_header_->node_count;
    //一个重要的Bug 修正
    //最开始我是在Split后才进行写入，但是这会导致一个问题，Split中在修正父节点的时候，直接对外存进行操作，修改其父节点
    //但是内存中的父节点并没有被修改，所以Split后写入会导致父节点修改失效，存储失败
    //这句话不要删，虽然Split里面有写入函数，但是进行Split 的概率很低
    WriteLeafNode(cur_leaf_node, cur_leaf_node->header.offset);
    Split(cur_leaf_node);
  }
  if (this->file_header_->root_offset != this->node_header_root_->offset) {
    this->ReadNodeHeader(this->node_header_root_, this->file_header_->root_offset);
  }
  delete cur_leaf_node;
  delete cur;
  return notDouble;
}

template<class T, class Key, int degree, class Compare, class Compare_>
sjtu::vector<T> BPlusTree<T, Key, degree, Compare, Compare_>::Search(const Key &key, bool &find) {
  NodeHeader *cur = new NodeHeader(*(this->node_header_root_));
  //NodeHeader *cur = this->node_header_root_;
  InternalNode *cur_internal_node = new InternalNode();
  ReadInternalNode(cur_internal_node, cur->offset);
  while (!cur_internal_node->header.is_leaf) {

    int index = Upper_Bound_Key(key, cur_internal_node->keys_, cur_internal_node->header.count_nodes);
    ReadInternalNode(cur_internal_node, cur_internal_node->children_offset[index]);
  }
  LeafNode *cur_leaf_node = new LeafNode();
  this->ReadLeafNode(cur_leaf_node, cur_internal_node->header.offset);
  delete cur_internal_node;


  int index = Upper_Bound_Key(key, cur_leaf_node->values, cur_leaf_node->header.count_nodes) - 1;
  int pre_index = Lower_Bound_Key(key, cur_leaf_node->values, cur_leaf_node->header.count_nodes, find);
  sjtu::vector<T> result;
  bool continue_find = find;
  int size_ = 0;
  while (continue_find || pre_index == 0) {
    //之所以这么写是在我Merge的时候，如果前面是 26 26 26 | 27 28 ... -> 26 26 | 26 27 28 ...
    //这时候 我后面删除了 26 会导致我读不到前面的数据 这时候要往前读一位
    if (continue_find) {
      find = true;
      size_ += index - pre_index + 1;
      //result.reserve(size_);
      for (int i = index; i >= pre_index; i--) {
        result.push_back(cur_leaf_node->values[i].second);
      }
    }
    if (pre_index == 0 && cur_leaf_node->pre_node_offset != -1) {
      ReadLeafNode(cur_leaf_node, cur_leaf_node->pre_node_offset);
      index = Upper_Bound_Key(key, cur_leaf_node->values, cur_leaf_node->header.count_nodes) - 1;
      pre_index = Lower_Bound_Key(key, cur_leaf_node->values, cur_leaf_node->header.count_nodes, continue_find);
    } else {
      break;
    }
  }
  delete cur;
  delete cur_leaf_node;
  return result;
}

template<class T, class Key, int degree, class Compare, class Compare_>
bool BPlusTree<T, Key, degree, Compare, Compare_>::Update(const Key &key, const T &value) {
  NodeHeader *cur = this->node_header_root_;
  InternalNode *cur_internal_node = new InternalNode();
  ReadInternalNode(cur_internal_node, cur->offset);
  while (!cur_internal_node->header.is_leaf) {
    int index = Upper_Bound_Key(key, cur_internal_node->keys_, cur_internal_node->header.count_nodes);
    ReadInternalNode(cur_internal_node,cur_internal_node->children_offset[index]);
  }
  LeafNode *cur_leaf_node = new LeafNode();
  this->ReadLeafNode(cur_leaf_node, cur_internal_node->header.offset);
  delete cur_internal_node;

  bool find = false;
  int index = Upper_Bound(key, value, cur_leaf_node->values, cur_leaf_node->header.count_nodes) - 1;
  int pre_index = Lower_Bound(key, value, cur_leaf_node->values, cur_leaf_node->header.count_nodes, find);
  if (find) {
    cur_leaf_node->values[pre_index].second = value;
    WriteLeafNode(cur_leaf_node, cur_leaf_node->header.offset);
  }
  ReadNodeHeader(this->node_header_root_, this->file_header_->root_offset);
  delete cur_leaf_node;
  return find;
}


#endif // BPLUSTREE_TCC
