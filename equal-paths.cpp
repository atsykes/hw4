#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here


bool equalPaths(Node * root)
{
    // Add your code below
    if (!root) return 1;

    return _checkLeafPaths(root->left, 0) == _checkLeafPaths(root->right, 0);
}

/*
* Finds the path of a subtree, given the root of that subtree
*/
int _checkLeafPaths(const Node* root, int h)
{
	if (!root) return 0;

    if (root->left && root->right)
    {
        // If one child is a leaf and the other one isnt return -1 (does not have equal path). Otherwise, track height
        if ((_isLeaf(root->left) && !_isLeaf(root->right)) || (!_isLeaf(root->left) && _isLeaf(root->right))) return -1;
        else if (_isLeaf(root->left) && _isLeaf(root->right)) return h+1;

        // Save values from the left and right
        int lvalue = _checkLeafPaths(root->left, h+1);
        int rvalue = _checkLeafPaths(root->right, h+1);
        
        // If one of the values is -1, return it (does not have equal paths)
        if (lvalue == -1 || rvalue == -1) return -1;
        if (lvalue > rvalue) return lvalue + 1;
        else return rvalue + 1;
    }
    else if (root->left)
    {
        // Similar to above, returns -1 if any subtree does not have an equal path
        int lvalue = _checkLeafPaths(root->left, h+1);
        if (lvalue == -1) return -1;
        return lvalue + 1;
    }
    else if (root->right)
    {
        // Similar to above, returns -1 if any subtree does not have an equal path
        int rvalue = _checkLeafPaths(root->right, h+1);
        if (rvalue == -1) return -1;
        return rvalue + 1;
    }
}

/*
* Checks whether a given node is a leaf node (no children)
*/
bool _isLeaf(const Node* n)
{
    return !n->left && !n->right;
}
