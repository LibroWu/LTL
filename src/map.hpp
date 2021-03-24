/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

    template<
            class Key,
            class T,
            class Compare = std::less<Key>
    >
    class map {
    public:
        enum COLOR {
            red, black
        };

        class RedBlackNode {
        public:
            COLOR nodeColor;
            RedBlackNode *next, *pre, *sibling, *parent, *lch, *rch;
            T value;
            Key key;

            RedBlackNode(const Key &k, const T &v, COLOR col = red) : key(k), value(v), sibling(nullptr),
                                                                      parent(nullptr), lch(nullptr),
                                                                      rch(nullptr), next(nullptr),
                                                                      pre(nullptr), nodeColor(col) {}

            ~RedBlackNode() {
                if (next)
                    next->pre = pre;
                if (pre)
                    pre->next = next;
            }

        };

        class RBT {
        public:
            RedBlackNode *head, *Beg, *End;

            //false for failing to insert because same key has existed
            typedef pair<RedBlackNode *, bool> pointer;

            RBT() : head(nullptr), Beg(nullptr), End(nullptr) {}

            ~RBT() {
                if (head)
                    makeEmpty(head);
            }

            void makeEmpty(RedBlackNode *ptr) {
                if (ptr->rch)
                    makeEmpty(ptr->rch);
                if (ptr->lch)
                    makeEmpty(ptr->lch);
                delete ptr;
            }

            void Del(RedBlackNode *ptr) {
                if (ptr == Beg) Beg = ptr->next;
                if (ptr == End) End = ptr->pre;
                delete ptr;
            }

            void singleRotate(RedBlackNode *ptr) {
                RedBlackNode *P = ptr->parent;
                if (P == nullptr) return;
                bool flag_ptr = isLeftChild(ptr);
                if (P == head) {
                    head = ptr;
                    ptr->parent = nullptr;
                }
                else {
                    if (isLeftChild(P)) P->parent->lch = ptr;
                    else P->parent->rch = ptr;
                    ptr->parent = P->parent;
                }
                P->parent = ptr;
                if (flag_ptr) {
                    if (ptr->rch) ptr->rch->parent = P;
                    P->lch = ptr->rch;
                    ptr->rch = P;
                }
                else {
                    if (ptr->lch) ptr->lch->parent = P;
                    P->rch = ptr->lch;
                    ptr->lch = P;
                }
            }

            void rotate(RedBlackNode *ptr, RedBlackNode *P, RedBlackNode *G, int dye_pattern = 0) {
                //LL
                if (P->lch == ptr && G->lch == P) {
                    //G is not root
                    if (G->parent) {
                        if (G->parent->lch == G) G->parent->lch = P;
                        else G->parent->rch = P;
                        P->parent = G->parent;
                    }//G is root
                    else {
                        head = P;
                        P->parent = nullptr;
                    }
                    G->parent = P;
                    G->lch = P->rch;
                    if (P->rch) P->rch->parent = G;
                    P->rch = G;
                    if (dye_pattern == 0) {
                        P->nodeColor = black;
                        G->nodeColor = red;
                    }
                    else if (dye_pattern == 1) {
                        ptr->nodeColor = black;
                        G->nodeColor = black;
                        P->nodeColor = red;
                    }
                }
                //RR
                if (P->rch == ptr && G->rch == P) {
                    //G is not root
                    if (G->parent) {
                        if (G->parent->rch == G) G->parent->rch = P;
                        else G->parent->lch = P;
                        P->parent = G->parent;
                    }//G is root
                    else {
                        head = P;
                        P->parent = nullptr;
                    }
                    G->parent = P;
                    G->rch = P->lch;
                    if (P->lch) P->lch->parent = G;
                    P->lch = G;
                    if (dye_pattern == 0) {
                        P->nodeColor = black;
                        G->nodeColor = red;
                    }
                    else if (dye_pattern == 1) {
                        ptr->nodeColor = black;
                        G->nodeColor = black;
                        P->nodeColor = red;
                    }
                }
                //LR
                if (P->rch == ptr && G->lch == P) {
                    //G is not root
                    if (G->parent) {
                        if (G->parent->lch == G) G->parent->lch = ptr;
                        else G->parent->rch = ptr;
                        ptr->parent = G->parent;
                    }//G is root
                    else {
                        head = ptr;
                        ptr->parent = nullptr;
                    }
                    P->rch = ptr->lch;
                    if (ptr->lch) ptr->lch->parent = P;
                    ptr->lch = P;
                    P->parent = ptr;
                    G->lch = ptr->rch;
                    if (ptr->rch) ptr->rch->parent = G;
                    ptr->rch = G;
                    G->parent = ptr;
                    if (dye_pattern == 0) {
                        ptr->nodeColor = black;
                        G->nodeColor = red;
                    }
                    else if (dye_pattern == 1) {
                        ptr->nodeColor = red;
                        G->nodeColor = black;
                        P->nodeColor = black;
                    }
                }
                //RL
                if (P->lch == ptr && G->rch == P) {
                    //G is not root
                    if (G->parent) {
                        if (G->parent->lch == G) G->parent->lch = ptr;
                        else G->parent->rch = ptr;
                        ptr->parent = G->parent;
                    }//G is root
                    else {
                        head = ptr;
                        ptr->parent = nullptr;
                    }
                    P->lch = ptr->rch;
                    if (ptr->rch) ptr->rch->parent = P;
                    ptr->rch = P;
                    P->parent = ptr;
                    G->rch = ptr->lch;
                    if (ptr->lch) ptr->lch->parent = G;
                    ptr->lch = G;
                    G->parent = ptr;
                    if (dye_pattern == 0) {
                        ptr->nodeColor = black;
                        G->nodeColor = red;
                    }
                    else if (dye_pattern == 1) {
                        ptr->nodeColor = red;
                        G->nodeColor = black;
                        P->nodeColor = black;
                    }
                }
            }

            pointer insert(const Key &key, const T &value) {
                Compare cmp;
                if (!head) {
                    head = new RedBlackNode(key, value, black);
                    Beg = End = head;
                    return pointer(head, true);
                }
                RedBlackNode *ptr = head, *child, *P, *G, *pre, *next;
                pre = next = nullptr;
                while (1) {
                    //have existed?
                    if (ptr->key == key) return pointer(ptr, false);
                    //avoid red uncle node
                    if (ptr->rch && ptr->lch)
                        if (ptr->rch->nodeColor == red && ptr->lch->nodeColor == red) {
                            ptr->rch->nodeColor = ptr->lch->nodeColor = black;
                            P = ptr->parent;
                            if (P) {
                                G = P->parent;
                                ptr->nodeColor = red;
                                if (P->nodeColor == red) {
                                    //rotate
                                    rotate(ptr, P, G);
                                }
                            }
                        }
                    //insert into left tree
                    if (cmp(key, ptr->key)) {
                        next = ptr;
                        if (ptr->lch) {
                            ptr = ptr->lch;
                        }//insert
                        else {
                            ptr->lch = new RedBlackNode(key, value);
                            child = ptr->lch;
                            child->pre = pre;
                            child->next = next;
                            if (pre) pre->next = child;
                            else Beg = child;
                            if (next) next->pre = child;
                            else End = child;
                            child->parent = ptr;
                            if (ptr->nodeColor == red) {
                                rotate(child, ptr, ptr->parent);
                            }
                            return pointer(child, true);
                        }
                    }//insert into right tree
                    else {
                        pre = ptr;
                        if (ptr->rch) {
                            ptr = ptr->rch;
                        }//insert
                        else {
                            ptr->rch = new RedBlackNode(key, value);
                            child = ptr->rch;
                            child->pre = pre;
                            child->next = next;
                            if (pre) pre->next = child;
                            else Beg = child;
                            if (next) next->pre = child;
                            else End = child;
                            child->parent = ptr;
                            if (ptr->nodeColor == red) {
                                rotate(child, ptr, ptr->parent);
                            }
                            return pointer(child, true);
                        }
                    }
                }
            }

            void SwapTwoRBNode(RedBlackNode *a, RedBlackNode *b) {
                RedBlackNode *AP, *AL, *AR, *BP, *BR, *BL;
                COLOR tmp = a->nodeColor;
                a->nodeColor = b->nodeColor;
                b->nodeColor = tmp;
                bool A_lch, B_lch;
                AP = a->parent, AL = a->lch, AR = a->rch;
                BP = b->parent, BL = b->lch, BR = b->rch;
                if (AP) A_lch = isLeftChild(a);
                if (BP) B_lch = isLeftChild(b);
                if (AP == b) {
                    if (BP) {
                        if (B_lch) BP->lch = a;
                        else BP->rch = a;
                    }
                    else head = a;
                    a->parent = BP;
                    b->parent = a;
                    if (A_lch) {
                        a->lch = b;
                        a->rch = BR;
                        if (BR) BR->parent = a;
                    }
                    else {
                        a->rch = b;
                        a->lch = AL;
                        if (BL) BL->parent = a;
                    }
                    if (AL) AL->parent = b;
                    b->lch = AL;
                    if (AR) AR->parent = b;
                    b->rch = AR;
                }
                else if (BP == a) {
                    if (AP) {
                        if (A_lch) AP->lch = b;
                        else AP->rch = b;
                    }
                    else head = b;
                    b->parent = AP;
                    a->parent = b;
                    if (B_lch) {
                        b->lch = a;
                        b->rch = AR;
                        if (AR) AR->parent = b;
                    }
                    else {
                        b->rch = a;
                        b->lch = AL;
                        if (AL) AL->parent = b;
                    }
                    if (BL) BL->parent = a;
                    a->lch = BL;
                    if (BR) BR->parent = a;
                    a->rch = BR;
                }
                else {
                    if (AP) {
                        if (A_lch) AP->lch = b;
                        else AP->rch = b;
                    }
                    else head = b;
                    a->parent = BP;
                    if (BP) {
                        if (B_lch) BP->lch = a;
                        else BP->rch = a;
                    }
                    else head = a;
                    b->parent = AP;
                    if (AL) AL->parent = b;
                    a->lch = BL;
                    if (AR) AR->parent = b;
                    a->rch = BR;
                    if (BL) BL->parent = a;
                    b->lch = AL;
                    if (BR) BR->parent = a;
                    b->rch = AR;
                }
            }

            bool isLeftChild(RedBlackNode *ptr) {
                return (ptr->parent->lch == ptr);
            }

            RedBlackNode *getSibling(RedBlackNode *ptr) {
                if (ptr->parent) {
                    if (isLeftChild(ptr)) return ptr->parent->rch;
                    else return ptr->parent->lch;
                }
                return nullptr;
            }

            RedBlackNode *getSiblingsRedChild(RedBlackNode *ptr) {
                RedBlackNode *tmp = getSibling(ptr);
                if (tmp) {
                    if (isLeftChild(ptr)) {
                        if (tmp->rch && tmp->rch->nodeColor == red) return tmp->rch;
                        if (tmp->lch && tmp->lch->nodeColor == red) return tmp->rch;
                    }
                    else {
                        if (tmp->lch && tmp->lch->nodeColor == red) return tmp->rch;
                        if (tmp->rch && tmp->rch->nodeColor == red) return tmp->rch;
                    }
                }
                return nullptr;
            }

            void Delete(const Key &key) {
                Compare cmp;
                RedBlackNode *ptr = head, *child, *P, *G, *R, *Sib;
                while (1) {
                    //make current node red
                    if (ptr == nullptr) break;
                    if (ptr->nodeColor == black) {
                        if ((ptr->lch == nullptr || ptr->lch->nodeColor == black) &&
                            (ptr->rch == nullptr || ptr->rch->nodeColor == black)) {
                            R = getSiblingsRedChild(ptr);
                            Sib = getSibling(ptr);
                            if (R == nullptr) {
                                if (ptr->parent) ptr->parent->nodeColor = black;
                                ptr->nodeColor = red;
                                if (Sib) Sib->nodeColor = red;
                            }
                            else {
                                rotate(R, Sib, ptr->parent, 1);
                                ptr->nodeColor = red;
                            }
                        }
                        else {
                            if (!cmp(key,ptr->key) && !cmp(ptr->key,key)) {
                                if (ptr->rch == nullptr || ptr->lch == nullptr) {
                                    if (ptr->rch != nullptr) child = ptr->rch;
                                    else child = ptr->lch;
                                    singleRotate(child);
                                    child->nodeColor = black;
                                    ptr->nodeColor = red;
                                }
                                else {
                                    child = ptr->rch, P = ptr, Sib = ptr->lch;
                                    SwapTwoRBNode(ptr, ptr->next);
                                    if (child->nodeColor == black) {
                                        singleRotate(Sib);
                                        Sib->nodeColor = black;
                                        P->nodeColor = red;
                                    }
                                    ptr = child;
                                }
                                continue;
                            }
                            else if (cmp(key, ptr->key)) {
                                P = ptr;
                                Sib = P->rch;
                                ptr = ptr->lch;
                                if (ptr && ptr->nodeColor == black) {
                                    singleRotate(Sib);
                                    P->nodeColor = red;
                                    Sib->nodeColor = black;
                                }
                                continue;
                            }
                            else  {
                                P = ptr;
                                Sib = P->lch;
                                ptr = ptr->rch;
                                if (ptr && ptr->nodeColor == black) {
                                    singleRotate(Sib);
                                    P->nodeColor = red;
                                    Sib->nodeColor = black;
                                }
                                continue;
                            }
                        }
                    }
                    //delete the node
                    if (!cmp(key,ptr->key) && ! cmp(ptr->key,key)) {
                        //leaf or has only one child
                        if (ptr->rch == nullptr) {
                            //has the left child
                            if (ptr->lch != nullptr) {
                                if (ptr->parent) {
                                    if (isLeftChild(ptr)) ptr->parent->lch = ptr->lch;
                                    else ptr->parent->rch = ptr->lch;
                                    ptr->lch->parent = ptr->parent;
                                }
                                else {
                                    head = ptr->lch;
                                    ptr->lch->parent = nullptr;
                                }
                            }//does not have a child
                            else {
                                if (ptr->parent) {
                                    if (isLeftChild(ptr)) ptr->parent->lch = nullptr;
                                    else ptr->parent->rch = nullptr;
                                }
                                else head = nullptr;
                            }
                            Del(ptr);
                            break;
                        }
                        else {
                            //has the right child
                            if (ptr->lch == nullptr) {
                                if (ptr->parent) {
                                    if (isLeftChild(ptr)) ptr->parent->lch = ptr->rch;
                                    else ptr->parent->rch = ptr->rch;
                                    ptr->rch->parent = ptr->parent;
                                }
                                else {
                                    head = ptr->rch;
                                    ptr->rch->parent = nullptr;
                                }
                                Del(ptr);
                                break;
                            }//has the right and left child
                            else {
                                child = ptr->rch;
                                SwapTwoRBNode(ptr, ptr->next);
                                ptr = child;
                            }
                        }
                    }
                    else ptr=cmp(key,ptr->key)?ptr->lch:ptr->rch;
                }
                //adjust the root color
                if (head && head->nodeColor == red)
                    head->nodeColor = black;
            }

            //false for not found
            pointer get(const Key &key) {
                Compare cmp;
                RedBlackNode *ptr = head;
                while (ptr) {
                    if (ptr->key == key) return pointer(ptr, true);
                    if (cmp(key, ptr->key)) ptr = ptr->lch;
                    else ptr = ptr->rch;
                }
                return pointer(nullptr, false);
            }

#define debugs
#ifdef debugs
            int BLACK_NUM, max_step;
            bool flag;

            void Dfs_check(RedBlackNode *node, int step, int black_num) {
                if (node->lch == nullptr && node->rch == nullptr) {
                    if (max_step < step) max_step = step;
                    if (BLACK_NUM == 0) BLACK_NUM = black_num;
                    else if (BLACK_NUM != black_num) flag = true;
                    return;
                }
                if (node->lch) Dfs_check(node->lch, step + 1, black_num + node->lch->nodeColor);
                if (node->rch) Dfs_check(node->rch, step + 1, black_num + node->rch->nodeColor);
            }

            void show() {
                int l = 0, r = 0, step[10000];
                RedBlackNode *que[10000];
                BLACK_NUM = max_step = 0;
                flag = 0;
                Dfs_check(head, 0, 1);
                std::cout << flag << '\n';
                step[0] = 0;
                que[r++] = head;
                while (l < r) {
                    if (step[l] > max_step) break;
                    if (l && step[l - 1] < step[l]) std::cout << '\n';
                    if (que[l] == nullptr) std::cout << "\\n 1 ";
                    else std::cout << que[l]->key << ' ' << que[l]->nodeColor << ' ';
                    if (que[l]) {
                        step[r] = step[l] + 1;
                        que[r++] = que[l]->lch;
                        step[r] = step[l] + 1;
                        que[r++] = que[l]->rch;
                    }
                    else {
                        step[r] = step[l] + 1;
                        que[r++] = nullptr;
                        step[r] = step[l] + 1;
                        que[r++] = nullptr;
                    }
                    ++l;
                }
                std::cout << "\n------\n";
                for (RedBlackNode *ptr = Beg; ptr; ptr = ptr->next) {
                    std::cout << ptr->key << " " << ptr->value << "**\n";
                }
                std::cout << "------\n";
            }

#endif
#undef debugs


        } Nebula;


    public:
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use sjtu::map as value_type by typedef.
         */
        typedef pair<const Key, T> value_type;

        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = map.begin(); --it;
         *       or it = map.end(); ++end();
         */
        class const_iterator;

        class iterator {
        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
        public:
            iterator() {
                // TODO
            }

            iterator(const iterator &other) {
                // TODO
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
             * an operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type &operator*() const {}

            bool operator==(const iterator &rhs) const {}

            bool operator==(const const_iterator &rhs) const {}

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {}

            bool operator!=(const const_iterator &rhs) const {}

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            value_type *operator->() const noexcept {}
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
         * TODO two constructors
         */
        map() {}

        map(const map &other) {}

        /**
         * TODO assignment operator
         */
        map &operator=(const map &other) {}

        /**
         * TODO Destructors
         */
        ~map() {}

        /**
         * TODO
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type `index_out_of_bound'
         */
        T &at(const Key &key) {}

        const T &at(const Key &key) const {}

        /**
         * TODO
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T &operator[](const Key &key) {}

        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T &operator[](const Key &key) const {}

        /**
         * return a iterator to the beginning
         */
        iterator begin() {}

        const_iterator cbegin() const {}

        /**
         * return a iterator to the end
         * in fact, it returns past-the-end.
         */
        iterator end() {}

        const_iterator cend() const {}

        /**
         * checks whether the container is empty
         * return true if empty, otherwise false.
         */
        bool empty() const {}

        /**
         * returns the number of elements.
         */
        size_t size() const {}

        /**
         * clears the contents
         */
        void clear() {}

        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        pair<iterator, bool> insert(const value_type &value) {}

        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        void erase(iterator pos) {}

        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         * The default method of check the equivalence is !(a < b || b > a)
         */
        size_t count(const Key &key) const {}

        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find(const Key &key) {}

        const_iterator find(const Key &key) const {}
    };

}

#endif
