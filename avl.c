/*avl.c*/

//
// AVL Tree ADT implementation file.
//
// Alex Viznytsya
// Spring 2017
//

// ignore stdlib warnings if working in Visual Studio:
#define _CRT_SECURE_NO_WARNINGS 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "avl.h"

// AVLCreate:
// Dynamically creates and returns an empty AVL tree.
//
AVL *AVLCreate(void) {

    AVL *tree;
    tree = (AVL *)malloc(sizeof(AVL));
    tree->Root = NULL;
    tree->Count = 0;

    return tree;
}

// _AVLFree:
// This is AVLFree helper function. It recursivery free memory from the
// leaves of the tree to the root.
//
AVLNode *_AVLFree(AVLNode *root, void(*fp)(AVLKey key, AVLValue value)) {
    
    if(root == NULL) {
        return NULL;
    } else {
        _AVLFree(root->Left, fp);
        _AVLFree(root->Right, fp);
        fp(root->Key, root->Value);
        free(root);
    }
    
    return NULL;
}

// AVLFree:
// Frees the memory associated with the tree: the handle and the nodes.
// The provided function pointer is called to free the memory that
// might have been allocated as part of the key or value.
//
void AVLFree(AVL *tree, void(*fp)(AVLKey key, AVLValue value)) {

    _AVLFree(tree->Root, fp);
    free(tree);
    
    return;
}

// AVLCompareKeys:
// Compares key1 and key2, returning
//   value < 0 if key1 <  key2
//   0         if key1 == key2
//   value > 0 if key1 >  key2
//
int AVLCompareKeys(AVLKey key1, AVLKey key2) {
    
    if(key1 < key2) {
        return -1;
    } else if(key1 == key2) {
        return 0;
    } else {
        return 1;
    }
}


// AVLCount:
// Returns # of nodes in the tree.
//
int AVLCount(AVL *tree) {
  
    return tree->Count;
}

// AVLHeight:
// Returns the overall height of the AVL tree.
//
int AVLHeight(AVL *tree) {
    
    if (tree->Root == NULL) {
        return -1;
    } else {
        return tree->Root->Height;
    }
}

// _max2:
// Helper function that return largest of two numbers.
//
int _max2(int val1, int val2) {
    
    return (val1 > val2) ? val1 : val2;
}

// _height:
// Helper function that returns the heights of AVl node.
//
int _height(AVLNode *node) {

    if(node == NULL) {
        return -1;
    } else {
        return node->Height;
    }
}

// RightRotate:
// Rotate node to the right at k2, and returns new root node.
//
AVLNode *RightRotate(AVLNode *k2) {
    
    AVLNode *k1 = k2->Left;
    AVLNode *Y = k1->Right;
    
    k1->Right = k2;
    k2->Left = Y;
    
    k2->Height = 1 + _max2(_height(k2->Left), _height(k2->Right));
    k1->Height = 1 + _max2(_height(k1->Left), _height(k1->Right));
    
    return k1;
}

// LeftRotate:
// Rotate node to the left at k2, and returns new root node.
//
AVLNode *LeftRotate(AVLNode *k1) {
    
    AVLNode *k2 = k1->Right;
    AVLNode *Y = k2->Left;
    
    k2->Left = k1;
    k1->Right = Y;
    
    k1->Height = 1 + _max2(_height(k1->Left), _height(k1->Right));
    k2->Height = 1 + _max2(_height(k2->Left), _height(k2->Right));
    
    return k2;
}


// AVLInsert:
// Inserts new AVlValue node to the AVL tree.
//
boolean AVLInsert(AVL *tree, AVLKey key, AVLValue value) {
    
    AVLNode *prev = NULL;
    AVLNode *cur = tree->Root;
    AVLNode *stack[64];
    int topStack = -1;
    
    // Find location where to insert new mode:
    while(cur != NULL) {
        topStack++;
        stack[topStack] = cur;
        if(AVLCompareKeys(key, cur->Key) == 0) {
            return false;
        } else if(AVLCompareKeys(key, cur->Key) < 0) {
            prev = cur;
            cur = cur->Left;
        } else {
            prev = cur;
            cur = cur->Right;
        }
    }
    
    // Create new node:
    AVLNode *newNode = (AVLNode *)malloc(sizeof(AVLNode));
    newNode->Key = key;
    newNode->Value = value;
    newNode->Height = 0;
    newNode->Left = NULL;
    newNode->Right = NULL;
    
    // Insert new node:
    if(prev == NULL) {
        tree->Root = newNode;
    } else if(AVLCompareKeys(key, prev->Key) < 0) {
        prev->Left = newNode;
    }  else {
        prev->Right = newNode;
    }
    tree->Count++;
    
    // Check if AVL tree is balanced:
    boolean rebalance = false;
    AVLNode *N = NULL;
    while(topStack >= 0) {
        N = stack[topStack];
        topStack--;
        
        int hl = _height(N->Left);
        int hr = _height(N->Right);
        int newH = 1 + _max2(hl, hr);
        
        if(N->Height == newH) {
            rebalance = false;
            break;
        } else if(abs(hl - hr) > 1) {
            rebalance = true;
            break;
        } else {
            N->Height = newH;
        }
    }
    
    // Balance AVL tree if needed:
    AVLNode *K = NULL;
    if(rebalance) {
        cur = N;
        if(topStack < 0) {
            prev = NULL;
        } else {
            prev = stack[topStack];
        }
        
        // Case 1 or 2:
        if(_height(cur->Left) > _height(cur->Right)) {
            K = cur->Left;
            
            // Case 2, left rotate @ K:
            if(_height(K->Left) < _height(K->Right)) {
                cur->Left = LeftRotate(K);
            }
            
            // Second right rotation for case 2, and case 1 rotation @ cur:
            if(prev == NULL) {
                tree->Root = RightRotate(cur);
            } else if(prev->Left == cur) {
                prev->Left = RightRotate(cur);
            } else {
                prev->Right = RightRotate(cur);
            }
        } else {
        // Case 3 or 4:
            K = N->Right;
            
            // Case 3, right rotate @ K:
            if(_height(K->Left) > _height(K->Right)) {
                cur->Right = RightRotate(K);
            }
            
            // Second left rotation for case 3, and case 4 rotation @ cur:
            if(prev == NULL) {
                tree->Root = LeftRotate(cur);
            } else if(prev->Left == cur) {
                prev->Left = LeftRotate(cur);
            } else {
                prev->Right = LeftRotate(cur);
            }
        }
    }
    
    return true;
}

// AVLSearch:
// Search and return AVL node in AVL tree using AVLKey.
//
AVLNode *AVLSearch(AVL *tree, AVLKey key) {
    
    if(tree->Root == NULL) {
        return NULL;
    } else {
        AVLNode *cur = tree->Root;
        while(cur != NULL) {
            if(AVLCompareKeys(key, cur->Key) == 0) {
                return cur;
            } else if(AVLCompareKeys(key, cur->Key) < 0) {
                cur = cur->Left;
            } else {
                cur = cur->Right;
            }
        }
    }
    
    return NULL;
}

