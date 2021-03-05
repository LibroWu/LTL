#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu {
    const int splitThreshold = 400;
    const int mergeThreshold = 200;
    const int mergeThreshold1 = 10;
    const int splitThreshold1 = 380;

    template<class T>
    class deque {
    private:
        class Node {
        public:
            T value;
            Node *next, *pre;

            Node(const T &e) : next(nullptr), pre(nullptr), value(e) {}
        };

        class Block {
        public:
            size_t num;
            Node *head, *tail;
            Block *next, *pre;

            Block() : num(0), head(nullptr), tail(nullptr), next(nullptr), pre(nullptr) {}

            Block(const Block &other) : num(other.num), head(nullptr), tail(nullptr), next(nullptr), pre(nullptr) {
                if (!num) return;
                if (other.head == nullptr)
                    return;
                head = new Node(other.head->value);
                Node *ptr = head, *ptr_other;
                for (ptr_other = other.head->next; ptr_other; ptr_other = ptr_other->next) {
                    ptr->next = new Node(ptr_other->value);
                    ptr->next->pre = ptr;
                    ptr = ptr->next;
                }
                tail = ptr;
            }

            void Split() {
                Block *tmp = new Block;
                tmp->num = num >> 1;
                num = num >> 1;
                Node *ptr = head;
                for (int i = 1; i < num; ++i) ptr = ptr->next;
                tmp->tail = tail;
                tmp->head = ptr->next;
                tail = ptr;
                tail->next = nullptr;
                tmp->next = next;
                if (next)
                    next->pre = tmp;
                next = tmp;
                tmp->pre = this;
            }

            void clear() {
                num = 0;
                for (Node *i = head, *j; i; i = j) {
                    j = i->next;
                    delete i;
                }
                head = tail = nullptr;
                next = pre = nullptr;
            }

            ~Block() {
                clear();
            }

            void Merge() {
                if (num == 0) {
                    head = next->head;
                    tail = next->tail;
                    next->tail = next->head = nullptr;
                }
                else {
                    if (next->num) {
                        tail->next = next->head;
                        tail = next->tail;
                        next->head = next->tail = nullptr;
                    }
                }
                num += next->num;
                Block *ptr = next;
                next = ptr->next;
                if (next)
                    next->pre = this;
                delete ptr;
            }

            bool insert(const size_t &pos, const T &v) {
                if (pos > num) return false;
                if (!head) {
                    head = tail = new Node(v);
                }
                else {
                    if (!pos) {
                        Node *ptr = new Node(v);
                        ptr->next = head;
                        head->pre = ptr;
                        head = ptr;
                    }
                    else if (pos == num) {
                        tail->next = new Node(v);
                        tail->next->pre = tail;
                        tail = tail->next;
                    }
                    else {
                        Node *ptr = head, *tmp = new Node(v);
                        for (int i = 1; i < pos; ++i) {
                            ptr = ptr->next;
                        }
                        tmp->next = ptr->next;
                        if (tmp->next)
                            tmp->next->pre = tmp;
                        tmp->pre = ptr;
                        ptr->next = tmp;
                    }
                }
                ++num;
                if (num == splitThreshold) {
                    Split();
                    return true;
                }
                return false;
            }

            Node *get(const size_t &pos) const {
                Node *ptr = head;
                for (int i = 0; i < pos; ++i)
                    ptr = ptr->next;
                return ptr;
            }

            bool erase(const size_t &pos) {
                if (pos >= num) return false;
                --num;
                if (!pos) {
                    Node *ptr = head;
                    head = head->next;
                    if (head) {
                        head->pre = nullptr;
                    }
                    else tail = nullptr;
                    delete ptr;
                }
                else if (pos == num) {
                    Node *ptr = tail;
                    tail = tail->pre;
                    tail->next = nullptr;
                    delete ptr;
                }
                else {
                    Node *ptr = head, *tmp;
                    for (size_t i = 1; i < pos; ++i) {
                        ptr = ptr->next;
                    }
                    tmp = ptr->next;
                    ptr->next = tmp->next;
                    if (ptr->next)
                        ptr->next->pre = ptr;
                    delete tmp;
                }
                if (next)
                    if (num + next->num <= mergeThreshold ||
                        num <= mergeThreshold1 && num + next->num <= splitThreshold1) {
                        Merge();
                        return true;
                    }
                return false;
            }
        };

        class ULL {
        public:
            Block *head, *tail;
            size_t num, block_num;

            ULL() : head(nullptr), tail(nullptr), num(0), block_num(0) {}

            ULL(const ULL &other) : head(nullptr), tail(nullptr), num(other.num), block_num(other.block_num) {
                if (!num) return;
                head = new Block(*other.head);
                Block *ptr = head, *ptr_other;
                for (ptr_other = other.head->next; ptr_other; ptr_other = ptr_other->next) {
                    ptr->next = new Block(*ptr_other);
                    ptr->next->pre = ptr;
                    ptr = ptr->next;
                }
                tail = ptr;
            }

            ULL &operator=(const ULL &other) {
                if (this == &other) return *this;
                clear();
                num = other.num;
                block_num = other.block_num;
                if (other.head) {
                    head = new Block(*other.head);
                    Block *ptr = head, *ptr_other;
                    for (ptr_other = other.head->next; ptr_other; ptr_other = ptr_other->next) {
                        ptr->next = new Block(*ptr_other);
                        ptr->next->pre = ptr;
                        ptr = ptr->next;
                    }
                    tail = ptr;
                }
                return *this;
            }

            void clear() {
                block_num = num = 0;
                for (Block *i = head, *j; i; i = j) {
                    j = i->next;
                    delete i;
                }
                head = tail = nullptr;
            }

            void insert(const size_t pos, const T &v) {
                if (pos > num) return;
                ++num;
                if (!head) {
                    head = tail = new Block();
                    head->insert(0, v);
                }
                else {
                    size_t m = pos;
                    Block *ptr = head;
                    while (m > ptr->num) {
                        m -= ptr->num;
                        ptr = ptr->next;
                    }
                    if (ptr->insert(m, v)) {
                        ++block_num;
                        if (ptr == tail)
                            tail = tail->next;
                    }
                }
            }

            void erase(const size_t pos) {
                if (pos >= num) return;
                size_t m = pos;
                Block *ptr = head;
                while (m >= ptr->num) {
                    m -= ptr->num;
                    ptr = ptr->next;
                }
                --num;
                if (ptr->erase(m)) {
                    --block_num;
                    if (ptr->next == tail)
                        tail = ptr;
                }
            }

            Node *get(const size_t &pos) const {
                size_t m = pos;
                Block *ptr = head;
                while (m >= ptr->num) {
                    m -= ptr->num;
                    ptr = ptr->next;
                }
                return ptr->get(m);
            }

            ~ULL() {
                clear();
            }
        } Libro;

    public:
        class const_iterator;

        class iterator {
            friend deque<T>;
        private:
            int pos, posInBlock;
            ULL *source;
            Node *nodePtr;
            Block *blockPtr;
            bool invalid;
        public:
            /**
             * return a new iterator which pointer n-next elements
             *   if there are not enough elements, iterator becomes invalid
             * as well as operator-
             */
            iterator operator+(const int &n) const {
                iterator tmp = *this;
                tmp += n;
                return tmp;
            }

            iterator operator-(const int &n) const {
                iterator tmp = *this;
                tmp -= n;
                return tmp;
            }

            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const {
                if (source != rhs.source) throw invalid_iterator();
                return pos - rhs.pos;
            }

            iterator &operator+=(const int &n) {
                if (n < 0) return *this -= -n;
                if (invalid || pos + n > source->num) {
                    invalid = true;
                    return *this;
                }
                pos += n;
                if (pos == source->num) {
                    blockPtr = source->tail;
                    nodePtr = nullptr;
                    posInBlock = ((blockPtr) ? blockPtr->num : 0);
                    return *this;
                }
                int m = n;

                int t = pos - n;
                Node *tmp;

                while (m >= blockPtr->num - posInBlock) {

                    t += blockPtr->num - posInBlock;

                    m -= blockPtr->num - posInBlock;
                    blockPtr = blockPtr->next;
                    nodePtr = blockPtr->head;
                    posInBlock = 0;
                    tmp = source->get(t);
                    if (nodePtr->value != tmp->value)
                        std::cout << t << std::endl;
                }
                for (int i = 0; i < m; ++i) nodePtr = nodePtr->next;
                posInBlock = m;
                return *this;
            }

            iterator &operator-=(const int &n) {
                if (n < 0) return *this += -n;
                if (invalid || pos - n < 0) {
                    invalid = true;
                    return *this;
                }
                int m = n;
                if (m && pos == source->num) {
                    nodePtr = ((blockPtr) ? blockPtr->tail : nullptr);
                    --posInBlock;
                    --m;
                }
                while (m >= posInBlock + 1) {
                    m -= posInBlock + 1;
                    blockPtr = blockPtr->pre;
                    nodePtr = blockPtr->tail;
                    posInBlock = blockPtr->num - 1;
                }
                pos -= n;
                for (int i = 0; i < m; ++i) nodePtr = nodePtr->pre;
                posInBlock = posInBlock - m;
                return *this;
            }

            /**
             * TODO iter++
             */
            iterator operator++(int) {
                iterator tmp = *this;
                if (invalid || pos == source->num) {
                    invalid = true;
                    return tmp;
                }
                ++pos;
                if (pos == source->num) {
                    ++posInBlock;
                    nodePtr = nullptr;
                    return tmp;
                }
                if (posInBlock + 1 == blockPtr->num) {
                    blockPtr = blockPtr->next;
                    posInBlock = 0;
                    nodePtr = blockPtr->head;
                }
                else {
                    nodePtr = nodePtr->next;
                    ++posInBlock;
                }
                return tmp;
            }

            /**
             * TODO ++iter
             */
            iterator &operator++() {
                if (invalid || pos == source->num) {
                    invalid = true;
                    return *this;
                }
                ++pos;
                if (pos == source->num) {
                    ++posInBlock;
                    nodePtr = nullptr;
                    return *this;
                }
                if (posInBlock + 1 == blockPtr->num && blockPtr->next) {
                    blockPtr = blockPtr->next;
                    posInBlock = 0;
                    nodePtr = blockPtr->head;
                }
                else {
                    nodePtr = nodePtr->next;
                    ++posInBlock;
                }
                return *this;
            }

            /**
             * TODO iter--
             */
            iterator operator--(int) {
                iterator tmp = *this;
                if (invalid || pos == 0) {
                    invalid = true;
                    return tmp;
                }
                --pos;
                if (pos + 1 == source->num) {
                    nodePtr = blockPtr->tail;
                    --posInBlock;
                    return tmp;
                }
                if (posInBlock == 0) {
                    blockPtr = blockPtr->pre;
                    posInBlock = blockPtr->num - 1;
                    nodePtr = blockPtr->tail;
                }
                else {
                    nodePtr = nodePtr->pre;
                    --posInBlock;
                }
                return tmp;
            }

            /**
             * TODO --iter
             */
            iterator &operator--() {
                if (invalid || pos == 0) {
                    invalid = true;
                    return *this;
                }
                --pos;
                if (pos + 1 == source->num) {
                    nodePtr = blockPtr->tail;
                    --posInBlock;
                    return *this;
                }
                if (posInBlock == 0) {
                    blockPtr = blockPtr->pre;
                    posInBlock = blockPtr->num - 1;
                    nodePtr = blockPtr->tail;
                }
                else {
                    nodePtr = nodePtr->pre;
                    --posInBlock;
                }
                return *this;
            }

            /**
             * TODO *it
             * 		throw if iterator is invalid
             */
            T &operator*() const {
                if (invalid || pos < 0 || pos >= source->num) throw invalid_iterator();
                return nodePtr->value;
            }

            /**
             * TODO it->field
             * 		throw if iterator is invalid
             */
            T *operator->() const noexcept {
                if (invalid || pos < 0 || pos >= source->num) throw invalid_iterator();
                return &(nodePtr->value);
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const {
                return (nodePtr == rhs.nodePtr);
            }

            bool operator==(const const_iterator &rhs) const {
                return (nodePtr == rhs.nodePtr);
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return (nodePtr != rhs.nodePtr);
            }

            bool operator!=(const const_iterator &rhs) const {
                return (nodePtr != rhs.nodePtr);
            }
        };

        class const_iterator {
            friend deque<T>;
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
        private:
            // data members.
            int pos, posInBlock;
            const ULL *source;
            Node *nodePtr;
            Block *blockPtr;
            bool invalid;
        public:
            const_iterator() : pos(0), posInBlock(0), nodePtr(nullptr), source(nullptr), blockPtr(nullptr), invalid(0) {
            }

            const_iterator(const const_iterator &other) : pos(other.pos), posInBlock(other.posInBlock),
                                                          source(other.source), nodePtr(other.nodePtr),
                                                          blockPtr(other.blockPtr), invalid(other.invalid) {}

            const_iterator(const iterator &other) : pos(other.pos), posInBlock(other.posInBlock),
                                                    source(other.source), nodePtr(other.nodePtr),
                                                    blockPtr(other.blockPtr), invalid(other.invalid) {}

        public:
            /**
             * return a new iterator which pointer n-next elements
             *   if there are not enough elements, iterator becomes invalid
             * as well as operator-
             */
            const_iterator operator+(const int &n) const {
                const_iterator tmp = *this;
                tmp += n;
                return tmp;
            }

            const_iterator operator-(const int &n) const {
                const_iterator tmp = *this;
                tmp -= n;
                return tmp;
            }

            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
            int operator-(const const_iterator &rhs) const {
                if (source != rhs.source) throw invalid_iterator();
                return pos - rhs.pos;
            }

            const_iterator &operator+=(const int &n) {
                if (n < 0) return *this -= -n;
                if (invalid || pos + n > source->num) {
                    invalid = true;
                    return *this;
                }
                pos += n;
                if (pos == source->num) {
                    nodePtr = nullptr;
                    blockPtr = source->tail;
                    posInBlock = ((blockPtr) ? blockPtr->num : 0);
                    return *this;
                }
                int m = n;
                while (m >= blockPtr->num - posInBlock) {
                    m -= blockPtr->num - posInBlock;
                    blockPtr = blockPtr->next;
                    nodePtr = blockPtr->head;
                    posInBlock = 0;
                }
                for (int i = 0; i < m; ++i) nodePtr = nodePtr->next;
                posInBlock = m;
                return *this;
            }

            const_iterator &operator-=(const int &n) {
                if (n < 0) return *this += -n;
                if (invalid || pos - n < 0) {
                    invalid = true;
                    return *this;
                }
                int m = n;
                if (m && pos == source->num) {
                    nodePtr = ((blockPtr) ? blockPtr->tail : nullptr);
                    --posInBlock;
                    --m;
                }
                pos -= n;
                while (m >= posInBlock + 1) {
                    m -= posInBlock + 1;
                    blockPtr = blockPtr->pre;
                    nodePtr = blockPtr->tail;
                    posInBlock = blockPtr->num - 1;
                }
                for (int i = 0; i < m; ++i) nodePtr = nodePtr->pre;
                posInBlock = posInBlock - m;
                return *this;
            }

            /**
             * TODO iter++
             */
            const_iterator operator++(int) {
                const_iterator tmp = *this;
                if (invalid || pos == source->num) {
                    invalid = true;
                    return tmp;
                }
                ++pos;
                if (pos == source->num) {
                    ++posInBlock;
                    nodePtr = nullptr;
                    return tmp;
                }
                if (posInBlock + 1 == blockPtr->num) {
                    blockPtr = blockPtr->next;
                    posInBlock = 0;
                    nodePtr = blockPtr->head;
                }
                else {
                    nodePtr = nodePtr->next;
                    ++posInBlock;
                }
                return tmp;
            }

            /**
             * TODO ++iter
             */
            const_iterator &operator++() {
                if (invalid || pos == source->num) {
                    invalid = true;
                    return *this;
                }
                ++pos;
                if (pos == source->num) {
                    ++posInBlock;
                    nodePtr = nullptr;
                    return *this;
                }
                if (posInBlock + 1 == blockPtr->num) {
                    blockPtr = blockPtr->next;
                    posInBlock = 0;
                    nodePtr = blockPtr->head;
                }
                else {
                    nodePtr = nodePtr->next;
                    ++posInBlock;
                }
                return *this;
            }

            /**
             * TODO iter--
             */
            const_iterator operator--(int) {
                const_iterator tmp = *this;
                if (invalid || pos == 0) {
                    invalid = true;
                    return tmp;
                }
                --pos;
                if (pos + 1 == source->num) {
                    nodePtr = blockPtr->tail;
                    --posInBlock;
                    return tmp;
                }
                if (posInBlock == 0) {
                    blockPtr = blockPtr->pre;
                    posInBlock = blockPtr->num - 1;
                    nodePtr = blockPtr->tail;
                }
                else {
                    nodePtr = nodePtr->pre;
                    --posInBlock;
                }
                return tmp;
            }

            /**
             * TODO --iter
             */
            const_iterator &operator--() {
                if (invalid || pos == 0) {
                    invalid = true;
                    return *this;
                }
                --pos;
                if (pos + 1 == source->num) {
                    nodePtr = blockPtr->tail;
                    --posInBlock;
                    return *this;
                }
                if (posInBlock == 0) {
                    blockPtr = blockPtr->pre;
                    posInBlock = blockPtr->num - 1;
                    nodePtr = blockPtr->tail;
                }
                else {
                    nodePtr = nodePtr->pre;
                    --posInBlock;
                }
                return *this;
            }

            /**
             * TODO *it
             * 		throw if iterator is invalid
             */
            const T &operator*() const {
                if (invalid || pos < 0 || pos >= source->num) throw invalid_iterator();
                return nodePtr->value;
            }

            /**
             * TODO it->field
             * 		throw if iterator is invalid
             */
            const T *operator->() const noexcept {
                if (invalid || pos < 0 || pos >= source->num) throw invalid_iterator();
                return &(nodePtr->value);
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const {
                return (nodePtr == rhs.nodePtr);
            }

            bool operator==(const const_iterator &rhs) const {
                return (nodePtr == rhs.nodePtr);
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return (nodePtr != rhs.nodePtr);
            }

            bool operator!=(const const_iterator &rhs) const {
                return (nodePtr != rhs.nodePtr);
            }
        };

        /**
         * TODO Constructors
         */
        deque() {}

        deque(const deque &other) {
            Libro = other.Libro;
        }

        /**
         * TODO Deconstructor
         */
        ~deque() {}

        /**
         * TODO assignment operator
         */
        deque &operator=(const deque &other) {
            Libro = other.Libro;
            return *this;
        }

        /**
         * access specified element with bounds checking
         * throw index_out_of_bound if out of bound.
         */
        T &at(const size_t &pos) {
            if (pos >= Libro.num) throw index_out_of_bound();
            return Libro.get(pos)->value;
        }

        const T &at(const size_t &pos) const {
            if (pos >= Libro.num) throw index_out_of_bound();
            return Libro.get(pos)->value;
        }

        T &operator[](const size_t &pos) {
            if (pos >= Libro.num) throw index_out_of_bound();
            return Libro.get(pos)->value;
        }

        const T &operator[](const size_t &pos) const {
            if (pos >= Libro.num) throw index_out_of_bound();
            return Libro.get(pos)->value;
        }

        /**
         * access the first element
         * throw container_is_empty when the container is empty.
         */
        const T &front() const {
            if (Libro.num == 0) throw container_is_empty();
            return Libro.get(0)->value;
        }

        /**
         * access the last element
         * throw container_is_empty when the container is empty.
         */
        const T &back() const {
            if (Libro.num == 0) throw container_is_empty();
            return Libro.get(Libro.num - 1)->value;
        }

        /**
         * returns an iterator to the beginning.
         */
        iterator begin() {
            iterator tmp;
            tmp.source = &Libro;
            tmp.pos = 0;
            tmp.posInBlock = 0;
            tmp.invalid = 0;
            tmp.blockPtr = Libro.head;
            tmp.nodePtr = ((tmp.blockPtr) ? tmp.blockPtr->head : nullptr);
            return tmp;
        }

        const_iterator cbegin() const {
            const_iterator tmp;
            tmp.source = &Libro;
            tmp.pos = 0;
            tmp.posInBlock = 0;
            tmp.invalid = 0;
            tmp.blockPtr = Libro.head;
            tmp.nodePtr = ((tmp.blockPtr) ? tmp.blockPtr->head : nullptr);
            return tmp;
        }

        /**
         * returns an iterator to the end.
         */
        iterator end() {
            iterator tmp;
            tmp.source = &Libro;
            tmp.pos = Libro.num;
            tmp.invalid = 0;
            tmp.blockPtr = Libro.tail;
            tmp.posInBlock = ((Libro.tail) ? Libro.tail->num : 0);
            tmp.nodePtr = nullptr;
            return tmp;
        }

        const_iterator cend() const {
            const_iterator tmp;
            tmp.source = &Libro;
            tmp.pos = Libro.num;
            tmp.invalid = 0;
            tmp.blockPtr = Libro.tail;
            tmp.posInBlock = ((Libro.tail) ? Libro.tail->num : 0);
            tmp.nodePtr = nullptr;
            return tmp;
        }

        /**
         * checks whether the container is empty.
         */
        bool empty() const {
            return (Libro.num == 0);
        }

        /**
         * returns the number of elements
         */
        size_t size() const {
            return Libro.num;
        }

        /**
         * clears the contents
         */
        void clear() {
            Libro.clear();
        }

        /**
         * inserts elements at the specified locat on in the container.
         * inserts value before pos
         * returns an iterator pointing to the inserted value
         *     throw if the iterator is invalid or it point to a wrong place.
         */
        iterator insert(iterator pos, const T &value) {
            if (pos.pos < 0 || pos.pos > pos.source->num) throw invalid_iterator();
            Libro.insert(pos.pos, value);
            return pos;
        }

        /**
         * removes specified element at pos.
         * removes the element at pos.
         * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
         * throw if the container is empty, the iterator is invalid or it points to a wrong place.
         */
        iterator erase(iterator pos) {
            if (pos.source->num == 0 || pos.pos < 0 || pos.pos >= pos.source->num) throw invalid_iterator();
            pos++;
            Libro.erase(pos.pos - 1);
            return pos;
        }

        /**
         * adds an element to the end
         */
        void push_back(const T &value) {
            Libro.insert(Libro.num, value);
        }

        /**
         * removes the last element
         *     throw when the container is empty.
         */
        void pop_back() {
            Libro.erase(Libro.num - 1);
        }

        /**
         * inserts an element to the beginning.
         */
        void push_front(const T &value) {
            Libro.insert(0, value);
        }

        /**
         * removes the first element.
         *     throw when the container is empty.
         */
        void pop_front() {
            Libro.erase(0);
        }
    };

}

#endif
