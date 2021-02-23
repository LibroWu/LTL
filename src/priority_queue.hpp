#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

/**
 * a container like std::priority_queue which is a heap internal.
 */
    template<typename T, class Compare = std::less<T>>
    class priority_queue {
    private:

        Compare cmp;

        class Node {
        public:
            size_t num;
            T value;
            Node *child, *tail, *next;

            explicit Node(T e = T(), size_t n = 1) : value(e), next(nullptr), num(n), child(nullptr), tail(nullptr) {}
        };

        Node *root;

        size_t _size;

        Node *_copy(Node *&ptr, Node *another) {
            if (!another) {
                ptr = nullptr;
                return nullptr;
            }
            ptr = new Node(another->value, another->num);
            Node *tmp = _copy(ptr->next, another->next);
            ptr->tail = _copy(ptr->child, another->child);
            return ((!tmp) ? ptr : tmp);
        }

        void _merge(Node *&ptr) {
            while (ptr->next && ptr->num == ptr->next->num) {
                //add ptr into ptr->next
                if (cmp(ptr->value, ptr->next->value)) {
                    if (!ptr->next->child) {
                        ptr->next->child = ptr->next->tail = ptr;
                    }
                    else {
                        ptr->next->tail->next = ptr;
                        ptr->next->tail = ptr;
                    }
                    ptr = ptr->next;
                    ptr->tail->next = nullptr;
                }
                    //add ptr->next into ptr
                else {
                    if (!ptr->child) {
                        ptr->child = ptr->tail = ptr->next;
                    }
                    else {
                        ptr->tail->next = ptr->next;
                        ptr->tail = ptr->next;
                    }
                    ptr->next = ptr->next->next;
                    ptr->tail->next = nullptr;
                }
                ptr->num <<= 1;
            }
        }

        void clear() {
            for (Node *i = root, *j; i; i = j) {
                j = i->next;
                Node_Free(i);
            }
            root = nullptr;
        }

        void Node_Free(Node *ptr) {
            for (Node *i = ptr->child, *j; i; i = j) {
                j = i->next;
                Node_Free(i);
            }
            delete ptr;
        }

        void _insert(Node *ptr) {
            if (!root) {
                root = ptr;
                return;
            }
            for (Node *i = ptr, *j; i; i = j) {
                Node *cur;
                j = i->next;
                if (root->num > i->num) {
                    i->next = root;
                    root = i;
                    continue;
                }

                for (cur = root; cur->next; cur = cur->next) {
                    if (cur->next->num >= i->num) break;
                }
                i->next = cur->next;
                cur->next = i;
                _merge(cur->next);
            }
        }

    public:
        /**
         * TODO constructors
         */
        priority_queue() : root(nullptr), _size(0) {}

        priority_queue(const priority_queue &other) : _size(other._size) {
            _copy(root, other.root);
        }

        /**
         * TODO deconstructor
         */
        ~priority_queue() {
            for (Node *i = root, *j; i; i = j) {
                j = i->next;
                Node_Free(i);
            }
        }

        /**
         * TODO Assignment operator
         */
        priority_queue &operator=(const priority_queue &other) {
            if (this == &other) return *this;
            clear();
            _copy(root, other.root);
            _size=other._size;
            return *this;
        }

        /**
         * get the top of the queue.
         * @return a reference of the top element.
         * throw container_is_empty if empty() returns true;
         */
        const T &top() const {
            if (!root) {
                throw container_is_empty();
            }
            Node *res = root;
            for (Node *i = root->next; i; i = i->next) {
                if (cmp(res->value, i->value))
                    res = i;
            }
            return res->value;
        }

        /**
         * TODO
         * push new element to the priority queue.
         */
        void push(const T &e) {
            ++_size;
            if (!root) {
                root = new Node(e, 1);
            }
            else {
                Node *tmp = new Node(e);
                tmp->next = root;
                root = tmp;
                _merge(root);
            }
        }

        /**
         * TODO
         * delete the top element.
         * throw container_is_empty if empty() returns true;
         */
        void pop() {
            if (!root) {
                throw container_is_empty();
            }
            --_size;
            Node *pre_t = nullptr;
            Node *now_t = root;
            T res = root->value;
            for (Node *now = root->next, *pre = root; now; pre = now, now = now->next) {
                if (cmp(res, now->value)) {
                    res = now->value;
                    pre_t = pre;
                    now_t = now;
                }
            }
            if (!pre_t) {
                root = root->next;
            }
            else {
                pre_t->next = now_t->next;
            }
            _insert(now_t->child);
            delete now_t;
        }

        /**
         * return the number of the elements.
         */
        size_t size() const {
            return _size;
        }

        /**
         * check if the container has at least an element.
         * @return true if it is empty, false if it has at least an element.
         */
        bool empty() const {
            return (_size == 0);
        }

        /**
         * return a merged priority_queue with at least O(logn) complexity.
         */
        void merge(priority_queue &other) {
            _insert(other.root);
            other.root = nullptr;
            other._size=0;
        }
    };

}

#endif
