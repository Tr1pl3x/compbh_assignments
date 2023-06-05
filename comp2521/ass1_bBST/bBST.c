#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "bBST.h"
#include "List.h"

////////////////////////////////////////////////////////////////////////
/* IMPORTANT:
   Do NOT modify the code above this line.
*/
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// Auxiliary function prototypes

// Write Auxiliary function prototypes here, and declare them as static

// this varaiable has been used in TreeKthSmallest and TreeKthLargest functions.
static int count;

/* Reference:
* The followind functions: max, rightRotate, leftRotate, takeBlanace and makeBalance,
* are inspired and taken as reference from https://www.geeksforgeeks.org/insertion-in-an-avl-tree/
*/

// Finds the height of the given node
static int height( Node n) {
    if (n == NULL) return 0;
    return n->height;
}

// returns the greater value among the two given integers
static int max(int a, int b) {
    return (a > b)? a : b;
}

// Perfroms right rotation from the given node
static Node rightRotate(Node n) {
    
    Node left_node = n->left;
    Node sub_tree = left_node->right;
 
    // Perform rotation
    left_node->right = n;
    n->left = sub_tree;
 
    // Update the heights of the nodes
    n->height = max(height(n->left), height(n->right)) + 1;
    left_node->height = max(height(left_node->left), height(left_node->right)) + 1;
 
    return left_node;
}

// Performs left rotation from the given node
static Node leftRotate(Node n) {
    
    Node right_node = n->right;
    Node sub_tree = right_node->left;
 
    // Perform rotation
    right_node->left = n;
    n->right = sub_tree;
 
    // Update heights
    n->height = max(height(n->left), height(n->right)) + 1;
    right_node->height = max(height(right_node->left), height(right_node->right)) + 1;
 
    return right_node;
}

// finds the height difference of the node's right and left child
static int takeBalance(Node n) {
    if ( n == NULL) return 0;
    return height(n->left) - height(n->right);
}

// balances the tree
static Node makeBalance(Node node, int key){
    
    int balance = takeBalance(node);
 
    if (balance > 1 && key < node->left->key){
        return rightRotate(node);
    }
    if (balance < -1 && key > node->right->key){
        return leftRotate(node);
    }
    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    if (balance < -1 && key < node->right->key){
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}

// creates a new node using given key
static Node newNode(int key){
    
    Node rn = (Node) malloc(sizeof(struct node));
    rn->key = key;
    rn->left = rn->right = NULL;
    rn->height = 1;
    return rn;

}

// inserts new node
static Node insertNode(Node node, int key) {   
    
    // Node is empty
    if (node == NULL){
        node = newNode(key);
    }
 
    if (key < node->key) {
        node->left = insertNode(node->left, key);
    } else if (key > node->key) {
        node->right = insertNode(node->right, key);
    } else { 
        return node;
    }

    // after successful insertion, updates the height and rebalances the tree.
    node->height = 1 + max(height(node->left), height(node->right));
    node = makeBalance(node, key);
    
    return node;
}

// frees the given node
static void freeTree(Node n){
    
    if(n != NULL){
       freeTree(n->left);
       freeTree(n->right);
       free(n);
    }
}

// search the given key in the tree
static bool searchTree(Node n, int key){
    
    if (n == NULL) {
        return false;
    } else if (key < n->key){
        return searchTree(n->left, key);
    } else if (key > n->key){
        return searchTree(n->right, key);
    } else{
        return true;
    }                               
       
}

// join the two given nodes
static Node joinTrees(Node n1, Node n2) {
   
    if (n1 == NULL) {
        return n2;
    } else if (n2 == NULL) {
        return n1;
    } else {
        Node curr = n2;
        Node parent = NULL;
        while (curr->left != NULL) {    
	        parent = curr;
	        curr = curr->left;
        }
        if (parent != NULL) {
	        parent->left = curr->right;  
	        curr->right = n2;
        }
        curr->left = n1;
        return curr; 
    }
}

// deletes the given key from the node
static Node deleteNode(Node t, int key){
    
    Node new_tree;
    if (t != NULL) {
        if (key < t->key) {
	        t->left = deleteNode(t->left, key);
        } else if (key > t->key) {
	        t->right = deleteNode(t->right, key);
        } else {
	        if (t->left == NULL && t->right == NULL){
                new_tree = NULL;
            } else if (t->left == NULL){
                new_tree = t->right;
            } else if (t->right == NULL){
                new_tree = t->left;
            } else {
                new_tree = joinTrees(t->left, t->right);
            }
            free(t);
            t = new_tree;
        }
    }
   return t;
}

// changes nodes into list
static void nodeASList(Node n, List l){

    // if node is empty
    if (n == NULL) return;

    nodeASList(n->left, l);
    ListAppend(l, n->key);
    nodeASList(n->right, l);
}

// find the kth smallest node
static Node kthSmallestNode(Node n, int k){
    
    // if node is empty
    if (n == NULL)
        return NULL;
 
    Node leftNode = kthSmallestNode(n->left, k);
    if (leftNode != NULL) return leftNode;
 
    count++;
    if (count == k) {
        return n;
    }
    return kthSmallestNode(n->right, k);
}

// find the kth largest node
static Node kthLargestNode(Node n, int k){
    
    // if node is empty
    if (n == NULL) return NULL;
 
    Node rightNode = kthLargestNode(n->right, k);
    if (rightNode != NULL) return rightNode;
 
    count++;
    if (count == k){
        return n;
    } 
    return kthLargestNode(n->left, k);
}

// finds the largest key less than or equal
static int FindFloor(Node n, int key){
    
    // if node is empty
    if (n == NULL)
        return UNDEFINED;
 
    if (n->key == key) {
        return n->key;

    } else if (n->key > key) {
        return FindFloor(n->left, key);

    } else {
        
        int floor_value = FindFloor(n->right, key);
        if(floor_value <= key && floor_value != UNDEFINED){
            return floor_value;
        }
        else{
            return n->key;
        }
    }
}

// finds the smallest key less than or equal
static int FindCeiling(Node n, int key){
    
    // if node is empty
    if (n == NULL)
        return UNDEFINED;
 
    if (n->key == key) {
        return n->key;
    }
    if (n->key < key) {
        return FindCeiling(n->right, key);
    }
    int ceiling_value = FindCeiling(n->left, key);
    if (ceiling_value >= key) {
        return ceiling_value;
    } else {
        return n->key;
    }
}

// finds the keys between two given keys
static void treeBetweenTwoKey(Node n, int lower, int upper, List l){
    
    // if node is empty
    if ( n == NULL ) {
      return;
    }
    if ( lower < n->key ) {
        treeBetweenTwoKey(n->left, lower, upper, l);
    }
    if ( lower <= n->key && upper >= n->key ){
        ListAppend(l, n->key);  
    }
    treeBetweenTwoKey(n->right, lower, upper, l);
}

// finds the lowest common ancestor between two given keys
static int lcaBtwTwoKey(Node n, int a, int b){
    
    // if node is empty
    if (n == NULL) return UNDEFINED;
 
    if (n->key > a && n->key > b) {
        return lcaBtwTwoKey(n->left, a, b);
    }
    if (n->key < a && n->key < b) {
        return lcaBtwTwoKey(n->right, a, b);
    }
    return n->key;
}

////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////

/**
 * Creates a new empty tree.
 * The time complexity of this function must be O(1).
 */
Tree TreeNew(void) {

    Tree new_tree = (Tree )malloc (sizeof(Node));
    new_tree->root = NULL;
    return new_tree;
}

////////////////////////////////////////////////////////////////////////

/**
 * Frees all memory allocated for the given tree.
 * The time complexity of this function must be O(n).
 */
void TreeFree(Tree t) {

    freeTree(t->root);
    free(t);
}

////////////////////////////////////////////////////////////////////////

/**
 * Searches the tree for a given key and returns true if the key is in
 * the tree or false otherwise.
 * The time complexity of this function must be O(log n).
 */
bool TreeSearch(Tree t, int key) {

    if (searchTree(t->root, key)){
        return true;
    } else {
        return false;
    }
    
}

////////////////////////////////////////////////////////////////////////

/**
 * Inserts the given key into the tree.
 * You must use the proper AVL insertion algorithm.
 * Returns true if the key was inserted successfully, or false if the
 * key was already present in the tree.
 * The time complexity of this function must be O(log n).
 */
bool TreeInsert(Tree t, int key) {
    // TODO: Complete this function
    if (t->root != NULL){
        if(TreeSearch(t, key)){
            return false;
        }
    }
    t->root = insertNode(t->root, key);
    if (TreeSearch(t, key)){
        return true;
    } else {
        return false;
    }
}


////////////////////////////////////////////////////////////////////////

/**
 * Deletes the given key from the tree if it is present.
 * You must use the proper AVL deletion algorithm.
 * Returns true if the key was deleted successfully, or false if the key
 * was not present in the tree.
 * The time complexity of this function must be O(log n).
 */
bool TreeDelete(Tree t, int key) {

    if (t != NULL){
        t->root = deleteNode(t->root, key);
        if (t != NULL){
            if (!TreeSearch(t, key)){
                return true;
            } else {
                return false;
            }
        } else {
            return true;
        }
    } else {
        return false;
    }
}

////////////////////////////////////////////////////////////////////////

/**
 * Creates a list containing all the keys in the given tree in ascending
 * order.
 * The time complexity of this function must be O(n).
 */
List TreeToList(Tree t) {
 
    List asl = ListNew();
    nodeASList(t->root, asl);
    return asl;
}

////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////

/**
 * Returns the k-th smallest key in the tree.
 * Assumes that k is between 1 and the number of nodes in the tree.
 * k = 1 will return the smallest value in the tree.
 * The time complexity of this function must be O(log n + k).
 */

int TreeKthSmallest(Tree t, int k) {

    count = 0;
    Node smallest = kthSmallestNode(t->root, k);
    return smallest->key;
}

////////////////////////////////////////////////////////////////////////

/**
 * Returns the k-th largest key in the tree.
 * Assumes that k is between 1 and the number of nodes in the tree.
 * k = 1 will return the largest value in the tree.
 * The time complexity of this function must be O(log n + k).
 */
int TreeKthLargest(Tree t, int k) {

    count = 0;
    Node largest = kthLargestNode(t->root, k);
    return largest->key;
}

////////////////////////////////////////////////////////////////////////

/**
 * Returns the least common ancestor of two keys, a and b.
 * Returns UNDEFINED if either a or b are not present in the tree.
 * a may be larger than b, or equal to each other.
 * Either a or b may be the LCA in some cases.
 * The time complexity of this function must be O(log n).
 */

int TreeLCA(Tree t, int a, int b) {

    if((!TreeSearch(t, a)) || (!TreeSearch(t, b))){
        return UNDEFINED;
    }
    int lca = lcaBtwTwoKey(t->root, a, b);
    return lca;
}

////////////////////////////////////////////////////////////////////////

/**
 * Returns the largest key less than or equal to the given value.
 * Returns UNDEFINED if there is no such key.
 * The time complexity of this function must be O(log n).
 */
int TreeFloor(Tree t, int key) {

    int floor_key = FindFloor(t->root, key);
    return floor_key;
}

////////////////////////////////////////////////////////////////////////

/**
 * Returns the smallest key greater than or equal to the given value.
 * Returns UNDEFINED if there is no such key.
 * The time complexity of this function must be O(log n).
 */

int TreeCeiling(Tree t, int key) {

    int ceiling_key = FindCeiling(t->root, key);
    return ceiling_key;
}

////////////////////////////////////////////////////////////////////////

/**
 * Searches for all keys between the two given keys (inclusive) and
 * returns the keys in a list in ascending order.
 * Assumes that lower is less than or equal to upper.
 * The time complexity of this function must be O(log n + m), where m is
 * the length of the returned list.
 */

List TreeSearchBetween(Tree t, int lower, int upper) {

    List l = ListNew();
    treeBetweenTwoKey(t->root, lower, upper, l);
    return l;
}

////////////////////////////////////////////////////////////////////////

