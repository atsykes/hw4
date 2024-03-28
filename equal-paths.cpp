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

int _checkLeafPaths(const Node* root, int h)
{
	if (!root) return 0;

    if (root->left && root->right)
    {
        if ((_isLeaf(root->left) && !_isLeaf(root->right)) || (!_isLeaf(root->left) && _isLeaf(root->right))) return -1;
        else if (_isLeaf(root->left) && _isLeaf(root->right)) return h+1;

        int lvalue = _checkLeafPaths(root->left, h+1);
        int rvalue = _checkLeafPaths(root->right, h+1);
        
        if (lvalue == -1 || rvalue == -1) return -1;
        if (lvalue > rvalue) return lvalue + 1;
        else return rvalue + 1;
    }
    else if (root->left)
    {
        int lvalue = _checkLeafPaths(root->left, h+1);
        if (lvalue == -1) return -1;
        return lvalue + 1;
    }
    else if (root->right)
    {
        int rvalue = _checkLeafPaths(root->right, h+1);
        if (rvalue == -1) return -1;
        return rvalue + 1;
    }
}

bool _isLeaf(const Node* n)
{
    return !n->left && !n->right;
}
