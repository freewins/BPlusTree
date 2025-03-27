#include <iostream>
#include <cstring>
#include "BPlusTree.hpp"

void print_(std::vector<int> & res) {
  for (auto t = res.rbegin(); t != res.rend(); t++) {
    std::cout << *t << " ";
  }
  std::cout<<"\n";
}
int main() {
  struct Key {
    char key_[66] = {};
    Key() {
      memset(key_, 0, sizeof(key_));
    }
    Key(const char* key) {
      std::strcpy(key_, key);
      key_[strlen(key)] = '\0';
    }
    bool operator<(const Key &other) const {
      return strcmp(key_, other.key_) < 0;
    }
  };
  BPlusTree<int,Key,600> tree("data");
  std::string op;
  char key[66];
  Key t;
  std::vector<int> res;
  int n,tmp;
  std::cin >> n;
  for (int i = 1; i<= n; i++) {
    std::cin >> op;
    if (op == "insert") {
      std::cin >> key;
      std::cin >> tmp;
      tree.Insert(Key(key),tmp);
    }
    else if (op == "delete") {
      int tmp;
      std::cin >> key >> tmp;
      bool find = false;
      if (tree.Remove(Key(key),tmp)) {
      }
    }
    else if (op == "find") {
      std::cin >> key;
      bool find = false;
      res = tree.Search(Key(key), find);
      if (find) {
        print_(res);
      }
      else {
        std::cout << "null\n";
      }
    }else {
    }
  }
  return 0;
}