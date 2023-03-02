#include <stdlib.h>
#include <iostream>
#include <cmath>
#include "disk_storage.h"

using namespace std;

//number of keys in a node, N+1 pointers.
//const int N = 9;

// This is how many keys we can have, by using memory arithmetic, the way C++ handles padding is accounted for.
const int N = floor((blockSize - sizeof(int) - sizeof(int) - sizeof(int))/(sizeof(int) + sizeof(void *) + sizeof(void*)));

// max number of records per block.
const int max_rec = floor(blockSize/sizeof(Record));

struct queryResults {
    Record record;
    queryResults * nextRecord = NULL;

};

class Node {

    public: 
        bool isLeaf; //boolean to signify if the B+tree node is a leaf or internal node
        int keys[N], size; // pointer to keys (numVotes) and track node size

        Node *ptrs[N + 1]; // physical storage of Node pointers array there's always 1 more pointer than keys
        Record *blocks[N]; //physical storage of Record pointers array
        //int keys[N]; // physical storage of keys array

        //Node * nextLeaf;
        Node(bool);
        void printNode();
};

Node::Node(bool toBeLeaf) {
    isLeaf = toBeLeaf;
    size = 0;

    if (toBeLeaf == true) ptrs[0] = NULL;

    // key = keys;
    // ptr = ptrs;
};
// prints first five index nodes
void Node::printNode(){
    cout << "Index Node accessed, its keys are: ";
    for (int i = 0; i < size; i++){
        cout << keys[i] << " ";   
    }
    cout << "\n";
};

class BPTree {

    private:
        void _insert(int *, Record *, Node *, Node**, bool *, bool *); // int for key you want to insert; Node pointer to enable recursive insert; flag to determine direction of insert
        void deleteInternalNode(int, Node *, Node *);
        Node *findParent(Node *, Node *);     

    public:
        Node *root;

        BPTree();
        void insert(int, Record *);
        void deleteKey(int);
        void displayTree(Node *);
        void displayTreeModified(Node *);
        void query(int, queryResults **);
        void query(int, int, queryResults **);    
        Node * getRoot();
        int getHeight();
        int countNodes(Node *, int &);
};

BPTree::BPTree() {
    root = NULL;
};

void BPTree::query(int x, queryResults ** results) {
    int indexesAccessed = 0;
    int blocksAccessed = 0;

    if (root == NULL) {
        cout << "Tree is empty\n";
        return;

    } else {

        Node *cursor = root;
        while (cursor->isLeaf == false) { // traversing in internal nodes

            // cursor->printNode(); // prints the keys for the first 5 index nodes
            indexesAccessed++;

            for (int i=0; i<cursor->size; i++) {
                if (x < cursor->keys[i]) {
                    cursor = cursor->ptrs[i]; // moving down the left branch of internal node
                    break;
                }
                if (i == cursor->size-1) {
                    cursor = cursor->ptrs[i+1]; // moving down the right branch of internal node
                    break;
                }
            }
        }
        
        // in leaf node
        // if (indexesAccessed < 5) cursor->printNode(); // this prints the leaf nodes
        indexesAccessed++;

        queryResults ** temp = results;

        for (int i=0; i<cursor->size; i++) {

            if (x == cursor->keys[i]) {
                // cout << "Key " << cursor->keys[i] << " found\n";
                // ptr to head of block == cursor->blocks[i]
            
                bool foundlast = false;

                Record * follow = cursor->blocks[i];

                while(foundlast == false){
                    
                    // this prints the first 5 data blocks accessed
                    if (blocksAccessed < 5){
                        // cout << "data block accessed, its tconst values are: ";
                        Record * blk = follow;
                        for (int j = 0; j < max_rec; j++){
                            Record * r = &follow[j];
                            // if (r->deleted == false) 
                            // cout << r->tconst << " ";
                        }
                        // cout << "\n";
                    }
                    blocksAccessed++;

                    for (int i = 0; i < max_rec; i++){
                        Record * r = &follow[i];

                        if (r->numVotes == x){
                            
                            *temp = new queryResults;
                            (*temp)->record = *r;
                            temp = &((*temp)->nextRecord);

                            if (r->nextDupe == 0){
                                foundlast = true;
                                break;
                            }

                            else if (r->nextDupe == follow){
                                //temp->nextRecord = new queryResults;
                                
                                continue;
                            }

                            else {
                                // temp->nextRecord = new queryResults;

                                follow = r->nextDupe;
                                break;
                            }
                            
                        }
                    }
                }
                cout << "Number of index node(s) accessed: " << indexesAccessed << "\n";
                cout << "Number of data block(s) accessed: " << blocksAccessed << "\n";

                return; // exit the function
                //return queryResults; // this exits the function
            }
        }
        cout << "[Key not found]\n";
        cout << "Number of index node(s) accessed: " << indexesAccessed << "\n";
        cout << "Number of data block(s) accessed: " << blocksAccessed << "\n";
        return;
    }
};

// this function executes a ranged query
void BPTree::query(int min, int max, queryResults ** results) {
    int indexesAccessed = 0;
    int blocksAccessed = 0;

    if (root == NULL) {
        cout << "Tree is empty\n";
        return;

    } else {

        Node *cursor = root;
        while (cursor->isLeaf == false) { // traversing in internal nodes

            // cursor->printNode(); // prints keys for the first 5 index nodes
            indexesAccessed++;

            for (int i=0; i<cursor->size; i++) {
                if (min < cursor->keys[i]) {
                    cursor = cursor->ptrs[i]; // moving down the left branch of internal node
                    break;
                }
                if (i == cursor->size-1) {
                    cursor = cursor->ptrs[i+1]; // moving down the right branch of internal node
                    break;
                }
            }
        }
        
        // in leaf node
        // if (indexesAccessed < 5) cursor->printNode(); // this prints the leaf nodes
        indexesAccessed++;

        bool queryEnd = false;
        queryResults ** temp = results;

        while (queryEnd == false){
        
            for (int i=0; i<cursor->size; i++) {

                if (min <= cursor->keys[i] && max >= cursor->keys[i]) {
                    int x = cursor->keys[i];
                    // cout << "Key " << x << " found\n";
                    // ptr to head of block == cursor->blocks[i]
                
                    bool foundlast = false; // flag to check if duplicate chain has ended

                    Record * follow = cursor->blocks[i];

                    while(foundlast == false){
                        
                        // this prints the first 5 data blocks accessed
                        if (blocksAccessed < 5){
                            // cout << "Key " << x << " found\n";
                            // cout << "data block accessed, its tconst values are: ";
                            Record * blk = follow;
                            for (int j = 0; j < max_rec; j++){
                                Record * r = &follow[j];
                                // if (r->deleted == false) cout << r->tconst << " ";
                            }
                            // cout << "\n";
                        }
                        blocksAccessed++;
                        if (blocksAccessed == 6) {
                            // cout << "Remaining keys hidden.\n";
                        }

                        for (int i = 0; i < max_rec; i++){
                            Record * r = &follow[i];

                            if (r->numVotes == x){
                                *temp = new queryResults;
                                (*temp)->record = *r;

                                temp = &((*temp)->nextRecord);

                                if (r->nextDupe == 0){
                                    foundlast = true;
                                    break;
                                }

                                else if (r->nextDupe == follow){
                                    continue;
                                }

                                else {
                                    follow = r->nextDupe;
                                    break;
                                }
                                
                            }
                        }
                    }
                } else if (max < cursor->keys[i]) {
                    queryEnd = true;
                    break;
                }
            }
            if (cursor->ptrs[0] != NULL) cursor = cursor->ptrs[0]; // visit sibling leaf node;
            else queryEnd = true; // there are no more sibling leaf nodes to visit, end the search.

        }
        cout << "Number of index node(s) accessed: " << indexesAccessed << "\n";
        cout << "Number of data block(s) accessed: " << blocksAccessed << "\n";

        return; // exit the function

    }
};

int BPTree::getHeight() {
    int height = 1;
    
    if (root == NULL) {
        cout << "Tree is empty\n";
        return 0;
    } else {
        Node *cursor = root;
        while (cursor->isLeaf == false) {
            cursor = cursor->ptrs[0];
            height++;
        }
    }
    
    return height;
}

int BPTree::countNodes(Node *cursor, int &count) {
    if (cursor != NULL) {
        //cout << "the size is " << cursor->size << "\n";
        count++;

        if (cursor->isLeaf == false) { // internal node
            for (int i=0; i<cursor->size+1; i++) { // (n+1) nodes for n keys
                countNodes(cursor->ptrs[i], count); // recursive call
            }
        }
    }
    return count;
}

void BPTree::displayTree(Node *cursor) {
    if (root == cursor) cout << "[ROOT]\n";
    else if (cursor->isLeaf == false) cout << "[INTERNAL]\n";
    else cout << "[LEAVES]\n";
    //else return;

    if (cursor != NULL) {
        //cout << "the size is " << cursor->size << "\n";
        for (int i=0; i<cursor->size; i++) {
            cout << cursor->keys[i] << " "; // print elements in each node
        }

        cout << "\n";
        if (cursor->isLeaf == false) { // internal node
            for (int i=0; i<cursor->size+1; i++) { // (n+1) nodes for n keys
                displayTree(cursor->ptrs[i]); // recursive call
            }
        }
    }

    if (root == cursor) cout << "(END)\n";
}

//prints root nodes
void BPTree::displayTreeModified(Node *cursor) {
    cout << "[ROOT]\n";

    if (cursor != NULL) {
        //cout << "the size is " << cursor->size << "\n";
        for (int i=0; i<cursor->size; i++) {
            cout << cursor->keys[i] << " "; // print elements in each node
        }
        cout << "\n";

        // if (cursor->isLeaf == false){
        //     cursor = cursor->ptrs[0];
        //     if (cursor->isLeaf == false) cout << "FIRST CHILD NODE - [INTERNAL]\n";
        //     else cout << "FIRST CHILD NODE - [LEAVES]\n";
        // }
        // else cursor = NULL;

        // if (cursor != NULL) {
        //     for (int i=0; i<cursor->size; i++) {
        //         cout << cursor->keys[i] << " "; // print elements in each node
        //     }
        // }
    }
    cout << "\n(END)\n";
}

Node *BPTree::getRoot() {
    return root;
}

void BPTree::insert(int key, Record * block) { //wrapper function to hide complexity
    int * newKey = &key;
    
    Node * tempPtr = NULL;
    Node ** newPtr = &tempPtr;
    bool isInternal = false; // are we currently trying to deal with internal nodes?
    bool isInserting = true; // are we currently trying to insert anything?

    bool * internal = &isInternal;
    bool * inserting = &isInserting;

    _insert(newKey, block, root, newPtr, internal, inserting);

}

// let's make this recursive + make a wrapper function for this after we are done
void BPTree::_insert(int * newKey, Record * block, Node * current, Node ** newPtr, bool * internal, bool * inserting) {
    // internal should false and inserting should be false on the first run
    // case 1: Root is empty (new tree)
    if (root == NULL) {
        root = new Node(true);
        root -> keys[0] = *newKey;
        root -> size = 1;
        root -> blocks[0] = block;
        //root -> nextLeaf = NULL;
        return; //nothing else to do, just insert
    }

    // case 2: direction - DOWN -> find correct LEAF node for this key
    if (current->isLeaf == false && *internal == false){

        Node * cur = current;
        int * cur_key = cur->keys;
        Node ** cur_ptr = cur->ptrs;

        for (int i = 0; i < cur -> size; i++) {
            if (*newKey < *cur_key){
                cur = *cur_ptr;
                break;
            }

            cur_key++;
            cur_ptr++;

            if (i == cur->size - 1) { // check if we are already on the last pointer
                cur = *cur_ptr; // last pointer
                break;
            }
        }

        _insert(newKey, block, cur, newPtr, internal, inserting);
    }

    //This only runs on leaf nodes layer, now we need to find out where in the node to insert the new key
    if (*inserting == true && *internal == false) {
        // basic variables that we need
        Node * cur = current;
        int * cur_key = cur->keys;
        Record ** cur_block = cur->blocks;

        int keytoinsert = *newKey;
        Record * blocktoinsert = block;

        for (int i = 0; i < current -> size; i++, cur_key++, cur_block++) {
            if (keytoinsert < *cur_key){ // slightly inefficient, after we slot in the original keytoinsert, we know the rest will keep swapping...
                int temp = *cur_key;
                Record * temp_block = *cur_block;
                *cur_key = keytoinsert;
                *cur_block = blocktoinsert;
                keytoinsert = temp;
                blocktoinsert = temp_block;
            }

            else if (keytoinsert == *cur_key){ // we dont have to insert any key, instead we just chain the records with the same key together like a linked list

                *inserting = false;

                Record * follow = *cur_block;

                bool foundlast = false; // have we found the very last node in the duplicate linked list? i.e. hit NULL

                while (foundlast == false){
                    int i;
                    for (i = 0; i < max_rec; i++){ // this is how we can search through the whole block
                        Record * r = &follow[i];

                        if (keytoinsert == r->numVotes){
                            if (r->nextDupe == 0){ // found the end, insert the next dupe and end.
                                foundlast = true;
                                r->nextDupe = blocktoinsert;
                                break;
                            }
                            else if (r->nextDupe == follow){ // if the next Dupe is the same block so just continue searching
                                continue;
                            }
                            else { // if not follow the chain
                                follow = r->nextDupe;
                                break;
                            }
                        }
                    }

                    if (i == max_rec) foundlast = true; // if we exhaust an entire block, just end it here.
                }

                return;
            }
        }

        //we're gonna always have one remaining 'keytoinsert' be it our original newkey or not
        if (current->size < N){ // since current size is smaller than node size we can just add it in normally
            *cur_key = keytoinsert;
            *cur_block = blocktoinsert;
            cur->size++;
            *inserting = false;
        }
        // node overflow! time to split the node.
        else {
            int half = ceil((float) N/2); 
            Node * newnode = new Node(true);

            // populate the new node
            for (int i = half, j = 0; i<N; i++, j++) {
                newnode->keys[j] = cur->keys[i];
                newnode->blocks[j] = cur->blocks[i];
                newnode->size++;
                // make the old node empty
                // cur->key[i] = NULL;
                // cur->record[i] = NULL;
                cur->size--;
            }

            // we still haven't insert the last "keytoinsert" and "datatoinsert" from earlier
            int last = floor((float) N/2);
            newnode-> keys[last] = keytoinsert;
            newnode-> blocks[last] = blocktoinsert;
            newnode->size++;

            // since this new node is to the "right" of the old node
            newnode->ptrs[0] = cur->ptrs[0];
            cur->ptrs[0] = newnode;

            *newKey = newnode->keys[0]; // take the smallest key on the right subtree for indexing
            *newPtr = newnode; // this shares to all recursive layers that this is the new pointer to be inserted

            // this sets inserting to be true throughout all levels of the recursive function stack.
            // which basically means that when we return to an earlier level, we are telling it we need to insert a new key
            *inserting = true; //redundant

            // final case, you need a parent node but there isnt 1 (i.e. you are on root - need a new root):
            if (root == current){
                Node * newroot = new Node(false);
                newroot->size = 1;
                newroot->keys[0] = *newKey;
                newroot->ptrs[0] = current;
                newroot->ptrs[1] = newnode;
                //newroot->nextLeaf = NULL;
                newroot->isLeaf = false;

                root = newroot;
            }
            

        }

        // here, we are absolutely done with the leaf nodes, and we proceed to deal with the internal nodes, by going up the recusive functions
        *internal = true;

        // return here as we do not need to progress further on the leave layer.
        return;
    }
    

    //This runs only on the internal layers, the pointers to boolean values will tell us so.
    if (*inserting == true && *internal == true){
        Node * cur = current;
        int * cur_key = cur->keys;
        Node ** cur_ptr = cur->ptrs;

        cur_ptr++; // we do this because we will never touch the first (left-most) pointer. why?

        int keytoinsert = *newKey;
        Node * ptrtoinsert = *newPtr;

        //look for where to insert the new key
        for (int i = 0; i < current -> size; i++, cur_key++, cur_ptr++) {
            if (keytoinsert < *cur_key){
                int temp = *cur_key;
                Node * temp_ptr = *cur_ptr;
                *cur_key = keytoinsert;
                *cur_ptr = ptrtoinsert;
                keytoinsert = temp;
                ptrtoinsert = temp_ptr;
            }
        }

        // CASE 1: INTERNAL NODE IS NOT FULL.
        if (cur->size < N){
            *cur_key = keytoinsert;
            *cur_ptr = ptrtoinsert;
            cur->size++;
            *inserting = false; // we don't have to insert anymore, we have found where to insert the internal node key.
        }

        // CASE 2: INTERNAL NODE IS FULL (DON'T INSERT AT THIS LEVEL, INSERT AT THE LEVEL ABOVE)
        else {
            int half = ceil((float) N/2);
            Node * newnode = new Node(false);

            *newKey = cur->keys[half]; // this is the key we need to insert upwards...
            //cur -> key[half + 1] = NULL;
            cur->size--;

            for (int i = half, j = 0; i<N; i++, j++){
                if (i + 1 < N){
                    newnode->keys[j] = cur->keys[i+1]; //we have to skip 1 here
                    cur->size--;
                    newnode->size++;
                }
                //cout << "the key now is " << cur->key[i] << "\n";
                newnode->ptrs[j] = cur->ptrs[i+1];
                

                //delete the original nodes || we actually dont' have to set NULL as long as the size is correct
                // cur->key[i] = NULL;
                // cur->ptr[i] = NULL;

            }

            int last = floor((float) N/2);
            newnode -> keys[last-1] = keytoinsert; // why do we -1? because we have 1 less! since we skipped 1 earlier...
            newnode -> ptrs[last] = ptrtoinsert;
            newnode -> size++;

            *inserting = true; //we must continue inserting

            *newPtr = newnode; //the new node does not have a parent

            // final case for internal node, you need to create a parent node but there isnt 1:
            if (root == current){
                Node * newroot = new Node(false);
                newroot->size = 1;
                newroot->keys[0] = *newKey;
                newroot->ptrs[0] = current;
                newroot->ptrs[1] = *newPtr;
                //newroot->nextLeaf = NULL;
                newroot->isLeaf = false;

                root = newroot;

                *inserting = false;
                return;
            }

        }

        *internal = true; //redundant

        // return here as we do not need to progress further on this internal layer.
        return;
    }

    else { // if there are no insertion to be done at the internal layers, then just progress by returning
        return;
    }
};

void BPTree::deleteKey(int x) {
    if (root == NULL) {
        cout << "Tree is empty\n";
        return;
    } else {
        // modified search function to see if key exists
        Node *cursor = root;
        Node *parent;
        int leftIndex, rightIndex;
        while (cursor->isLeaf == false) {
            for (int i=0; i<cursor->size; i++) {
                // keeps track of pointers
                parent = cursor;
                leftIndex = i - 1; 
                rightIndex = i + 1;
                if (x < cursor->keys[i]) {
                    cursor = cursor->ptrs[i]; // moving down the left branch of internal node
                    break;
                }
                if (i == cursor->size-1) {
                    leftIndex = i; 
                    rightIndex = i + 2; 
                    cursor = cursor->ptrs[i+1]; // moving down the right branch of internal node
                    break;
                }
            }
        }
        bool found = false;
        int pos;
        for (pos=0; pos<cursor->size; pos++) {
            if (cursor->keys[pos] == x) {
                found = true; // key found
                break;
            }
        }
        if (!found) {
            cout << "Key to be deleted not found\n";
            return;
        }

        // delete function
        for (int i=pos; i<cursor->size; i++) {
            // moving key[i+1] to key[i] -> right to left
            cursor->keys[i] = cursor->keys[i+1];
            // cursor->blocks[i] = cursor->blocks[i+1];
        }

        Record *follow = cursor->blocks[pos];
        for (int i=0; i<max_rec; i++) {
            Record *r = &follow[i];

            if (r->numVotes == x) {
                r->deleted = true;
            }
        }
        
        cursor->size--;

        // deletion at root node
        if (cursor == root) {
            // for (int i=0; i<N+1; i++) {
            //     cursor->ptr[i] = NULL;
            // }
            cursor->ptrs[0] = NULL;
            if (cursor->size == 0) {
                cout << "Tree no longer exists\n";
                // delete[] cursor->key;
                // delete[] cursor->ptr;
                // delete cursor;
                root = NULL;
            }
            return;
        }

        // update pointer that is pointing to next leaf node
        // cursor->ptr[cursor->size] = cursor->ptr[cursor->size+1];
        // cursor->ptr[cursor->size+1] = NULL;

        // case 1: easy case
        // check if current leaf node satisfy (n+1)/2
        if (cursor->size >= (N+1)/2) {
            return;
        }

        // case 2a: borrow key from left sibling node
        if (leftIndex >= 0) { // check if left node exists
            Node *leftNode = parent->ptrs[leftIndex];
            if (leftNode->size-1 >= (N+1)/2) { // check if left node has extra keys to lend
                // moving key[i-1] to key[i] -> left to right
                for (int i=cursor->size; i>0; i--) {
                    cursor->keys[i] = cursor->keys[i-1];
                }
                cursor->size++;

                // update pointer that is pointing to next leaf node
                // cursor->ptr[cursor->size] = cursor->ptr[cursor->size-1];
                // cursor->ptr[cursor->size-1] = NULL;
                // update key from left leaf node
                cursor->keys[0] = leftNode->keys[leftNode->size-1];

                leftNode->size--;
                // update pointer that is pointing to next leaf node
                // leftNode->ptr[leftNode->size] = cursor;
                // leftNode->ptr[leftNode->size+1] = NULL;

                // update parent node
                parent->keys[leftIndex] = cursor->keys[0];
                return;
            }
        }
        
        // case 2b: borrow key from right sibling node
        if (rightIndex <= parent->size) { // check if right node exists
            Node *rightNode = parent->ptrs[rightIndex];
            if (rightNode->size-1 >= (N+1)/2) { // check if right node has extra keys to lend
                cursor->size++;
                // update pointer that is pointing to next leaf node
                // cursor->ptr[cursor->size] = cursor->ptr[cursor->size-1]; 
                // cursor->ptr[cursor->size-1] = NULL;
                // update key from right leaf node
                cursor->keys[cursor->size-1] = rightNode->keys[0];

                rightNode->size--;
                // moving key[i+1] to key[i] -> right to left
                for (int i=0; i<rightNode->size; i++) {
                    rightNode->keys[i] = rightNode->keys[i+1];
                }
                
                // update pointer that is pointing to next leaf node
                // rightNode->ptr[rightNode->size] = rightNode->ptr[rightNode->size+1];
                // rightNode->ptr[rightNode->size+1] = NULL;

                // update parent node
                parent->keys[rightIndex-1] = rightNode->keys[0];

                return;
            }
        }

        // case 3: not possible to borrow keys from a sibling node
        if (leftIndex >= 0) { // check if left node exists
            Node *leftNode = parent->ptrs[leftIndex];
            
            // merge current node into left node
            for (int i=leftNode->size, j=0; j<cursor->size; i++, j++) {
                leftNode->keys[i] = cursor->keys[j];
            }

            // update pointer that is pointing to next leaf node
            // leftNode->ptr[leftNode->size] = NULL;
            leftNode->size += cursor->size;
            // leftNode->ptr[leftNode->size] = cursor->ptr[cursor->size];
            leftNode->ptrs[0] = cursor->ptrs[0];
            deleteInternalNode(parent->keys[leftIndex], parent, cursor);
            
        } else if (rightIndex <= parent->size) { // check if right node exists
            Node *rightNode = parent->ptrs[rightIndex];
            
            // merge right node into current node
            for (int i=cursor->size, j=0; j<rightNode->size; i++, j++) {
                cursor->keys[i] = rightNode->keys[j];
            }

            // update pointer that is pointing to next leaf node
            // cursor->ptr[cursor->size] = NULL;
            cursor->size += rightNode->size;
            // cursor->ptr[cursor->size] = rightNode->ptr[rightNode->size];
            cursor->ptrs[0] = rightNode->ptrs[0];
            deleteInternalNode(parent->keys[rightIndex-1], parent, rightNode);
        }
    }
}

Node *BPTree::findParent(Node *cursor, Node* child) {
    // checking all branches in the B+ tree to find child node's parent 
    Node *parent;

    // checking current node != root node
    if (cursor->isLeaf || cursor->ptrs[0]->isLeaf) { 
        return NULL;
    }
    for (int i=0; i<cursor->size+1; i++) {
        if (cursor->ptrs[i] == child) {
            parent = cursor;
            return parent;
        } else {
            // recursive call on findParent function
            parent = findParent(cursor->ptrs[i], child);
            if (parent != NULL) {
                return parent;
            }
        }
    }
    return parent;
}

void BPTree::deleteInternalNode(int parentKey, Node *cursor, Node *child) {
    if (cursor == root) {
        if (cursor->size == 1) {
            if (cursor->ptrs[0] == child) { // check if it is left node
                // delete left node
                // delete[] child->keys;
                // delete[] child->ptrs;
                // delete child;
                // change root node
                root = cursor->ptrs[1];
                // delete[] cursor->keys;
                // delete[] cursor->ptrs;
                // delete cursor;
                return;
            } else if (cursor->ptrs[1] == child) { // check if it is right node
                 // delete right node
                // delete[] child->keys;
                // delete[] child->ptrs;
                // delete child;
                // change root node
                root = cursor->ptrs[0];
                // delete[] cursor->keys;
                // delete[] cursor->ptrs;
                // delete cursor;
                return;
            }
        }
    }

    // current node != root node
    // finding key index in current node
    int index;
    for (index=0; index<cursor->size; index++) {
        if (cursor->keys[index] == parentKey) {
            break;
        }
    }

    // update keys in current node
    for (int i=index; i<cursor->size; i++) {
        // replace key[i] by key[i+1]
        cursor->keys[i] = cursor->keys[i+1];
    }

    // finding ptr index in current node
    for (index=0; index<cursor->size+1; index++) {
        if (cursor->ptrs[index] == child) {
            break;
        }
    }

    // update ptr in current node
    for (int i=index; i<cursor->size+1; i++) {
        cursor->ptrs[i] = cursor->ptrs[i+1];
    }

    cursor->size--;
    // check if current node meets the node size (N/2) requirement
    if (cursor->size >= N/2) {
        return;
    }
    
    // break recursive call once current node == root node
    if (cursor == root) {
        return;
    }
    
    // find parent of current node
    Node *parent = findParent(root, cursor);

    // find left and right nodes from the current node
    int leftIndex, rightIndex;
    for (index=0; index<parent->size+1; index++) {
        if (parent->ptrs[index] == cursor) {
            leftIndex = index - 1;
            rightIndex = index + 1;
            break;
        }
    }
    
    // modified cases 2a and 2b for internal nodes
    if (leftIndex >= 0) { // check if left node exists
        Node *leftNode = parent->ptrs[leftIndex];
        if (leftNode->size >= (N+1)/2) { // check if left node meets size requirement
            // updating keys
            for (int i=cursor->size; i>0; i--) {
                cursor->keys[i] = cursor->keys[i-1];
            }
            cursor->keys[0] = parent->keys[leftIndex];
            parent->keys[leftIndex] = leftNode->keys[leftNode->size-1];

            // updating ptrs
            for (int i=cursor->size+1; i>0; i--) {
                cursor->ptrs[i] = cursor->ptrs[i-1];
            }
            cursor->ptrs[0] = leftNode->ptrs[leftNode->size];
            // cursor->ptr[0] = leftNode->ptr[0];
            cursor->size++;
            leftNode->size--;
            return;
        }
    }
    if (rightIndex <= parent->size) { // check if right node exists
        Node *rightNode = parent->ptrs[rightIndex]; 
        if (rightNode->size >= (N+1)/2) { // check if right node meets size requirement
            // updating keys
            cursor->keys[cursor->size] = parent->keys[index];
            parent->keys[index] = rightNode->keys[0];
            for (int i=0; i<rightNode->size-1; i++) {
                rightNode->keys[i] = rightNode->keys[i+1];
            }

            // updating ptrs
            cursor->ptrs[cursor->size+1] = rightNode->ptrs[0];
            // cursor->ptr[0] = rightNode->ptr[0];
            for (int i=0; i<rightNode->size+1; i++) {
                rightNode->ptrs[i] = rightNode->ptrs[i+1];
            }

            cursor->size++;
            rightNode->size--;
            return;
        }
    }

    // modified case 3 for internal nodes
    if (leftIndex >= 0) { // check if left node exists
        Node *leftNode = parent->ptrs[leftIndex];
        
        // merging current node into left node
        // updating keys
        leftNode->keys[leftNode->size] = parent->keys[leftIndex];
        for (int i=leftNode->size+1, j=0; j<cursor->size; j++) {
            // moving keys from current node to left node
            leftNode->keys[i] = cursor->keys[j];
        }

        // updating ptrs
        for (int i=leftNode->size+1, j=0; j<cursor->size+1; j++) {
            // moving ptrs from current node to left node
            leftNode->ptrs[i] = cursor->ptrs[j];
            // update ptrs in current node
            cursor->ptrs[j] = NULL;
        }

        leftNode->size += cursor->size + 1;
        cursor->size = 0;

        // recursive call
        deleteInternalNode(parent->keys[leftIndex], parent, cursor);
    } else if (rightIndex <= parent->size) { // check if right node exists
        Node *rightNode = parent->ptrs[rightIndex];

        // merging right node into current node
        // updating keys
        cursor->keys[cursor->size] = parent->keys[rightIndex-1];
        for (int i=cursor->size+1, j=0; j<rightNode->size; j++) {
            // moving keys from right node to current node
            cursor->keys[i] = rightNode->keys[j];
        }

        // updating ptrs
        for (int i=cursor->size+1, j=0; j<rightNode->size+1; j++) {
            // moving ptrs from right node to current node
            cursor->ptrs[i] = rightNode->ptrs[j]; 
            // update ptrs in right node
            rightNode->ptrs[j] = NULL;
        }

        cursor->size += rightNode->size + 1;
        rightNode->size = 0;

        // recursive call
        deleteInternalNode(parent->keys[rightIndex-1], parent, rightNode);
    }
};