#pragma once
#include <initializer_list>
#include <iostream>
#include <memory>

template <typename T, typename Allocator = std::allocator<T>>
class List {
 private:
  ///////////////NODE/////////////////

  class Node {
   public:
    T node_value;
    Node* left = nullptr;
    Node* right = nullptr;
    Node(const T& value, Node* lft = nullptr, Node* rght = nullptr)
        : node_value(value), left(lft), right(rght) {}
    // Node(T&& value, Node* lft = nullptr, Node* rght = nullptr)
    //     : node_value(value), left(lft), right(rght) {}
    Node() : node_value(T()) {}

    void swap(Node& other) {
      node_value = other.node_value;
      std::swap(*right, *other.right);
      std::swap(*left, *other.left);
      std::swap(right->left, other.right->left);
      std::swap(left->right, other.left->right);
    }
  };
  class Static {
   public:
    Node* fake = nullptr;
  };
  //////////////NODE END//////////////////

 public:
  ///////////usings////////////
  using value_type = T;
  using allocator_type = Allocator;
  using node_allocator_type =
      typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
  using node_allocator_traits =
      typename std::allocator_traits<node_allocator_type>;
  /////////////////////////////////////

 private:
  /////////////Attributes//////////////////
  node_allocator_type allocator_;  //= Allocator();
  Static ring_;
  size_t size_ = 0;
  /////////////////////////////////////////

 public:
  ///////////////////////Iterator////////////////////////////

  template <bool IsConst>
  class ListIterator
      : public std::iterator<
            std::bidirectional_iterator_tag,
            typename std::conditional<IsConst, const T, T>::type> {
   private:
    Node* ptr_ = nullptr;

   public:
    ///////////////////////////TYPES//////////////////////////////////////

    typedef typename std::conditional<IsConst, const T, T>::type Ttype;
    using value_type = typename std::iterator<std::bidirectional_iterator_tag,
                                              Ttype>::value_type;
    using pointer =
        typename std::iterator<std::bidirectional_iterator_tag, Ttype>::pointer;
    using difference_type =
        typename std::iterator<std::bidirectional_iterator_tag,
                               Ttype>::difference_type;
    using reference = typename std::iterator<std::bidirectional_iterator_tag,
                                             Ttype>::reference;

    //////////////////////////////////////////////////////////////////////

    /////////////////Iterator constructors///////////////////////////

    ListIterator() = default;

    ListIterator(Node* ptr) : ptr_(ptr) {}

    ListIterator(const ListIterator<IsConst>& other) : ptr_(other.ptr_) {}

    //////////////////////////////////////////////////////////////////////
    //////////////Iterator operators/////////////

    ///////////////////assignment operator/////////////////////////

    void operator=(const ListIterator& other) { ptr_ = other.ptr_; }

    ///////////////////////////////////////////////////////////////

    /////////////////iterator increment/////////////////

    ListIterator<IsConst>& operator++() {
      ptr_ = ptr_->right;
      return *this;
    }

    ListIterator<IsConst> operator++(int) {
      ListIterator<IsConst> temp(*this);
      ptr_ = ptr_->right;
      return temp;
    }

    /////////////////////////////////////////////////////

    ////////////////iterator decrement///////////////

    ListIterator<IsConst>& operator--() {
      ptr_ = ptr_->left;
      return *this;
    }

    ListIterator<IsConst> operator--(int) {
      ListIterator<IsConst> temp(*this);
      ptr_ = ptr_->left;
      return temp;
    }

    //////////////////////////////////////////////////

    //////////////////access operators////////////////////

    reference operator*() const { return ptr_->node_value; }
    pointer operator->() const { return &(ptr_->node_value); }

    //////////////////////////////////////////////////////

    ////////////////////compare operators////////////////////////
    friend bool operator==(const ListIterator<IsConst>& iter1,
                           const ListIterator<IsConst>& iter2) {
      return iter1.ptr_ == iter2.ptr_;
    }

    friend bool operator!=(const ListIterator<IsConst>& iter1,
                           const ListIterator<IsConst>& iter2) {
      return !(iter1 == iter2);
    }
    ////////////////////////////////////////////////////////////
  };

  ////////////////////Iterator using////////////////////////
  using iterator = ListIterator<false>;
  using const_iterator = ListIterator<true>;
  using reverse_iterator = std::reverse_iterator<ListIterator<false>>;
  using const_reverse_iterator = std::reverse_iterator<ListIterator<true>>;
  //////////////////////////////////////////////////////////

  ////////////////////Iterator Methods///////////////////////
  iterator begin() { return iterator(ring_.fake->right); }

  iterator end() { return iterator(ring_.fake); }

  iterator begin() const { return iterator(ring_.fake->right); }

  iterator end() const { return iterator(ring_.fake); }

  const_iterator cbegin() { return const_iterator(ring_.fake->right); }

  const_iterator cend() { return const_iterator(ring_.fake); }

  const_iterator cbegin() const { return const_iterator(ring_.fake->right); }

  const_iterator cend() const { return const_iterator(ring_.fake); }

  reverse_iterator rbegin() { return reverse_iterator(end()); }

  reverse_iterator rend() { return reverse_iterator(begin()); }

  const_reverse_iterator crbegin() { return const_reverse_iterator(cend()); }

  const_reverse_iterator crend() { return const_reverse_iterator(cbegin()); }
  ////////////////////////////////////////////////////////////

  ///////////////Iterator End////////////////////////////////

 private:
  ///////////////////////Create Node
  /// Methods///////////////////////////////////////
  Node* create_fake_node() {
    Node* node = node_allocator_traits::allocate(allocator_, 1);
    node->left = nullptr;
    node->right = nullptr;
    return node;
  }

  Node* create_node(const T& value) {
    Node* node = node_allocator_traits::allocate(allocator_, 1);
    try {
      node_allocator_traits::construct(allocator_, node, value);
    } catch (...) {
      node_allocator_traits::deallocate(allocator_, node, 1);
      throw;
    }
    return node;
  }

  Node* create_node(T&& value) {
    Node* node = node_allocator_traits::allocate(allocator_, 1);
    try {
      node_allocator_traits::construct(allocator_, node, std::move(value));
    } catch (...) {
      node_allocator_traits::deallocate(allocator_, node, 1);
    }
    return node;
  }

  Node* create_default_node() {
    Node* node = node_allocator_traits::allocate(allocator_, 1);
    try {
      node_allocator_traits::construct(allocator_, node);
    } catch (...) {
      node_allocator_traits::deallocate(allocator_, node, 1);
      throw;
    }
    return node;
  }
  ///////////////////////////////////////////////////////////////////////////////////

  //////////emplace_back//////////////
  void emplace_back() {
    try {
      if (size_ == 0) {
        ring_.fake = create_fake_node();
      }
      Node* temp = create_default_node();
      if (ring_.fake->left == nullptr) {
        ring_.fake->right = temp;
        temp->left = ring_.fake;
      } else {
        ring_.fake->left->right = temp;
        temp->left = ring_.fake->left;
      }
      temp->right = ring_.fake;
      ring_.fake->left = temp;
      ++size_;
    } catch (...) {
      clear();
      throw;
    }
  }
  ////////////////////////////////////

  ///////////////////Clear///////////////////////
  void clear() {
    while (size_ > 0) {
      pop_back();
    }
  }
  //////////////////////////////////////////////

 public:
  ////////////////////////////////Constructors////////////////////////////////////

  explicit List(Allocator alloc = Allocator()) : allocator_(alloc) {}

  List(size_t count, const T& value, const Allocator& alloc = Allocator()) try
      : allocator_(alloc) {
    while (count-- != 0) {
      try {
        push_back(value);
      } catch (...) {
        clear();
        throw;
      }
    }
  } catch (...) {
    // clear();
    throw;
  }

  explicit List(size_t count, const Allocator& alloc = Allocator()) try
      : allocator_(alloc) {
    while (count-- != 0) {
      emplace_back();
    }
  } catch (...) {
    // clear();
    throw;
  }

  List(const List& other) try
      : allocator_(
            std::allocator_traits<Allocator>::
                select_on_container_copy_construction(other.allocator_)) {
    Node* temp = other.ring_.fake;
    for (size_t i = 0; i < other.size_; ++i) {
      temp = temp->right;
      try {
        push_back(temp->node_value);
      } catch (...) {
        clear();
        throw;
      }
    }
  } catch (...) {
    // clear();
    throw;
  }

  List(std::initializer_list<T> init, const Allocator& alloc = Allocator()) try
      : allocator_(alloc) {
    auto iter = init.begin();
    while (iter != init.end()) {
      push_back(*(iter++));
    }
  } catch (...) {
    // clear();
    throw;
  }
  //////////////////////////////////////////////////////////////////////////////////////////////

  /////////Destructor////////////
  ~List() { clear(); }
  ////////////////////////////////

  /////////Assignment Operator//////////////

  List& operator=(const List& other) {
    if (std::allocator_traits<
            Allocator>::propagate_on_container_copy_assignment::value) {
      clear();
      allocator_ = other.allocator_;
      for (const auto& elem : other) {
        push_back(elem);
      }
    } else {
      try {
        size_t psz = size_;
        for (const auto& elem : other) {
          push_back(elem);
          if (psz != 0) {
            pop_front();
            --psz;
          }
        }
        while (psz-- != 0) {
          pop_front();
        }
      } catch (...) {
        throw;
      }
    }
    return *this;
  }

  //////////////////////////////////////////

  ///////////METHODS/////////////

  //////////////////////Pushs//////////////////////

  void push_back(const T& value) {
    try {
      if (size_ == 0) {
        ring_.fake = create_fake_node();
      }
      Node* temp = create_node(value);
      if (size_ == 0) {
        ring_.fake->right = temp;
        temp->left = ring_.fake;
      } else {
        ring_.fake->left->right = temp;
        temp->left = ring_.fake->left;
      }
      temp->right = ring_.fake;
      ring_.fake->left = temp;
      ++size_;
    } catch (...) {
      throw;
    }
  }

  void push_front(const T& value) {
    try {
      if (size_ == 0) {
        ring_.fake = create_fake_node();
      }
      Node* temp = create_node(value);
      if (size_ == 0) {
        ring_.fake->left = temp;
        temp->right = ring_.fake;
      } else {
        ring_.fake->right->left = temp;
        temp->right = ring_.fake->right;
      }
      temp->left = ring_.fake;
      ring_.fake->right = temp;
      ++size_;
    } catch (...) {
      throw;
    }
  }

  void push_back(T&& value) {
    try {
      if (size_ == 0) {
        ring_.fake = create_fake_node();
      }
      Node* temp = create_node(std::move(value));
      if (size_ == 0) {
        ring_.fake->right = temp;
        temp->left = ring_.fake;
      } else {
        ring_.fake->left->right = temp;
        temp->left = ring_.fake->left;
      }
      temp->right = ring_.fake;
      ring_.fake->left = temp;
      ++size_;
    } catch (...) {
      throw;
    }
  }

  void push_front(T&& value) {
    try {
      if (size_ == 0) {
        ring_.fake = create_fake_node();
      }
      Node* temp = create_node(std::move(value));
      if (size_ == 0) {
        ring_.fake->left = temp;
        temp->right = ring_.fake;
      } else {
        ring_.fake->right->left = temp;
        temp->right = ring_.fake->right;
      }
      temp->left = ring_.fake;
      ring_.fake->right = temp;
      ++size_;
    } catch (...) {
      throw;
    }
  }

  ///////////////////////////////////////////////////////

  /////////////////////////Pops///////////////////////////

  void pop_back() {
    if (size_ > 0) {
      Node* node = ring_.fake->left;
      node->left->right = node->right;
      node->right->left = node->left;
      node->left = node->right = nullptr;
      node_allocator_traits::destroy(allocator_, node);
      node_allocator_traits::deallocate(allocator_, node, 1);
      if (--size_ == 0) {
        node_allocator_traits::deallocate(allocator_, ring_.fake, 1);
        ring_.fake = nullptr;
      }
    }
  }

  void pop_front() {
    if (size_ > 0) {
      Node* node = ring_.fake->right;
      node->left->right = node->right;
      node->right->left = node->left;
      node->left = node->right = nullptr;
      node_allocator_traits::destroy(allocator_, node);
      node_allocator_traits::deallocate(allocator_, node, 1);
      --size_;
      if (size_ == 0) {
        node_allocator_traits::deallocate(allocator_, ring_.fake, 1);
        ring_.fake = nullptr;
      }
    }
  }

  //////////////////////////////////////////////////////////

  /////////////////////Element access///////////////////////
  T& front() { return *begin(); }
  const T& front() const { return *cbegin(); }

  T& back() { return ring_.fake->left->node_value; }
  const T& back() const { return ring_.fake->left->node_value; }

  //////////////////////////////////////////////////////////////

  ////////////////////empty and size////////////////////////////

  bool empty() const { return size_ == 0; }
  size_t size() const { return size_; }

  //////////////////////////////////////////////////////////////

  ////////////////////get allocator//////////////////////////////

  node_allocator_type& get_allocator() { return allocator_; }

  ///////////////////////////////////////////////////////////////
};
