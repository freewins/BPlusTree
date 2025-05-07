//
// Created by Freewings on 2025/5/7.
//

#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <climits>
#include <cstddef>
#include <stdexcept>
#include <memory>

namespace sjtu {
  class exception {
    protected:
     const std::string variant = "";
     std::string detail = "";

    public:
     exception() {
     }
     exception(const exception &ec) : variant(ec.variant), detail(ec.detail) {
     }
     virtual std::string what() {
         return variant + " " + detail;
     }
 };

 class index_out_of_bound : public exception {
     /* __________________________ */
 };

 class runtime_error : public exception {
     /* __________________________ */
 };

 class invalid_iterator : public exception {
     /* __________________________ */
 };

 class container_is_empty : public exception {
     /* __________________________ */
 };




  /**
   * a data container like std::vector
   * store data in a successive memory and support random access.
   */
  template<typename T>
  class vector {
  private:
    T *data;
    std::allocator<T> alloc;
    using Traits = std::allocator_traits<std::allocator<T> >;
    // _size - The total number of the vector saved.
    // _capacity - The space it uses to store datas.
    // if not empty , the _size is from 1 ~ n
    size_t _size, _capacity;

  public:
    /**
     * TODO
     * a type for actions of the elements of a vector, and you should write
     *   a class named const_iterator with same interfaces.
     */
    /**
     * you can see RandomAccessIterator at CppReference for help.
     */
    class const_iterator;

    class reverse_iterator;

    class iterator {
      // The following code is written for the C++ type_traits library.
      // Type traits is a C++ feature for describing certain properties of a type.
      // For instance, for an iterator, iterator::value_type is the type that the
      // iterator points to.
      // STL algorithms and containers may use these type_traits (e.g. the following
      // typedef) to work properly. In particular, without the following code,
      // @code{std::sort(iter, iter1);} would not compile.
      // See these websites for more information:
      // https://en.cppreference.com/w/cpp/header/type_traits
      // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
      // About iterator_category: https://en.cppreference.com/w/cpp/iterator
    public:
      using difference_type = std::ptrdiff_t;
      using value_type = T;
      using pointer = T *;
      using reference = T &;
      using iterator_category = std::output_iterator_tag;

    private:
      /**
       * TODO add data members
       *   just add whatever you want.
       */
      //friend class const_iterator;
      pointer ptr;

    public:


      iterator(pointer ptr_) : ptr(ptr_) {
      }

      /**
       * return a new iterator which pointer n-next elements
       * as well as operator-
       */
      iterator operator+(const int &n) const {
        //TODO
        return iterator(ptr + n);
      }

      iterator operator-(const int &n) const {
        //TODO
        return iterator(ptr - n);
      }

      // return the distance between two iterators,
      // if these two iterators point to different vectors, throw invaild_iterator.
      int operator-(const iterator &rhs) const {
        //TODO
        return abs(ptr - rhs.ptr);
      }

      iterator &operator+=(const int &n) {
        //TODO
        this->ptr += n;
        return *this;
      }

      iterator &operator-=(const int &n) {
        //TODO
        this->ptr -= n;
        return *this;
      }

      /**
       * TODO iter++
       */
      iterator operator++(int) {
        iterator tmp(*this);
        tmp.ptr = this->ptr;
        this->ptr = this->ptr + 1;
        return tmp;
      }

      /**
       * TODO ++iter
       */
      iterator &operator++() {
        this->ptr = this->ptr + 1;
        return *this;
      }

      /**
       * TODO iter--
       */
      iterator operator--(int) {
        iterator tmp;
        tmp.ptr = this->ptr;
        this->ptr = this->ptr - 1;
        return tmp;
      }

      /**
       * TODO --iter
       */
      iterator &operator--() {
        this->ptr = this->ptr - 1;
        return *this;
      }

      /**
       * TODO *it
       */
      T &operator*() const {
        return *ptr;
      }

      /**
       * a operator to check whether two iterators are same (pointing to the same memory address).
       */
      bool operator==(const iterator &rhs) const {
        return ptr == rhs.ptr;
      }

      bool operator==(const const_iterator &rhs) const {
        return ptr == rhs.ptr;
      }

      /**
       * some other operator for iterator.
       */
      bool operator!=(const iterator &rhs) const {
        return ptr != rhs.ptr;
      }

      bool operator!=(const const_iterator &rhs) const {
        return ptr != rhs.ptr;
      }
    };

    class reverse_iterator {
      // The following code is written for the C++ type_traits library.
      // Type traits is a C++ feature for describing certain properties of a type.
      // For instance, for an iterator, iterator::value_type is the type that the
      // iterator points to.
      // STL algorithms and containers may use these type_traits (e.g. the following
      // typedef) to work properly. In particular, without the following code,
      // @code{std::sort(iter, iter1);} would not compile.
      // See these websites for more information:
      // https://en.cppreference.com/w/cpp/header/type_traits
      // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
      // About iterator_category: https://en.cppreference.com/w/cpp/iterator
    public:
      using difference_type = std::ptrdiff_t;
      using value_type = T;
      using pointer = T *;
      using reference = T &;
      using iterator_category = std::output_iterator_tag;

    private:
      /**
       * TODO add data members
       *   just add whatever you want.
       */
      //friend class const_iterator;
      pointer ptr;

    public:


      reverse_iterator(pointer ptr_) : ptr(ptr_) {
      }

      /**
       * return a new iterator which pointer n-next elements
       * as well as operator-
       */
      reverse_iterator operator+(const int &n) const {
        //TODO
        return iterator(ptr + n);
      }

      reverse_iterator operator-(const int &n) const {
        //TODO
        return iterator(ptr - n);
      }

      // return the distance between two iterators,
      // if these two iterators point to different vectors, throw invaild_iterator.
      int operator-(const reverse_iterator &rhs) const {
        //TODO
        return abs(ptr - rhs.ptr);
      }

      reverse_iterator &operator+=(const int &n) {
        //TODO
        this->ptr += n;
        return *this;
      }

      reverse_iterator &operator-=(const int &n) {
        //TODO
        this->ptr -= n;
        return *this;
      }

      /**
       * TODO iter++
       */
      reverse_iterator operator++(int) {
        iterator tmp(*this);
        tmp.ptr = this->ptr;
        this->ptr = this->ptr - 1;
        return tmp;
      }

      /**
       * TODO ++iter
       */
      reverse_iterator &operator++() {
        this->ptr = this->ptr - 1;
        return *this;
      }

      /**
       * TODO iter--
       */
      iterator operator--(int) {
        iterator tmp;
        tmp.ptr = this->ptr;
        this->ptr = this->ptr + 1;
        return tmp;
      }

      /**
       * TODO --iter
       */
      iterator &operator--() {
        this->ptr = this->ptr + 1;
        return *this;
      }

      /**
       * TODO *it
       */
      T &operator*() const {
        return *ptr;
      }

      /**
       * a operator to check whether two iterators are same (pointing to the same memory address).
       */
      bool operator==(const reverse_iterator &rhs) const {
        return ptr == rhs.ptr;
      }

      bool operator==(const const_iterator &rhs) const {
        return ptr == rhs.ptr;
      }

      /**
       * some other operator for iterator.
       */
      bool operator!=(const reverse_iterator &rhs) const {
        return ptr != rhs.ptr;
      }

      bool operator!=(const const_iterator &rhs) const {
        return ptr != rhs.ptr;
      }
    };

    /**
     * TODO
     * has same function as iterator, just for a const object.
     */
    class const_iterator {
    public:
      using difference_type = std::ptrdiff_t;
      using value_type = T;
      using pointer = T *;
      using reference = T &;
      using iterator_category = std::output_iterator_tag;

    private:
      /*TODO*/
      pointer ptr;

    public:
      const_iterator(pointer ptr_) : ptr(ptr_) {
      }

      const_iterator operator+(const int &n) const {
        //TODO
        return const_iterator(ptr + n);
      }

      const_iterator operator-(const int &n) const {
        //TODO
        return const_iterator(ptr - n);
      }

      // return the distance between two iterators,
      // if these two iterators point to different vectors, throw invaild_iterator.
      int operator-(const const_iterator &rhs) const {
        //TODO
        return abs(ptr - rhs.ptr);
      }

      const_iterator operator--(int) {
        const_iterator tmp(ptr);
        this->ptr = this->ptr - 1;
        return tmp;
      }

      const_iterator &operator--() {
        this->ptr = this->ptr - 1;
        return *this;
      }

      const_iterator operator++(int) {
        const_iterator tmp(ptr);
        this->ptr = this->ptr + 1;
        return tmp;
      }

      const_iterator &operator++() {
        this->ptr = this->ptr + 1;
        return *this;
      }

      /**
       * TODO *it
       */
      const T &operator*() const {
        return *ptr;
      }

      /**
       * a operator to check whether two iterators are same (pointing to the same memory address).
       */
      bool operator==(const iterator &rhs) const {
        return ptr == rhs.ptr;
      }

      bool operator==(const const_iterator &rhs) const {
        return ptr == rhs.ptr;
      }

      /**
       * some other operator for iterator.
       */
      bool operator!=(const iterator &rhs) const {
        return ptr != rhs.ptr;
      }

      bool operator!=(const const_iterator &rhs) const {
        return ptr != rhs.ptr;
      }
    };

    /**
     * TODO Constructs
     * At least two: default constructor, copy constructor
     */
    vector() {
      _size = 0;
      _capacity = 10;
      data = Traits::allocate(alloc, _capacity);
    }

    vector(const vector &other) {
      this->_size = other._size;
      this->_capacity = 2 * this->_size;
      data = Traits::allocate(alloc, _capacity);
      for (size_t i = 0; i < other._size; i++)
        Traits::construct(alloc, data + i, other.data[i]);
    }

    /**
     * TODO Destructor
     */
    ~vector() {
      for (int i = 0; i < _size; i++)
        Traits::destroy(alloc, data + i);
      Traits::deallocate(alloc, data, _capacity);
      data = nullptr;
      _size = 0;
      _capacity = 0;
    }

    /**
     * TODO Assignment operator
     */
    vector &operator=(const vector &other) {
      if (this->data == other.data) {
        return *this;
      }
      for (size_t i = 0; i < _size; i++) {
        Traits::destroy(alloc, data + i);
      }
      Traits::deallocate(alloc, data, _capacity);
      this->_size = other._size;
      this->_capacity = 2 * this->_size;
      data = Traits::allocate(alloc, _capacity);
      for (size_t i = 0; i < other._size; i++)
        Traits::construct(alloc, data + i, other.data[i]);
      return *this;
    }

    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     */
    T &at(const size_t &pos) {
      if (pos < 0 || pos >= _size) throw sjtu::index_out_of_bound();
      return data[pos];
    }

    const T &at(const size_t &pos) const {
      if (pos < 0 || pos >= _size) throw sjtu::index_out_of_bound();
      return data[pos];
    }

    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     * !!! Pay attentions
     *   In STL this operator does not check the boundary but I want you to do.
     */
    T &operator[](const size_t &pos) {
      if (pos < 0 || pos >= _size) throw sjtu::index_out_of_bound();
      return data[pos];
    }

    const T &operator[](const size_t &pos) const {
      if (pos < 0 || pos >= _size) throw sjtu::index_out_of_bound();
      return data[pos];
    }

    /**
     * access the first element.
     * throw container_is_empty if size == 0
     */
    const T &front() const {
      if (_size == 0) throw sjtu::container_is_empty();
      else return data[0];
    }

    /**
     * access the last element.
     * throw container_is_empty if size == 0
     */
    const T &back() const {
      if (_size == 0) throw sjtu::container_is_empty();
      else return data[this->_size - 1];
    }

    /**
     * returns an iterator to the beginning.
     */
    iterator begin() {
      return iterator(data);
    }

    reverse_iterator rbegin() {
      return reverse_iterator(data + _size - 1);
    }
    reverse_iterator rend() {
      return reverse_iterator(data - 1);
    }
    const_iterator cbegin() const {
      return const_iterator(data);
    }

    /**
     * returns an iterator to the end.
     */
    iterator end() {
      return iterator(data + _size);
    }

    const_iterator cend() const {
      return const_iterator(data + _size);
    }

    /**
     * checks whether the container is empty
     */
    bool empty() const {
      return _size == 0;
    }

    /**
     * returns the number of elements
     */
    size_t size() const {
      return _size;
    }

    /**
     * clears the contents
     */
    void clear() {
      for (int i = 0; i < _size; i++)
        Traits::destroy(alloc, data + i);
      _size = 0;
    }

    /**
     * inserts value before pos
     * returns an iterator pointing to the inserted value.
     */
    iterator insert(iterator pos, const T &value) {
      if (_size == _capacity) {
        _capacity *= 2;
        T *new_data = Traits::allocate(alloc, _capacity);
        size_t start_pos = pos - this->begin();
        for (size_t i = start_pos; i < _size; i++) {
          Traits::construct(alloc, new_data + i + 1, data[i]);
          Traits::destroy(alloc, data + i);
        }
        Traits::construct(alloc, new_data + start_pos, value);
        for (size_t i = 0; i < start_pos; i++) {
          Traits::construct(alloc, new_data + i, data[i]);
          Traits::destroy(alloc, data + i);
        }
        Traits::deallocate(alloc, data, _size);
        _size++;
        data = new_data;
        return iterator(data + start_pos);
      } else {
        Traits::construct(alloc, data + _size, value);
        for (auto t = this->end(); t != pos; --t) {
          *t = *(t - 1);
        }
        *pos = value;
        _size++;
        return pos;
      }
    }

    /**
     * inserts value at index ind.
     * after inserting, this->at(ind) == value
     * returns an iterator pointing to the inserted value.
     * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
     */
    iterator insert(const size_t &ind, const T &value) {
      if (ind > _size) {
        throw sjtu::index_out_of_bound();
      } else if (_size == _capacity) {
        _capacity *= 2;
        T *new_data = Traits::allocate(alloc, _capacity);
        for (size_t i = ind; i < _size; i++) {
          Traits::construct(alloc, new_data + i + 1, data[i]);
          Traits::destroy(alloc, data + i);
        }
        Traits::construct(alloc, new_data + ind, value);
        for (size_t i = 0; i < ind; i++) {
          Traits::construct(alloc, new_data + i, data[i]);
          Traits::destroy(alloc, data + i);
        }
        Traits::deallocate(alloc, data, _size);
        _size++;
        data = new_data;
      } else {
        Traits::construct(alloc, data + _size, value);
        for (size_t i = _size; i > ind; i--) {
          data[i] = data[i - 1];
        }
        data[ind] = value;
        _size++;
      }
      return iterator(data + ind);
    }

    /**
     * removes the element at pos.
     * return an iterator pointing to the following element.
     * If the iterator pos refers the last element, the end() iterator is returned.
     */
    iterator erase(iterator pos) {
      for (size_t i = pos - data; i < _size; ++i) {
        data[i] = data[i + 1];
      }
      _size--;
      Traits::destroy(alloc, data + _size);
      return iterator(pos + 1);
    }

    /**
     * removes the element with index ind.
     * return an iterator pointing to the following element.
     * throw index_out_of_bound if ind >= size
     */
    iterator erase(const size_t &ind) {
      for (size_t i = ind; i < _size; i++) {
        data[i] = data[i + 1];
      }
      _size--;
      Traits::destroy(alloc, data + _size);
      return iterator(data + ind + 1);
    }

    /**
     * adds an element to the end.
     */
    void push_back(const T &value) {
      if (_size == _capacity) {
        _capacity *= 2;
        T *new_data = Traits::allocate(alloc, _capacity);
        for (size_t i = 0; i < _size; i++) {
          Traits::construct(alloc, new_data + i, data[i]);
          Traits::destroy(alloc, data + i);
        }
        Traits::deallocate(alloc, data, _size);
        Traits::construct(alloc, new_data + _size, value);
        data = new_data;
      } else {
        Traits::construct(alloc, data + _size, value);
      }
      _size++;
    }

    /**
     * remove the last element from the end.
     * throw container_is_empty if size() == 0
     */
    void pop_back() {
      if (_size == 0) {
        throw sjtu::container_is_empty();
      } else {
        --_size;
        Traits::destroy(alloc, data + _size);
      }
    }
  };
}

#endif