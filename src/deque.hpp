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
            Node *next, *pre;

            Node(T e) : value(e), next(nullptr), pre(nullptr) {}
        };

        class Block {
        public:
            const size_t merge_Threshold = 200;
            const size_t split_Threshold = 400;
            size_t num;
            Node *head, *tail;
            Block *next, *pre;
            deque<T> *source;

            Block(deque<T> *source) : source(source), num(0), head(nullptr), next(nullptr), pre(nullptr),
                                      tail(nullptr) {}

            Node *_copy(Node *&ptr, Node *&other) {
                if (!other) return nullptr;
                ptr = new Node(other->value);
                Node *tmp = _copy(ptr->next, other->next);
                if (!tmp) {
                    return ptr;
                }
                else {
                    ptr->next->pre = ptr;
                    return tmp;
                }
            }

            Block(Block *other, deque<T> *source) : source(source), num(other->num), next(nullptr), pre(nullptr) {
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
                ptr->pre = this;
                next = next->next;
                delete ptr;
                --source->Block_num;
            }

            //split this block into two blocks
            void Split() {
                ++source->Block_num;
                Block *tmp = new Block(source);
                Node *mid = head;
                num >>= 1;
                tmp->num = num;
                for (size_t i = 1; i < num; ++i) mid = mid->next;
                tmp->head = mid->next;
                tmp->tail = tail;
                tail = mid;
                tail->next = nullptr;
                tmp->next = next;
                if (next)
                    next->pre = tmp;
                tmp->pre = this;
                next = tmp;
            }

            bool push_back(const T &element) {
                if (!num) {
                    head = tail = new Node(element);
                }
                else {
                    tail->next = new Node(element);
                    tail->next->pre = tail;
                    tail = tail->next;
                }
                ++source->Node_num;
                ++num;
                if (num == split_Threshold) {
                    Split();
                    return true;
                }
                else return false;
            }

            bool add(Node *ptr, const size_t &pos) {
                ++source->Node_num;
                ++num;
                if (!pos) {
                    ptr->next = head;
                    if (head)
                        head->pre = ptr;
                    head = ptr;
                }
                else {
                    Node *tmp = head;
                    for (size_t i = 1; i < pos; ++i) tmp = tmp->next;
                    ptr->next = tmp->next;
                    ptr->pre = tmp;
                    if (ptr->next)
                        ptr->next->pre = ptr;
                    tmp->next = ptr;
                }
                if (num == split_Threshold) {
                    Split();
                    return true;
                }
                else return false;
            }

            bool erase(const size_t &pos) {
                --source->Node_num;
                --num;
                if (!pos) {
                    Node *ptr = head;
                    head = head->next;
                    if (head)
                        head->pre = nullptr;
                    delete ptr;
                }
                else if (pos == num) {
                    Node *ptr = tail;
                    tail = tail->pre;
                    if (tail)
                        tail->next = nullptr;
                    delete ptr;
                }
                else {
                    Node *tmp = head;
                    for (size_t i = 1; i < pos; ++i) tmp = tmp->next;
                    Node *ptr = tmp->next;
                    tmp->next = ptr->next;
                    tmp->next->pre = tmp;
                    delete ptr;
                }
                return (num == 0);
            }

            Node *get(const size_t &pos) {
                Node *ptr = head;
                for (size_t i = 0; i < pos; ++i) {
                    ptr = ptr->next;
                }
                return ptr;
            }
        };

        Block *_copy(Block *&ptr, Block *other) {
            if (!other) return nullptr;
            ptr = new Block(other, this);
            Block *tmp = _copy(ptr->next, other->next);
            if (!tmp) {
                return ptr;
            }
            else {
                ptr->next->pre = ptr;
                return tmp;
            }
        }

        Block *head, *tail;
        size_t Block_num, Node_num;
    public:
        class const_iterator;

        class iterator {
            friend deque<T>;
        private:
            bool invalid;
            Block *block_ptr;
            Node *node_ptr;
            size_t pos_in_block, pos_in_chain, pos;
            deque<T> *source;
        public:
            /**
             * return a new iterator which pointer n-next elements
             *   if there are not enough elements, iterator becomes invalid
             * as well as operator-
             */
            iterator operator+(const int &n) const {
                if (n < 0) return this->operator-(-n);
                iterator tmp;
                tmp.pos = pos + n;
                if (tmp.pos >= source->Node_num) {
                    tmp.invalid = true;
                    return tmp;
                }
                tmp.block_ptr = block_ptr;
                tmp.node_ptr = node_ptr;
                tmp.pos_in_block = pos_in_block;
                tmp.pos_in_chain = pos_in_chain;
                tmp.invalid = false;
                int t = n;
                while (t >= tmp.block_ptr->num - tmp.pos_in_block) {
                    t -= tmp.block_ptr->num - tmp.pos_in_block;
                    tmp.block_ptr = tmp.block_ptr->next;
                    if (!tmp.block_ptr) {
                        tmp.invalid = true;
                        return tmp;
                    }
                    tmp.pos_in_block = 0;
                    ++tmp.pos_in_chain;
                }
                tmp.pos_in_block = t;
                tmp.node_ptr = tmp.block_ptr->get(t);
                return tmp;
            }

            iterator operator-(const int &n) const {
                if (n < 0) return this->operator+(-n);
                iterator tmp;
                if (n > pos) {
                    tmp.invalid = true;
                    return tmp;
                }
                tmp.pos = pos - n;
                tmp.block_ptr = block_ptr;
                tmp.node_ptr = node_ptr;
                tmp.pos_in_block = pos_in_block;
                tmp.pos_in_chain = pos_in_chain;
                tmp.invalid = false;
                int t = n, kk = pos_in_block;
                while (t > kk) {
                    t -= kk;
                    tmp.block_ptr = tmp.block_ptr->pre;
                    if (!tmp.block_ptr) {
                        tmp.invalid = true;
                        return tmp;
                    }
                    tmp.pos_in_block = kk = tmp.block_ptr->num - 1;
                    --tmp.pos_in_chain;
                }
                tmp.pos_in_block = tmp.block_ptr->num - 1 - t;
                tmp.node_ptr = tmp.block_ptr->get(tmp.pos_in_block);
                return tmp;
            }

            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const {
                if (source != rhs.source) throw invalid_iterator();
                return pos - rhs.pos;
            }

            iterator &operator+=(const int &n) {
                return *this = *this + n;
            }

            iterator &operator-=(const int &n) {
                return *this = *this - n;
            }

            /**
             * TODO iter++
             */
            iterator operator++(int) {
                iterator tmp = *this;
                *this = *this + 1;
                return tmp;
            }

            /**
             * TODO ++iter
             */
            iterator &operator++() {
                return *this = *this + 1;
            }

            /**
             * TODO iter--
             */
            iterator operator--(int) {
                iterator tmp = *this;
                *this = *this - 1;
                return tmp;
            }

            /**
             * TODO --iter
             */
            iterator &operator--() {
                return *this = *this - 1;
            }

            /**
             * TODO *it
             * 		throw if iterator is invalid
             */
            T &operator*() const {
                if (invalid) throw invalid_iterator();
                return node_ptr->value;
            }

            /**
             * TODO it->field
             * 		throw if iterator is invalid
             */
            T *operator->() const noexcept {
                if (invalid) throw invalid_iterator();
                return &node_ptr->value;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const {
                return (node_ptr == rhs.node_ptr);
            }

            bool operator==(const const_iterator &rhs) const {
                return (node_ptr == rhs.node_ptr);
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return (node_ptr != rhs.node_ptr);
            }

            bool operator!=(const const_iterator &rhs) const {
                return (node_ptr != rhs.node_ptr);
            }
        };

        class const_iterator {
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
            friend deque<T>;
        private:
            bool invalid;
            Block *block_ptr;
            Node *node_ptr;
            size_t pos_in_block, pos_in_chain, pos;
            deque<T> *source;
        public:
            const_iterator() : invalid(true), block_ptr(nullptr), node_ptr(nullptr), pos_in_block(0), pos_in_chain(0),
                               pos(0) {}

            const_iterator(const const_iterator &other) : invalid(true), block_ptr(other.block_ptr),
                                                          node_ptr(other.node_ptr), pos_in_block(other.pos_in_block),
                                                          pos_in_chain(other.pos_in_chain), pos(other.pos) {
            }

            const_iterator(const iterator &other) : invalid(true), block_ptr(other.block_ptr),
                                                    node_ptr(other.node_ptr), pos_in_block(other.pos_in_block),
                                                    pos_in_chain(other.pos_in_chain), pos(other.pos) {
            }
            // And other methods in iterator.
            // And other methods in iterator.
            // And other methods in iterator.
        public:
            const_iterator operator+(const int &n) const {
                if (n < 0) return this->operator-(-n);
                const_iterator tmp;
                tmp.pos = pos + n;
                if (tmp.pos >= source->Node_num) {
                    tmp.invalid = true;
                    return tmp;
                }
                tmp.block_ptr = block_ptr;
                tmp.node_ptr = node_ptr;
                tmp.pos_in_block = pos_in_block;
                tmp.pos_in_chain = pos_in_chain;
                tmp.invalid = false;
                int t = n;
                while (t >= tmp.block_ptr->num - tmp.pos_in_block) {
                    t -= tmp.block_ptr->num - tmp.pos_in_block;
                    tmp.block_ptr = tmp.block_ptr->next;
                    if (!tmp.block_ptr) {
                        tmp.invalid = true;
                        return tmp;
                    }
                    tmp.pos_in_block = 0;
                    ++tmp.pos_in_chain;
                }
                tmp.pos_in_block = t;
                tmp.node_ptr = tmp.block_ptr->get(t);
                return tmp;
            }

            const_iterator operator-(const int &n) const {
                if (n < 0) return this->operator+(-n);
                const_iterator tmp;
                if (n > pos) {
                    tmp.invalid = true;
                    return tmp;
                }
                tmp.pos = pos - n;
                tmp.block_ptr = block_ptr;
                tmp.node_ptr = node_ptr;
                tmp.pos_in_block = pos_in_block;
                tmp.pos_in_chain = pos_in_chain;
                tmp.invalid = false;
                int t = n, kk = pos_in_block;
                while (t > kk) {
                    t -= kk;
                    tmp.block_ptr = tmp.block_ptr->pre;
                    if (!tmp.block_ptr) {
                        tmp.invalid = true;
                        return tmp;
                    }
                    tmp.pos_in_block = kk = tmp.block_ptr->num - 1;
                    --tmp.pos_in_chain;
                }
                tmp.pos_in_block = tmp.block_ptr->num - 1 - t;
                tmp.node_ptr = tmp.block_ptr->get(tmp.pos_in_block);
                return tmp;
            }

            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
            int operator-(const const_iterator &rhs) const {
                if (source != rhs.source) throw invalid_iterator();
                return pos - rhs.pos;
            }

            const_iterator &operator+=(const int &n) {
                return *this = *this + n;
            }

            const_iterator &operator-=(const int &n) {
                return *this = *this - n;
            }

            /**
             * TODO iter++
             */
            const_iterator operator++(int) {
                const_iterator tmp = *this;
                *this = *this + 1;
                return tmp;
            }

            /**
             * TODO ++iter
             */
            const_iterator &operator++() {
                return *this = *this + 1;
            }

            /**
             * TODO iter--
             */
            const_iterator operator--(int) {
                const_iterator tmp = *this;
                *this = *this - 1;
                return tmp;
            }

            /**
             * TODO --iter
             */
            iterator &operator--() {
                return *this = *this - 1;
            }

            /**
             * TODO *it
             * 		throw if iterator is invalid
             */
            const T &operator*() const {
                if (invalid) throw invalid_iterator();
                return node_ptr->value;
            }

            /**
             * TODO it->field
             * 		throw if iterator is invalid
             */
            const T *operator->() const noexcept {
                if (invalid) throw invalid_iterator();
                return &node_ptr->value;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const {
                return (node_ptr == rhs.node_ptr);
            }

            bool operator==(const const_iterator &rhs) const {
                return (node_ptr == rhs.node_ptr);
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return (node_ptr != rhs.node_ptr);
            }

            bool operator!=(const const_iterator &rhs) const {
                return (node_ptr != rhs.node_ptr);
            }
        };

        /**
         * TODO Constructors
         */
        deque() : head(nullptr), tail(nullptr), Block_num(0), Node_num(0) {}

        deque(const deque &other) : head(nullptr), tail(nullptr), Block_num(other.Block_num), Node_num(other.Node_num) {
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
            tail = _copy(head, other.head);
            return *this;
        }

        /**
         * access specified element with bounds checking
         * throw index_out_of_bound if out of bound.
         */
        T &at(const size_t &pos) {
            if (pos >= Node_num) throw index_out_of_bound();
            size_t t = pos;
            Block *ptr = head;
            while (t >= ptr->num) {
                t -= ptr->num;
                ptr = ptr->next;
            }
            Node *ptrr = ptr->get(t);
            return ptrr->value;
        }

        const T &at(const size_t &pos) const {
            if (pos >= Node_num) throw index_out_of_bound();
            size_t t = pos;
            Block *ptr = head;
            while (t >= ptr->num) {
                t -= ptr->num;
                ptr = ptr->next;
            }
            return ptr->get(t)->value;
        }

        T &operator[](const size_t &pos) {
            return this->at(pos);
        }

        const T &operator[](const size_t &pos) const {
            return this->at(pos);
        }

        /**
         * access the first element
         * throw container_is_empty when the container is empty.
         */
        const T &front() const {
            if (!Node_num) throw container_is_empty();
            return head->head->value;
        }

        /**
         * access the last element
         * throw container_is_empty when the container is empty.
         */
        const T &back() const {
            if (!Node_num) throw container_is_empty();
            return tail->tail->value;
        }

        /**
         * returns an iterator to the beginning.
         */
        iterator begin() {
            iterator tmp;
            if (!Node_num) {
                tmp.invalid = true;
                return tmp;
            }
            tmp.source = this;
            tmp.invalid = false;
            tmp.pos = 0;
            tmp.pos_in_chain = 0;
            tmp.pos_in_block = 0;
            tmp.block_ptr = head;
            tmp.node_ptr = head->head;
        }

        const_iterator cbegin() const {
            const_iterator tmp;
            if (!Node_num) {
                tmp.invalid = true;
                return tmp;
            }
            tmp.source = this;
            tmp.invalid = false;
            tmp.pos = 0;
            tmp.pos_in_chain = 0;
            tmp.pos_in_block = 0;
            tmp.block_ptr = head;
            tmp.node_ptr = head->head;
        }

        /**
         * returns an iterator to the end.
         */
        iterator end() {
            iterator tmp;
            if (!Node_num) {
                tmp.invalid = true;
                return tmp;
            }
            tmp.source = this;
            tmp.invalid = false;
            tmp.pos = Node_num - 1;
            tmp.pos_in_chain = Block_num - 1;
            tmp.pos_in_block = tail->num - 1;
            tmp.block_ptr = tail;
            tmp.node_ptr = tail->tail;
        }

        const_iterator cend() const {
            const_iterator tmp;
            if (!Node_num) {
                tmp.invalid = true;
                return tmp;
            }
            tmp.source = this;
            tmp.invalid = false;
            tmp.pos = Node_num - 1;
            tmp.pos_in_chain = Block_num - 1;
            tmp.pos_in_block = tail->num - 1;
            tmp.block_ptr = tail;
            tmp.node_ptr = tail->tail;
        }

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
                ++Block_num;
                head = tail = new Block(this);
                tail->push_back(value);
            }
            else {
                if (tail->push_back(value)) tail = tail->next;
            }
        }

        /**
         * removes the last element
         *     throw when the container is empty.
         */
        void pop_back() {
            if (!Node_num) throw container_is_empty();
            if (tail->erase(tail->num - 1)) {
                --Block_num;
                Block *ptr = tail;
                tail = tail->pre;
                if (tail)
                    tail->next = nullptr;
                delete ptr;
            }
        }

        /**
         * inserts an element to the beginning.
         */
        void push_front(const T &value) {
            Node *tmp = new Node(value);
            if (!Block_num) {
                ++Block_num;
                head = tail = new Block(this);
                head->add(tmp, 0);
            }
            else {
                head->add(tmp, 0);
            }
        }

        /**
         * removes the first element.
         *     throw when the container is empty.
         */
        void pop_front() {
            if (!Node_num) throw container_is_empty();
            if (head->erase(0)) {
                --Block_num;
                Block *ptr = head;
                head = head->next;
                if (head)
                    head->pre = nullptr;
                delete ptr;
            }
        }
    };

}

#endif
