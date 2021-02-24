#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
    template<typename T>
    class vector {
    private:
        const size_t init_size = 5;
        T **data;
        size_t real_size, num;

        void doubleSpace() {
            T **tmp;
            real_size <<= 1;
            tmp = new T *[real_size];
            for (int i = 0; i < num; ++i) tmp[i] = data[i];
            delete[] data;
            data = tmp;
        }

        void shrinkSpace() {
            T **tmp;
            real_size >>= 1;
            tmp = new T *[real_size];
            for (int i = 0; i < num; ++i) tmp[i] = data[i];
            delete[] data;
            data = tmp;
        }

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

        class iterator {
            friend vector<T>;
        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
            int pos;
            const vector<T> *source;
        public:
            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */
            iterator operator+(const int &n) const {
                iterator tmp;
                tmp.pos = pos + n;
                tmp.source = source;
                return tmp;
            }

            iterator operator-(const int &n) const {
                iterator tmp;
                tmp.pos = pos - n;
                tmp.source = source;
                return tmp;
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const {
                if (source != rhs.source) throw invalid_iterator();
                return pos - rhs.pos;
            }

            iterator &operator+=(const int &n) {
                pos = pos + n;
                return *this;
            }

            iterator &operator-=(const int &n) {
                pos = pos - n;
                return *this;
            }

            /**
             * TODO iter++
             */
            iterator operator++(int) {
                iterator tmp = *this;
                ++pos;
                return tmp;
            }

            /**
             * TODO ++iter
             */
            iterator &operator++() {
                ++pos;
                return *this;
            }

            /**
             * TODO iter--
             */
            iterator operator--(int) {
                iterator tmp = *this;
                --pos;
                return tmp;
            }

            /**
             * TODO --iter
             */
            iterator &operator--() {
                --pos;
                return *this;
            }

            /**
             * TODO *it
             */
            T &operator*() const {
                return *(source->data[pos]);
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator &rhs) const {
                return (source == rhs.source && pos == rhs.pos);
            }

            bool operator==(const const_iterator &rhs) const {
                return (source == rhs.source && pos == rhs.pos);
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return (source != rhs.source || pos != rhs.pos);
            }

            bool operator!=(const const_iterator &rhs) const {
                return (source != rhs.source || pos != rhs.pos);
            }
        };

        /**
         * TODO
         * has same function as iterator, just for a const object.
         */
        class const_iterator {
            friend vector<T>;
        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
            const vector<T> *source;
            size_t pos;
        public:
            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */
            const_iterator operator+(const int &n) const {
                const_iterator tmp;
                tmp.pos = pos + n;
                tmp.source = source;
                return tmp;
            }

            const_iterator operator-(const int &n) const {
                const_iterator tmp;
                tmp.pos = pos - n;
                tmp.source = source;
                return tmp;
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.
            int operator-(const const_iterator &rhs) const {
                if (source != rhs.source) throw invalid_iterator();
                return pos - rhs.pos;
            }

            const_iterator &operator+=(const int &n) {
                pos = pos + n;
                return *this;
            }

            const_iterator &operator-=(const int &n) {
                pos = pos - n;
                return *this;
            }

            /**
             * TODO iter++
             */
            const_iterator operator++(int) {
                const_iterator tmp = *this;
                ++pos;
                return tmp;
            }

            /**
             * TODO ++iter
             */
            const_iterator &operator++() {
                ++pos;
                return *this;
            }

            /**
             * TODO iter--
             */
            const_iterator operator--(int) {
                iterator tmp = *this;
                --pos;
                return tmp;
            }

            /**
             * TODO --iter
             */
            const_iterator &operator--() {
                --pos;
                return *this;
            }

            /**
             * TODO *it
             */
            const T &operator*() const {
                return *(source->data[pos]);
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator &rhs) const {
                return (pos == rhs.pos && source == rhs.source);
            }

            bool operator==(const const_iterator &rhs) const {
                return (pos == rhs.pos && source == rhs.source);
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return (source != rhs.source || pos != rhs.pos);
            }

            bool operator!=(const const_iterator &rhs) const {
                return (source != rhs.source || pos != rhs.pos);
            }

        };

        /**
         * TODO Constructs
         * Atleast two: default constructor, copy constructor
         */
        vector() : real_size(init_size), num(0) {
            data = new T *[init_size];
        }

        vector(const vector &other) : real_size(other.real_size), num(other.num) {
            data = new T *[real_size];
            for (int i = 0; i < num; ++i) {
                data[i] = new T(*other.data[i]);
            }
        }

        /**
         * TODO Destructor
         */
        ~vector() {
            clear();
        }

        /**
         * TODO Assignment operator
         */
        vector &operator=(const vector &other) {
            if (this == &other) return *this;
            clear();
            real_size = other.real_size;
            num = other.num;
            data = new T *[real_size];
            for (int i = 0; i < num; ++i) {
                data[i] = new T(*other.data[i]);
            }
            return *this;
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, num)
         */
        T &at(const size_t &pos) {
            if (pos >= num || pos < 0) throw index_out_of_bound();
            return *data[pos];
        }

        const T &at(const size_t &pos) const {
            if (pos >= num || pos < 0) throw index_out_of_bound();
            return *data[pos];
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, num)
         * !!! Pay attentions
         *   In STL this operator does not check the boundary but I want you to do.
         */
        T &operator[](const size_t &pos) {
            if (pos >= num || pos < 0) throw index_out_of_bound();
            return *data[pos];
        }

        const T &operator[](const size_t &pos) const {
            if (pos >= num || pos < 0) throw index_out_of_bound();
            return *data[pos];
        }

        /**
         * access the first element.
         * throw container_is_empty if num == 0
         */
        const T &front() const {
            if (num == 0) throw container_is_empty();
            return *data[0];
        }

        /**
         * access the last element.
         * throw container_is_empty if num == 0
         */
        const T &back() const {
            if (num == 0) throw container_is_empty();
            return *data[num - 1];
        }

        /**
         * returns an iterator to the beginning.
         */
        iterator begin() {
            iterator tmp;
            tmp.pos = 0;
            tmp.source = this;
            return tmp;
        }

        const_iterator cbegin() const {
            const_iterator tmp;
            tmp.pos = 0;
            tmp.source = this;
            return tmp;
        }

        /**
         * returns an iterator to the end.
         */
        iterator end() {
            iterator tmp;
            tmp.pos = num;
            tmp.source = this;
            return tmp;
        }

        const_iterator cend() const {
            const_iterator tmp;
            tmp.pos = num;
            tmp.source = this;
            return tmp;
        }

        /**
         * checks whether the container is empty
         */
        bool empty() const {
            return (num == 0);
        }

        /**
         * returns the number of elements
         */
        size_t size() const {
            return num;
        }

        /**
         * clears the contents
         */
        void clear() {
            for (int i = 0; i < num; ++i)
                delete data[i];
            delete[] data;
            data = nullptr;
        }

        /**
         * inserts value before pos
         * returns an iterator pointing to the inserted value.
         */
        iterator insert(iterator pos, const T &value) {
            return insert(pos.pos, value);
        }

        /**
         * inserts value at index ind.
         * after inserting, this->at(ind) == value
         * returns an iterator pointing to the inserted value.
         * throw index_out_of_bound if ind > num (in this situation ind can be num because after inserting the num will increase 1.)
         */
        iterator insert(const size_t &ind, const T &value) {
            if (ind > num) throw index_out_of_bound();
            if (num == real_size) doubleSpace();
            for (size_t i = num; i > ind; --i) data[i] = data[i - 1];
            data[ind] = new T(value);
            ++num;
            iterator tmp;
            tmp.pos = ind;
            tmp.source = this;
            return tmp;
        }

        /**
         * removes the element at pos.
         * return an iterator pointing to the following element.
         * If the iterator pos refers the last element, the end() iterator is returned.
         */
        iterator erase(iterator pos) {
            return erase(pos.pos);
        }

        /**
         * removes the element with index ind.
         * return an iterator pointing to the following element.
         * throw index_out_of_bound if ind >= num
         */
        iterator erase(const size_t &ind) {
            if (ind >= num) throw index_out_of_bound();
            delete data[ind];
            for (size_t i = ind; i < num; ++i) data[i] = data[i + 1];
            --num;
            if (real_size > init_size && num < real_size >> 2) shrinkSpace();
            iterator tmp;
            tmp.source = this;
            tmp.pos = ind;
            return tmp;
        }

        /**
         * adds an element to the end.
         */
        void push_back(const T &value) {
            if (num == real_size) doubleSpace();
            data[num++] = new T(value);
        }

        /**
         * remove the last element from the end.
         * throw container_is_empty if num() == 0
         */
        void pop_back() {
            if (!num) throw container_is_empty();
            delete data[--num];
            if (real_size > init_size && num < real_size >> 2) shrinkSpace();
        }
    };


}

#endif
