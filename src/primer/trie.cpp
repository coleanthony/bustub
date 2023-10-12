#include "primer/trie.h"
#include <cstddef>
#include <filesystem>
#include <memory>
#include <stack>
#include <string>
#include <string_view>
#include <vector>
#include "common/exception.h"

namespace bustub {

template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
  //throw NotImplementedException("Trie::Get is not implemented.");

  // You should walk through the trie to find the node corresponding to the key. If the node doesn't exist, return
  // nullptr. After you find the node, you should use `dynamic_cast` to cast it to `const TrieNodeWithValue<T> *`. If
  // dynamic_cast returns `nullptr`, it means the type of the value is mismatched, and you should return nullptr.
  // Otherwise, return the value.
  if (key.length()==0) {
    return nullptr;
  }
  auto root=this->root_;
  if (root==nullptr) {
    return nullptr;
  }
  for (auto c:key) {
    if (!root->children_.count(c)) {
      return nullptr;
    }
    root=root->children_.at(c);
  }
  const auto *valnode=dynamic_cast<const TrieNodeWithValue<T>*>(root.get());
  return valnode?valnode->value_.get():nullptr;
}

template <class T>
auto Trie::Put(std::string_view key, T value) const -> Trie {
  // Note that `T` might be a non-copyable type. Always use `std::move` when creating `shared_ptr` on that value.
  //throw NotImplementedException("Trie::Put is not implemented.");

  // You should walk through the trie and create new nodes if necessary. If the node corresponding to the key already
  // exists, you should create a new `TrieNodeWithValue`.
  auto node=this->root_;
  std::shared_ptr<T> shared_val=std::make_shared<T>(value);  //make shared value
  std::vector<std::shared_ptr<const TrieNode>> nodestack;
  int startkey=0;
  int keylen=key.length();
  for (auto c:key) {
    auto find=node->children_.find(c);
    if (find==node->children_.end()) {
      node=nullptr;
      break;
    }
    nodestack.push_back(node);
    node=node->children_.at(c);
    startkey++;
  }
  //create the different node;
  std::shared_ptr<const TrieNodeWithValue<T>> new_leafnode=node==nullptr?std::make_shared<TrieNodeWithValue<T>>(shared_val):std::make_shared<TrieNodeWithValue<T>>(node->children_,shared_val);
  std::shared_ptr<const TrieNode> child_node=new_leafnode;
  while (startkey<keylen) {
    char c=key[--keylen];
    std::map<char, std::shared_ptr<const TrieNode>> children{{c,child_node}};
    node=std::make_shared<const TrieNode>(children);
    child_node=node;
  }
  //copy the previous node;
  //construct the new tree from the bottom to top
  std::shared_ptr<TrieNode> uppernode;
  for (int i=nodestack.size()-1; i>=0; i--) {
    uppernode=std::shared_ptr<TrieNode>(nodestack[i]->Clone());
    //get it's children
    char c=key[i];
    uppernode->children_[c]=child_node;
    child_node=uppernode;
  }
  //construct the trie and return
  return Trie(uppernode);
}

auto Trie::Remove(std::string_view key) const -> Trie {
  //throw NotImplementedException("Trie::Remove is not implemented.");

  // You should walk through the trie and remove nodes if necessary. If the node doesn't contain a value any more,
  // you should convert it to `TrieNode`. If a node doesn't have children any more, you should remove it.
  auto node=this->root_;
  std::vector<std::shared_ptr<const TrieNode>> nodestack;
  for (auto c: key) {
    if (node==nullptr) {return *this;}
    auto find=node->children_.find(c);
    if (find==node->children_.end()) {return *this;}
    nodestack.push_back(node);
    node=node->children_.at(c);
  }
  if (!node->is_value_node_) {
    return *this;
  }
  std::shared_ptr<const TrieNode> lastnode=node->children_.empty()?nullptr:std::make_shared<const TrieNode>(node->children_);
  int startdel=nodestack.size()-1;
  for (; startdel>=0&&lastnode==nullptr; startdel--) {
    if (nodestack[startdel]->children_.size()>1) {
      break;
    }
  }
  if (startdel<0) {
    return Trie(nullptr);
  }
  std::shared_ptr<TrieNode> uppernode;
  for (;startdel>=0; startdel--) {
    uppernode=std::shared_ptr<TrieNode>(nodestack[startdel]->Clone());
    char c=key[startdel];
    uppernode->children_[c]=lastnode;
    lastnode=uppernode;
  }
  return Trie(uppernode);
}

// Below are explicit instantiation of template functions.
//
// Generally people would write the implementation of template classes and functions in the header file. However, we
// separate the implementation into a .cpp file to make things clearer. In order to make the compiler know the
// implementation of the template functions, we need to explicitly instantiate them here, so that they can be picked up
// by the linker.

template auto Trie::Put(std::string_view key, uint32_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint32_t *;

template auto Trie::Put(std::string_view key, uint64_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint64_t *;

template auto Trie::Put(std::string_view key, std::string value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const std::string *;

// If your solution cannot compile for non-copy tests, you can remove the below lines to get partial score.

using Integer = std::unique_ptr<uint32_t>;

template auto Trie::Put(std::string_view key, Integer value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const Integer *;

template auto Trie::Put(std::string_view key, MoveBlocked value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const MoveBlocked *;

}  // namespace bustub
