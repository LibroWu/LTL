#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu {

    template<class T>
    class deque {
    private:
        class Node {
        public:
            T value;
            Node *next;

            Node(T e) : value(e), next(nullptr) {}
        };

        class Block {
        public:
            const size_t merge_Threshold = 200;
            const size_t split_Threshold = 400;
            size_t num;
            Node *head, *tail;
            Block *next;

            Block() : num(0), head(nullptr), next(nullptr), tail(nullptr) {}

            Node *_copy(Node *&ptr, Node *&other) {
                if (!other) return nullptr;
                ptr = new Node(other->value);
                Node *tmp = _copy(ptr->next, other->next);
                return ((!tmp) ? ptr : tmp);
            }

            Block(Block *other) : num(other->num), next(nullptr) {
                tail = _copy(head, other->head);
            }

            ~Block() {
                for (Node *i = head, *j; i; i = j) {
                    j = i->next;
                    delete i;
                }
            }

            //Merge with next block
            void Merge() {
                if (!next) return;
                num = num + next->num;
                if (!head) {
                    head = next->head;
                    tail = next->tail;
                }
                else {
                    tail->next = next->head;
                    tail = next->tail;
                }
                Block *ptr = next;
                next = next->next;
                delete ptr;
            }

            //split this block into two blocks
            void Split() {
                Block *tmp = new Block;
                Node *mid = head;
                num >>= 1;
                tmp->num = num;
                for (size_t i = 1; i < num; ++i) mid = mid->next;
                tmp->head = mid->next;
                tmp->tail = tail;
                tail = mid;
                tail->next = nullptr;
                tmp->next = next;
                next = tmp;
            }

            void push_back(const T &element) {
                if (!num) {
                    head = tail = new Node(element);
                }
                else {
                    tail->next = new Node(element);
                    tail = tail->next;
                }
                ++num;
                if (num == split_Threshold) Split();
            }

            void add(Node *ptr, size_t pos) {
                ++num;
                if (!pos) {
                    ptr->next = head;
                    head = ptr;
                }
                else {
                    Node *tmp = head;
                    for (size_t i = 1; i < pos; ++i) tmp = tmp->next;
                    ptr->next = tmp->next;
                    tmp->next = ptr;
                }
                if (num == split_Threshold) Split();
            }
        };

        Block *_copy(Block *&ptr, Block *&other) {
            if (!other) return nullptr;
            ptr = new Block(other);
            Block *tmp = _copy(ptr->next, other->next);
            return ((!tmp) ? ptr : tmp);
        }

        Block *head, *tail;
        size_t Block_num, Node_num;
    public:
        class const_iterator;

        class iterator {
        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
        public:
            /**
             * return a new iterator which pointer n-next elements
             *   if there are not enough elements, iterator becomes invalid
             * as well as operator-
             */
            iterator operator+(const int &n) const {
                //TODO
            }

            iterator operator-(const int &n) const {
                //TODO
            }

            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const {
                //TODO
            }

            iterator &operator+=(const int &n) {
                //TODO
            }

            iterator &operator-=(const int &n) {
                //TODO
            }

            /**
             * TODO iter++
             */
            iterator operator++(int) {}

            /**
             * TODO ++iter
             */
            iterator &operator++() {}

            /**
             * TODO iter--
             */
            iterator operator--(int) {}

            /**
             * TODO --iter
             */
            iterator &operator--() {}

            /**
             * TODO *it
             * 		throw if iterator is invalid
             */
            T &operator*() const {}

            /**
             * TODO it->field
             * 		throw if iterator is invalid
             */
            T *operator->() const noexcept {}

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const {}

            bool operator==(const const_iterator &rhs) const {}

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {}

            bool operator!=(const const_iterator &rhs) const {}
        };

        class const_iterator {
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
        private:
            // data members.
        public:
            const_iterator() {
                // TODO
            }

            const_iterator(const const_iterator &other) {
                // TODO
            }

            const_iterator(const iterator &other) {
                // TODO
            }
            // And other methods in iterator.
            // And other methods in iterator.
            // And other methods in iterator.
        };

        /**
         * TODO Constructors
         */
        deque() : head(nullptr), tail(nullptr), Block_num(0), Node_num(0) {}

        deque(const deque &other) : head(nullptr), tail(nullptr), Block_num(0), Node_num(0) {
            tail = _copy(head, other.head);
        }

        /**
         * TODO Deconstructor
         */
        ~deque() {
            for (Block *i = head, *j; i; i = j) {
                j = i->next;
                delete i;
            }
        }

        /**
         * TODO assignment operator
         */
        deque &operator=(const deque &other) {
            if (this == &other) return *this;
            Block_num = other.Block_num;
            Node_num = other.Node_num;
            _copy(head, other.head);
        }

        /**
         * access specified element with bounds checking
         * throw index_out_of_bound if out of bound.
         */
        T &at(const size_t &pos) {}

        const T &at(const size_t &pos) const {}

        T &operator[](const size_t &pos) {}

        const T &operator[](const size_t &pos) const {}

        /**
         * access the first element
         * throw container_is_empty when the container is empty.
         */
        const T &front() const {}

        /**
         * access the last element
         * throw container_is_empty when the container is empty.
         */
        const T &back() const {}

        /**
         * returns an iterator to the beginning.
         */
        iterator begin() {}

        const_iterator cbegin() const {}

        /**
         * returns an iterator to the end.
         */
        iterator end() {}

        const_iterator cend() const {}

        /**
         * checks whether the container is empty.
         */
        bool empty() const {
            return (Node_num == 0);
        }

        /**
         * returns the number of elements
         */
        size_t size() const {
            return Node_num;
        }

        /**
         * clears the contents
         */
        void clear() {
            for (Block *i = head, *j; i; i = j) {
                j = i->next;
                delete i;
            }
            head = nullptr;
            tail = nullptr;
            Block_num = Node_num = 0;
        }

        /**
         * inserts elements at the specified locat on in the container.
         * inserts value before pos
         * returns an iterator pointing to the inserted value
         *     throw if the iterator is invalid or it point to a wrong place.
         */
        iterator insert(iterator pos, const T &value) {}

        /**
         * removes specified element at pos.
         * removes the element at pos.
         * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
         * throw if the container is empty, the iterator is invalid or it points to a wrong place.
         */
        iterator erase(iterator pos) {}

        /**
         * adds an element to the end
         */
        void push_back(const T &value) {
            if (!Block_num) {
                head=tail=new Block;
                tail->push_back(value);
            }else {
                tail->push_back(value);
            }
        }

        /**
         * removes the last element
         *     throw when the container is empty.
         */
        void pop_back() {
        }

        /**
         * inserts an element to the beginning.
         */
        void push_front(const T &value) {}

        /**
         * removes the first element.
         *     throw when the container is empty.
         */
        void pop_front() {}
    };

}

#endif
