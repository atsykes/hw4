#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{
 
}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n);
    void insertFixLeft(AVLNode<Key, Value>* n, AVLNode<Key, Value>* p, AVLNode<Key, Value>* g); // Updates balances when p is a left node of its parent
    void insertFixRight(AVLNode<Key, Value>* n, AVLNode<Key, Value>* p, AVLNode<Key, Value>* g); // Updates balances when p is a right node of its parent
    bool zigzig(AVLNode<Key, Value>* n); // Checks if there is a zigzig case at the given node using its balances
    bool zigzag(AVLNode<Key, Value>* n); // Checks if there is a zigzag case at the given node using its balances
    void rotateRight(AVLNode<Key, Value>* p);
    void rotateLeft(AVLNode<Key, Value>* p);
    void removeFix(AVLNode<Key, Value>* n, int diff);



};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    if (this->empty()) 
    {
        AVLNode<Key, Value>* current = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        current->setBalance(0);
        this->root_ = current;
        return;
    }
    AVLNode<Key, Value>* current = static_cast<AVLNode<Key,Value>*>(this->root_);
    AVLNode<Key, Value>* parent;

    // Similar to bst insert, finding an empty node in the correct spot
    while(current)
    {
        if (new_item.first < current->getKey()) 
        {
            parent = current;
            current = current->getLeft();
        }
        else if (new_item.first > current->getKey()) 
        {
            parent = current;
            current = current->getRight();
        }
        else 
        {
            current->setValue(new_item.second);
            return;
        }
    }

    AVLNode<Key, Value>* node = new AVLNode<Key, Value>(new_item.first, new_item.second, parent); 
    if (node->getKey() < parent->getKey()) parent->setLeft(node);
    else parent->setRight(node);

    // Setting Balances
    node->setBalance(0);
    if (node->getParent()->getBalance() == 0)
    {
        if (node->getParent()->getLeft() == node) node->getParent()->setBalance(-1);
        else if (node->getParent()->getRight() == node) node->getParent()->setBalance(1);
        insertFix(node->getParent(), node);
    }
    else if (node->getParent()->getBalance() == -1 || node->getParent()->getBalance() == 1) 
    {
        node->getParent()->setBalance(0);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n)
{
    if (!p) return;
    if (!p->getParent()) return;

    AVLNode<Key, Value>* g = p->getParent();
    if (g->getLeft() == p) insertFixLeft(n, p, g);
    else if (g->getRight() == p) insertFixRight(n, p, g);
}

/*
* Helper function for insertFix
* Updates balances when p is a left node of its parent
*/
template<class Key, class Value>
void AVLTree<Key, Value>::insertFixLeft(AVLNode<Key, Value>* n, AVLNode<Key, Value>* p, AVLNode<Key, Value>* g)
{
    g->setBalance(g->getBalance() - 1);
    if (g->getBalance() == 0) return;
    else if (g->getBalance() == -1) insertFix(g, p);
    else if (g->getBalance() == -2) // When the subtree is unbalanced
    {
        if (zigzig(g))
        {
            rotateRight(g);
            p->setBalance(0);
            g->setBalance(0);
        }
        else if (zigzag(g))
        {
            rotateLeft(p);
            rotateRight(g);
            if (n->getBalance() == -1)
            {
                p->setBalance(0);
                g->setBalance(1);
                n->setBalance(0);
            }
            else if (n->getBalance() == 0)
            {
                p->setBalance(0);
                g->setBalance(0);
                n->setBalance(0);
            }
            else if (n->getBalance() == 1)
            {
                p->setBalance(-1);
                g->setBalance(0);
                n->setBalance(0);
            }
        }
    }
}

/*
* Helper function for insertFix
* Updates balances when p is a right node of its parent
*/
template<class Key, class Value>
void AVLTree<Key, Value>::insertFixRight(AVLNode<Key, Value>* n, AVLNode<Key, Value>* p, AVLNode<Key, Value>* g)
{

    g->setBalance(g->getBalance() + 1);
    if (g->getBalance() == 0) return;
    else if (g->getBalance() == 1) insertFix(g, p);
    else if (g->getBalance() == 2) // When the subtree is unbalanced
    {
        if (zigzig(g))
        {
            rotateLeft(g);
            p->setBalance(0);
            g->setBalance(0);
        }
        else if (zigzag(g)) 
        {
            rotateRight(p);
            rotateLeft(g);
            if (n->getBalance() == 1)
            {
                p->setBalance(0);
                g->setBalance(-1);
                n->setBalance(0);
            }
            else if (n->getBalance() == 0)
            {
                p->setBalance(0);
                g->setBalance(0);
                n->setBalance(0);
            }
            else if (n->getBalance() == -1)
            {
                p->setBalance(1);
                g->setBalance(0);
                n->setBalance(0);
            }
        }
    }
}

/*
* Helper function for the insert helper functions
* Checks if there is a zigzig case at the given node using its balances
*/
template<class Key, class Value>
bool AVLTree<Key, Value>::zigzig(AVLNode<Key, Value>* n)
{
    if (n->getBalance() < 0)
    {
        if (n->getLeft()->getBalance() <= 0) return 1;
    }
    if (n->getBalance() > 0)
    {
        if (n->getRight()->getBalance() >= 0) return 1;
    }
    return 0;
}

/*
* Helper function for the insert helper functions
* Checks if there is a zigzag case at the given node using its balances
*/
template<class Key, class Value>
bool AVLTree<Key, Value>::zigzag(AVLNode<Key, Value>* n)
{
    if (n->getBalance() < 0)
    {
        if (n->getLeft()->getBalance() > 0) return 1;
    }
    if (n->getRight() > 0)
    {
        if (n->getRight()->getBalance() < 0) return 1;
    }
    return 0;
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* g)
{
    AVLNode<Key, Value>* p = g->getLeft();
    AVLNode<Key, Value>* pRight = p->getRight();
    AVLNode<Key, Value>* gParent = g->getParent();

    // Updating necessary pointers and moving certain subtrees around
    p->setParent(gParent);
    if (gParent)
    {
        if (gParent->getLeft() == g) gParent->setLeft(p);
        else gParent->setRight(p);
    }
    p->setRight(g);
    g->setParent(p);
    g->setLeft(pRight);
    if (pRight) pRight->setParent(g);

    if (this->root_ == g) this->root_ = p;
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* g)
{
    AVLNode<Key, Value>* p = g->getRight();
    AVLNode<Key, Value>* pLeft = p->getLeft();
    AVLNode<Key, Value>* gParent = g->getParent();

    // Updating necessary pointers and moving certain subtrees around
    p->setParent(gParent);
    if (gParent)
    {
        if (gParent->getLeft() == g) gParent->setLeft(p);
        else gParent->setRight(p);
    }
    p->setLeft(g);
    g->setParent(p);
    if (pLeft) pLeft->setParent(g);
    g->setRight(pLeft);

    if (this->root_ == g) this->root_ = p;
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    if (this->empty()) return;
    AVLNode<Key, Value>* n = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (!n) return;
    bool isRoot = !n->getParent();

    AVLNode<Key, Value>* pPred = nullptr; // Parent of the predecessor node
    int diff = 0;


    if (n->getLeft() && n->getRight()) // If there are two children nodes, swap with predecessor
    {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(n));
        AVLNode<Key, Value>* pred_left = pred->getLeft();
        
        nodeSwap(n, pred);

        // Getting the new parent of the removee and using whether it is a left/right child to update balances
        pPred = n->getParent();
        if (pPred->getLeft() == n) diff = 1;
        else if (pPred->getRight() == n) diff = -1;

        if (pred_left) pred_left->setParent(n->getParent());

        if (n->getParent()->getLeft() == n) n->getParent()->setLeft(pred_left);
        else n->getParent()->setRight(pred_left);

        n->setLeft(nullptr);
        n->setParent(nullptr);

        if (isRoot) this->root_ = pred;
        delete n;
        n = nullptr;
    }
    else if (n->getLeft()) // If there is only a left child, swap with it
    {
        nodeSwap(n, n->getLeft());

        // Getting the new parent of the removee (which is just the old left node) and using whether it is a left/right child to update balances
        pPred = n->getParent();
        if (pPred->getLeft() == n) diff = 1;
        else if (pPred->getRight() == n) diff = -1;

        if (isRoot) this->root_ = n->getParent();

        if (n->getParent()) n->getParent()->setLeft(n->getLeft());
        if (n->getParent()) n->getParent()->setRight(n->getRight());

        if (n->getLeft()) n->getLeft()->setParent(n->getParent());
        if (n->getRight()) n->getRight()->setParent(n->getParent());

        delete n;
        n = nullptr;
    }
    else if (n->getRight()) // If there is only a right child, swap with it
    {
        nodeSwap(n, n->getRight());

        // Getting the new parent of the removee (which is just the old right node) and using whether it is a left/right child to update balances
        pPred = n->getParent();
        if (pPred->getLeft() == n) diff = 1;
        else if (pPred->getRight() == n) diff = -1;
        
        if (isRoot) this->root_ = n->getParent();

        if (n->getParent()) n->getParent()->setLeft(n->getLeft());
        if (n->getParent()) n->getParent()->setRight(n->getRight());

        if (n->getLeft()) n->getLeft()->setParent(n->getParent());
        if (n->getRight()) n->getRight()->setParent(n->getParent());

        delete n;
        n = nullptr;
    }

    removeFix(pPred, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* n, int diff)
{
    if (!n) return;
    AVLNode<Key, Value>* p = n->getParent();
    int ndiff = 0;
    if (p) 
    {
        if (p->getLeft() == n) ndiff = 1;
        else ndiff = -1;
    }
    if (diff == -1) // Updates pointers accordingly when diff == -1 (removing a node on the right)
    {
        if (n->getBalance() + diff == -2) // When the subtree is unbalanced
        {
            AVLNode<Key, Value>* c = n->getLeft();
            if (c->getBalance() == -1)
            {
                rotateRight(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            }
            else if (c->getBalance() == 0)
            {
                rotateRight(n);
                n->setBalance(-1);
                c->setBalance(1);
            }
            else if (c->getBalance() == 1)
            {
                AVLNode<Key, Value>* g = c->getRight();
                rotateLeft(c);
                rotateRight(n);
                if (g->getBalance() == 1)
                {
                    n->setBalance(0);
                    c->setBalance(-1);
                    g->setBalance(0);
                }
                else if (g->getBalance() == 0)
                {
                    n->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                else if (g->getBalance() == -1)
                {
                    n->setBalance(1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                removeFix(p, ndiff);
            }
        }
        else if (n->getBalance() + diff == -1) 
        {
            n->setBalance(-1);
        }
        else if (n->getBalance() + diff == 0) 
        {
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }
    else if (diff == 1) // Updates pointers accordingly when diff == 1 (removing a node on the left)
    {
        if (n->getBalance() + diff == 2) // When the subtree is unbalanced
        {
            AVLNode<Key, Value>* c = n->getRight();
            if (c->getBalance() == 1)
            {
                rotateLeft(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            }
            else if (c->getBalance() == 0)
            {
                rotateLeft(n);
                n->setBalance(1);
                c->setBalance(-1);
            }
            else if (c->getBalance() == -1)
            {
                AVLNode<Key, Value>* g = c->getLeft();
                rotateRight(c);
                rotateLeft(n);
                if (g->getBalance() == -1)
                {
                    n->setBalance(0);
                    c->setBalance(1);
                    g->setBalance(0);
                }
                else if (g->getBalance() == 0)
                {
                    n->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                else if (g->getBalance() == 1)
                {
                    n->setBalance(-1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                removeFix(p, ndiff);
            }
        }
        else if (n->getBalance() + diff == 1) 
        {
            n->setBalance(1);
        }
        else if (n->getBalance() + diff == 0) 
        {
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
