#include <stdlib.h>
#include <iostream>
#include <cmath>
#include "pc_disk_storage.h"

using namespace std;

const int maxNumRecords = floor(blockSize/sizeof(Record));
const int n = floor((blockSize - sizeof(int) - sizeof(int) - sizeof(int))/(sizeof(int) + sizeof(void *) + sizeof(void*)));


struct recordResults {
    Record record;
    recordResults * nextRecord = NULL;

};

class BPlusNode {

    public: 
        bool isLeaf; 
        BPlusNode *ptrs[n + 1]; 

        int keys[n], size; 
        Record *blocks[n]; 

        BPlusNode(bool);
};

BPlusNode::BPlusNode(bool toLeaf) {
    isLeaf = toLeaf;
    size = 0;

    if (toLeaf == true) 
        ptrs[0] = NULL;

};

class BPlusTree {

    private:
        void deleteInternalNode(int, BPlusNode *, BPlusNode *);
        BPlusNode *getTheParent(BPlusNode *, BPlusNode *); 
        void _insert(int *, Record *, BPlusNode *, BPlusNode**, bool *, bool *); 
            

    public:
        BPlusNode *rootOfTree;
        BPlusTree();
        void showRootNodes(BPlusNode *);
        std::pair<int, int> findNumVotes(int, recordResults **);
        std::pair<int, int> findNumVotes(int, int, recordResults **);    
        BPlusNode * getRoot();
        int getLevelOfTree();
        void insert(int, Record *);
        void deleteKey(int);
        int NumNodesTree(BPlusNode *, int &);
};

BPlusTree::BPlusTree() {
    rootOfTree = NULL;
};

std::pair<int, int> BPlusTree::findNumVotes(int x, recordResults ** results) {
    int nodesNumAccess = 0;
    int blocksNumAccess = 0;

    if (rootOfTree == NULL) {
        cout << "This B plus tree is empty! \n";
        return std::make_pair(nodesNumAccess, blocksNumAccess);

    } else {

        BPlusNode *current = rootOfTree;
        while (current->isLeaf == false) { 
            nodesNumAccess++;

            for (int a=0; a<current->size; a++) {
                if (x < current->keys[a]) {
                    current = current->ptrs[a]; 
                    break;
                }
                if (a == current->size-1) {
                    current = current->ptrs[a+1];
                    break;
                }
            }
        }

        nodesNumAccess++;
        recordResults ** temp = results;

        for (int a=0; a<current->size; a++) {

            if (x == current->keys[a]) {

                bool foundlast = false;
                Record * recordFollow = current->blocks[a];

                while(foundlast == false){
                    if (blocksNumAccess < 5){
                        Record * blk = recordFollow;
                        for (int j = 0; j < maxNumRecords; j++){
                            Record * r = &recordFollow[j];
                        }
                    }
                    blocksNumAccess++;

                    for (int a = 0; a < maxNumRecords; a++){
                        Record * r = &recordFollow[a];

                        if (r->numVotes == x){
                            
                            *temp = new recordResults;
                            (*temp)->record = *r;
                            temp = &((*temp)->nextRecord);

                            if (r->nextDuplicate == 0){
                                foundlast = true;
                                break;
                            }

                            else if (r->nextDuplicate == recordFollow){
                                continue;
                            }

                            else {
                                recordFollow = r->nextDuplicate;
                                break;
                            }
                            
                        }
                    }
                }
                return std::make_pair(nodesNumAccess, blocksNumAccess);
            }
        }
        cout << "The Key is not found\n";
        return std::make_pair(nodesNumAccess, blocksNumAccess);
    }
};


std::pair<int, int> BPlusTree::findNumVotes(int min, int max, recordResults ** results) {
    int nodesNumAccess = 0;
    int blocksNumAccess = 0;

    if (rootOfTree == NULL) {
        cout << "Tree is empty\n";
        return std::make_pair(nodesNumAccess, blocksNumAccess);

    } else {

        BPlusNode *current = rootOfTree;
        while (current->isLeaf == false) { 
            nodesNumAccess++;

            for (int a=0; a<current->size; a++) {
                if (min < current->keys[a]) {
                    current = current->ptrs[a]; 
                    break;
                }
                if (a == current->size-1) {
                    current = current->ptrs[a+1]; 
                    break;
                }
            }
        }
        nodesNumAccess++;

        bool endFind = false;
        recordResults ** temp = results;

        while (endFind == false){
        
            for (int a=0; a<current->size; a++) {

                if (min <= current->keys[a] && max >= current->keys[a]) {
                    int x = current->keys[a];
                    bool foundlast = false; 
                    Record * follow = current->blocks[a];

                    while(foundlast == false){
                        if (blocksNumAccess < 5){
                            Record * blk = follow;
                            for (int j = 0; j < maxNumRecords; j++){
                                Record * r = &follow[j];
                            }
                        }
                        blocksNumAccess++;
                        if (blocksNumAccess == 6) {
                        }

                        for (int a = 0; a < maxNumRecords; a++){
                            Record * r = &follow[a];

                            if (r->numVotes == x){
                                *temp = new recordResults;
                                (*temp)->record = *r;

                                temp = &((*temp)->nextRecord);

                                if (r->nextDuplicate == 0){
                                    foundlast = true;
                                    break;
                                }

                                else if (r->nextDuplicate == follow){
                                    continue;
                                }

                                else {
                                    follow = r->nextDuplicate;
                                    break;
                                }
                                
                            }
                        }
                    }
                } else if (max < current->keys[a]) {
                    endFind = true;
                    break;
                }
            }
            if (current->ptrs[0] != NULL) current = current->ptrs[0];
            else endFind = true;

        }
        return std::make_pair(nodesNumAccess, blocksNumAccess);;

    }
};

int BPlusTree::getLevelOfTree() {
    int level = 1;
    
    if (rootOfTree == NULL) {
        cout << "The Tree is empty\n";
        return 0;
    } else {
        BPlusNode *current = rootOfTree;
        while (current->isLeaf == false) {
            current = current->ptrs[0];
            level++;
        }
    }
    return level;
}

int BPlusTree::NumNodesTree(BPlusNode *current, int &numNodes) {
    if (current != NULL) {
        numNodes++;
        if (current->isLeaf == false) { 
            for (int i=0; i<current->size+1; i++) { 
                NumNodesTree(current->ptrs[i], numNodes); 
            }
        }
    }
    return numNodes;
}

void BPlusTree::showRootNodes(BPlusNode *current) {
    cout << "The root nodes are: ";

    if (current != NULL) {
        for (int i=0; i<current->size; i++) {
            cout << current->keys[i] << " ";
        }
        cout << "\n";
    }
}

BPlusNode *BPlusTree::getRoot() {
    return rootOfTree;
}

void BPlusTree::insert(int key, Record * dataBlock) {
    int * newKey = &key;
    
    BPlusNode * tmpPtr = NULL;
    BPlusNode ** newPtr = &tmpPtr;
    bool internalNode = false; 
    bool insertingNode = true;

    bool * internalN = &internalNode;
    bool * insertingN = &insertingNode;

    _insert(newKey, dataBlock, rootOfTree, newPtr, internalN, insertingN);

}

void BPlusTree::_insert(int * newKey, Record * block, BPlusNode * curr, BPlusNode ** newPtr, bool * internalN, bool * insertingN) {
    if (rootOfTree == NULL) {
        rootOfTree = new BPlusNode(true);

        rootOfTree -> blocks[0] = block;
        rootOfTree -> keys[0] = *newKey;
        rootOfTree -> size = 1;
        return; 
    }
    if (curr->isLeaf == false && *internalN == false){

        BPlusNode * current = curr;
        BPlusNode ** currentPtr = current->ptrs;
        int * currentKey = current->keys;

        for (int a = 0; a < current -> size; a++) {
            if (*newKey < *currentKey){
                current = *currentPtr;
                break;
            }
            currentKey++;
            currentPtr++;
            if (a == current->size - 1) { 
                current = *currentPtr;
                break;
            }
        }
        _insert(newKey, block, current, newPtr, internalN, insertingN);
    }

    if (*insertingN == true && *internalN == false) {
        BPlusNode * current = curr;
        int * currentKey = current->keys;
        Record ** currentBlock = current->blocks;
        Record * insertBlock = block;
        int insertKey = *newKey;

        for (int a = 0; a < curr -> size; a++, currentKey++, currentBlock++) {
            if (insertKey < *currentKey){ 
                int tmpKey = *currentKey;
                Record * temp_block = *currentBlock;
                *currentKey = insertKey;
                *currentBlock = insertBlock;
                insertKey = tmpKey;
                insertBlock = temp_block;
            }

            else if (insertKey == *currentKey){ 

                *insertingN = false;
                Record * blockFollow = *currentBlock;
                bool foundlast = false; 

                while (foundlast == false){
                    int a;
                    for (a = 0; a < maxNumRecords; a++){ 
                        Record * r = &blockFollow[a];

                        if (insertKey == r->numVotes){
                            if (r->nextDuplicate == 0){ 
                                foundlast = true;
                                r->nextDuplicate = insertBlock;
                                break;
                            }
                            else if (r->nextDuplicate == blockFollow){ 
                                continue;
                            }
                            else { 
                                blockFollow = r->nextDuplicate;
                                break;
                            }
                        }
                    }
                    if (a == maxNumRecords) 
                        foundlast = true;
                }
                return;
            }
        }

        if (curr->size < n){ 
            *currentKey = insertKey;
            *currentBlock = insertBlock;
            current->size++;
            *insertingN = false;
        }
        else {
            int halfMin = ceil((float) n/2); 
            BPlusNode * newNode = new BPlusNode(true);

            for (int i = halfMin, j = 0; i<n; i++, j++) {
                newNode->keys[j] = current->keys[i];
                newNode->blocks[j] = current->blocks[i];
                newNode->size++;
                current->size--;
            }

            int lastMin = floor((float) n/2);
            newNode-> keys[lastMin] = insertKey;
            newNode-> blocks[lastMin] = insertBlock;
            newNode->size++;
            newNode->ptrs[0] = current->ptrs[0];
            current->ptrs[0] = newNode;

            *newKey = newNode->keys[0]; 
            *newPtr = newNode; 
            *insertingN = true; 

            if (rootOfTree == curr){
                BPlusNode * newroot = new BPlusNode(false);
                newroot->size = 1;
                newroot->keys[0] = *newKey;
                newroot->ptrs[0] = curr;
                newroot->ptrs[1] = newNode;
                newroot->isLeaf = false;

                rootOfTree = newroot;
            }
            

        }
        *internalN = true;

        return;
    }
    

    if (*insertingN == true && *internalN == true){
        BPlusNode * current = curr;
        int * currentKey = current->keys;
        BPlusNode ** currentPtr = current->ptrs;

        currentPtr++; 
        int keytoinsert = *newKey;
        BPlusNode * ptrtoinsert = *newPtr;

        for (int i = 0; i < curr -> size; i++, currentKey++, currentPtr++) {
            if (keytoinsert < *currentKey){
                int temp = *currentKey;
                BPlusNode * temp_ptr = *currentPtr;
                *currentKey = keytoinsert;
                *currentPtr = ptrtoinsert;
                keytoinsert = temp;
                ptrtoinsert = temp_ptr;
            }
        }
        if (current->size < n){
            *currentKey = keytoinsert;
            *currentPtr = ptrtoinsert;
            *insertingN = false;
            current->size++;
        }
        else {
            int halfMin = ceil((float) n/2);
            BPlusNode * newNode = new BPlusNode(false);

            *newKey = current->keys[halfMin]; 
            current->size--;

            for (int i = halfMin, j = 0; i<n; i++, j++){
                if (i + 1 < n){
                    newNode->keys[j] = current->keys[i+1];
                    current->size--;
                    newNode->size++;
                }
                newNode->ptrs[j] = current->ptrs[i+1];

            }

            int lastMin = floor((float) n/2);
            newNode -> keys[lastMin-1] = keytoinsert; 
            newNode -> ptrs[lastMin] = ptrtoinsert;
            *insertingN = true;
            newNode -> size++; 

            *newPtr = newNode;
            if (rootOfTree == curr){
                BPlusNode * newroot = new BPlusNode(false);
                newroot->size = 1;
                newroot->keys[0] = *newKey;
                newroot->ptrs[0] = curr;
                newroot->ptrs[1] = *newPtr;
                newroot->isLeaf = false;
                *insertingN = false;
                rootOfTree = newroot;
                return;
            }
        }
        *internalN = true;
        return;
    }
    else { 
        return;
    }
};

void BPlusTree::deleteKey(int y) {
    if (rootOfTree == NULL) {
        cout << "Tree is empty\n";
        return;
    } else {
        BPlusNode *parentOfCurrent;
        BPlusNode *current = rootOfTree;
        int indexLeft, indexRight;

        while (current->isLeaf == false) {
            for (int a=0; a<current->size; a++) {
                parentOfCurrent = current;
                indexRight = a + 1;
                indexLeft = a - 1; 
                if (y < current->keys[a]) {
                    current = current->ptrs[a]; 
                    break;
                }
                if (a == current->size-1) {
                    indexLeft = a; 
                    indexRight = a + 2; 
                    current = current->ptrs[a+1];
                    break;
                }
            }
        }
        bool foundLast = false;
        int position;
        for (position=0; position<current->size; position++) {
            if (current->keys[position] == y) {
                foundLast = true; 
                break;
            }
        }
        if (!foundLast) {
            cout << "Key is not found\n";
            return;
        }
        for (int a=position; a<current->size; a++) {
            current->keys[a] = current->keys[a+1];
        }

        Record *followRecord = current->blocks[position];
        for (int i=0; i<maxNumRecords; i++) {
            Record *rec = &followRecord[i];

            if (rec->numVotes == y) {
                rec->deleted = true;
            }
        }
        current->size--;

        if (current == rootOfTree) {
            current->ptrs[0] = NULL;
            if (current->size == 0) {
                cout << "BPlus tree doesn't exist\n";
                rootOfTree = NULL;
            }
            return;
        }
        if (current->size >= (n+1)/2) {
            return;
        }

        if (indexLeft >= 0) { 
            BPlusNode *nodeLeft = parentOfCurrent->ptrs[indexLeft];
            if (nodeLeft->size-1 >= (n+1)/2) { 
                for (int i=current->size; i>0; i--) {
                    current->keys[i] = current->keys[i-1];
                }
                current->size++;
                current->keys[0] = nodeLeft->keys[nodeLeft->size-1];
                nodeLeft->size--;
                parentOfCurrent->keys[indexLeft] = current->keys[0];
                return;
            }
        }
        
        if (indexRight <= parentOfCurrent->size) { 
            BPlusNode *nodeRight = parentOfCurrent->ptrs[indexRight];
            if (nodeRight->size-1 >= (n+1)/2) { 
                current->size++;
                current->keys[current->size-1] = nodeRight->keys[0];
                nodeRight->size--;
                for (int i=0; i<nodeRight->size; i++) {
                    nodeRight->keys[i] = nodeRight->keys[i+1];
                }
                parentOfCurrent->keys[indexRight-1] = nodeRight->keys[0];
                return;
            }
        }

        if (indexLeft >= 0) {
            BPlusNode *leftNode = parentOfCurrent->ptrs[indexLeft];
            for (int i=leftNode->size, j=0; j<current->size; i++, j++) {
                leftNode->keys[i] = current->keys[j];
            }
            leftNode->size += current->size;
            leftNode->ptrs[0] = current->ptrs[0];
            deleteInternalNode(parentOfCurrent->keys[indexLeft], parentOfCurrent, current);
            
        } else if (indexRight <= parentOfCurrent->size) {
            BPlusNode *rightNode = parentOfCurrent->ptrs[indexRight];
            
            for (int a=current->size, j=0; j<rightNode->size; a++, j++) {
                current->keys[a] = rightNode->keys[j];
            }
            current->size += rightNode->size;
            current->ptrs[0] = rightNode->ptrs[0];
            deleteInternalNode(parentOfCurrent->keys[indexRight-1], parentOfCurrent, rightNode);
        }
    }
}

BPlusNode *BPlusTree::getTheParent(BPlusNode *current, BPlusNode* child) {
    BPlusNode *parentNode;
    if (current->isLeaf || current->ptrs[0]->isLeaf) { 
        return NULL;
    }
    for (int a=0; a<current->size+1; a++) {
        if (current->ptrs[a] != child) {
            parentNode = getTheParent(current->ptrs[a], child);
            if (parentNode != NULL) {
                return parentNode;
            }
        } else {
            parentNode = current;
            return parentNode;
        }
    }
    return parentNode;
}

void BPlusTree::deleteInternalNode(int keyParent, BPlusNode *current, BPlusNode *child) {
    if (current == rootOfTree) {
        if (current->size == 1) {
            if (current->ptrs[1] == child) { 
                rootOfTree = current->ptrs[0];
                return;
            } else if (current->ptrs[0] == child) { 
                rootOfTree = current->ptrs[1];
                return;
            }
        }
    }
    int ind;
    for (ind=0; ind<current->size; ind++) {
        if (current->keys[ind] == keyParent) {
            break;
        }
    }
    for (int i=ind; i<current->size; i++) {
        current->keys[i] = current->keys[i+1];
    }

    for (ind=0; ind<current->size+1; ind++) {
        if (current->ptrs[ind] == child) {
            break;
        }
    }
    for (int i=ind; i<current->size+1; i++) {
        current->ptrs[i] = current->ptrs[i+1];
    }
    current->size--;
    if (current->size >= n/2) {
        return;
    }
    if (current == rootOfTree) {
        return;
    }

    BPlusNode *parentNode = getTheParent(rootOfTree, current);

    int indexLeft, indexRight;
    for (ind=0; ind<parentNode->size+1; ind++) {
        if (parentNode->ptrs[ind] == current) {
            indexRight = ind + 1;
            indexLeft = ind - 1;
            break;
        }
    }

    if (indexLeft >= 0) { 
        BPlusNode *leftNode = parentNode->ptrs[indexLeft];
        if (leftNode->size >= (n+1)/2) { 
            for (int a=current->size; a>0; a--) {
                current->keys[a] = current->keys[a-1];
            }
            current->keys[0] = parentNode->keys[indexLeft];
            parentNode->keys[indexLeft] = leftNode->keys[leftNode->size-1];

            for (int a=current->size+1; a>0; a--) {
                current->ptrs[a] = current->ptrs[a-1];
            }
            current->ptrs[0] = leftNode->ptrs[leftNode->size];
            current->size++;
            leftNode->size--;
            return;
        }
    }
    if (indexRight <= parentNode->size) { 
        BPlusNode *rightNode = parentNode->ptrs[indexRight]; 
        if (rightNode->size >= (n+1)/2) { 
            current->keys[current->size] = parentNode->keys[ind];
            parentNode->keys[ind] = rightNode->keys[0];
            for (int a=0; a<rightNode->size-1; a++) {
                rightNode->keys[a] = rightNode->keys[a+1];
            }

            current->ptrs[current->size+1] = rightNode->ptrs[0];
            for (int a=0; a<rightNode->size+1; a++) {
                rightNode->ptrs[a] = rightNode->ptrs[a+1];
            }

            current->size++;
            rightNode->size--;
            return;
        }
    }

    if (indexRight <= parentNode->size) {
        BPlusNode *rightNode = parentNode->ptrs[indexRight];

        current->keys[current->size] = parentNode->keys[indexRight-1];
        for (int i=current->size+1, j=0; j<rightNode->size; j++) {
            current->keys[i] = rightNode->keys[j];
        }

        for (int i=current->size+1, j=0; j<rightNode->size+1; j++) {
            current->ptrs[i] = rightNode->ptrs[j]; 
            rightNode->ptrs[j] = NULL;
        }

        current->size += rightNode->size + 1;
        rightNode->size = 0;

        deleteInternalNode(parentNode->keys[indexRight-1], parentNode, rightNode);

    } else if (indexLeft >= 0) { 

        BPlusNode *leftNode = parentNode->ptrs[indexLeft];
        leftNode->keys[leftNode->size] = parentNode->keys[indexLeft];
        for (int a=leftNode->size+1, j=0; j<current->size; j++) {
            leftNode->keys[a] = current->keys[j];
        }
        for (int a=leftNode->size+1, j=0; j<current->size+1; j++) {
            leftNode->ptrs[a] = current->ptrs[j];
            current->ptrs[j] = NULL;
        }

        leftNode->size += current->size + 1;
        current->size = 0;
        deleteInternalNode(parentNode->keys[indexLeft], parentNode, current);
    }
};