#include <stdlib.h>
#include <iostream>
#include <cmath>
#include "pc_disk_storage.h"

using namespace std;

const int maxNumRecords = floor(sizeOfBlock/sizeof(Record));
const int n = floor((sizeOfBlock - sizeof(int) - sizeof(int) - sizeof(void*))/(sizeof(void*) + sizeof(int) + sizeof(void *)));


struct recordResults {
    Record record;
    recordResults * nextRecord = NULL;

};

class BPlusNode {
    public: 
        bool isLeaf; 
        BPlusNode *ptrs[n + 1]; 

        int keys[n], sizeNode; 
        Record *dataBlocks[n]; 

        BPlusNode(bool);
};

BPlusNode::BPlusNode(bool toLeaf) {
    isLeaf = toLeaf;
    sizeNode = 0;

    if (toLeaf == true) 
        ptrs[0] = NULL;

};

class BPlusTree {

    private:
        BPlusNode *getTheParent(BPlusNode *, BPlusNode *); 
        void insertv2(int *, Record *, BPlusNode *, BPlusNode**, bool *, bool *);
        void deleteInternal(int, BPlusNode *, BPlusNode *);
            

    public:
        BPlusNode *rootOfTree;
        BPlusTree();
        void showRootNodes(BPlusNode *);
        std::pair<int, int> findNumVotes(int, recordResults **);
        std::pair<int, int> findNumVotes(int, int, recordResults **);    
        BPlusNode * getRoot();
        int getLevelOfTree();
        void insertv1(int, Record *);
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

    }

    nodesNumAccess = 1;
    BPlusNode *current = rootOfTree;
    while (current->isLeaf == false) { 
        nodesNumAccess++;

        for (int a=0; a<current->sizeNode; a++) {
            if (x < current->keys[a]) {
                current = current->ptrs[a]; 
                break;
            }
            if (a == current->sizeNode-1) {
                current = current->ptrs[a+1];
                break;
            }
        }
    }

    recordResults ** temp = results;

    for (int a=0; a<current->sizeNode; a++) {

        if (x == current->keys[a]) {

            bool foundlast = false;
            Record * recordFollow = current->dataBlocks[a];

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
    cout << "The Key is not found" << endl;
    return std::make_pair(nodesNumAccess, blocksNumAccess);
    
};


std::pair<int, int> BPlusTree::findNumVotes(int min, int max, recordResults ** results) {
    int nodesNumAccess = 0;
    int blocksNumAccess = 0;

    if (rootOfTree == NULL) {
        cout << "Tree is empty" << endl;
        return std::make_pair(nodesNumAccess, blocksNumAccess);

    } else {

        BPlusNode *current = rootOfTree;
        while (current->isLeaf == false) { 
            nodesNumAccess++;

            for (int a=0; a<current->sizeNode; a++) {
                if (min < current->keys[a]) {
                    current = current->ptrs[a]; 
                    break;
                }
                if (a == current->sizeNode-1) {
                    current = current->ptrs[a+1]; 
                    break;
                }
            }
        }
        nodesNumAccess++;

        bool endFind = false;
        recordResults ** temp = results;

        while (endFind == false){
        
            for (int a=0; a<current->sizeNode; a++) {

                if (min <= current->keys[a] && max >= current->keys[a]) {
                    int x = current->keys[a];
                    bool foundlast = false; 
                    Record * follow = current->dataBlocks[a];

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
    if (rootOfTree == NULL) {
        cout << "The Tree is empty\n";
        return 0;
    }

    int level = 1;
    BPlusNode *current = rootOfTree;
    while (!current->isLeaf) {
        current = current->ptrs[0];
        level++;
    }
    return level;
}

int BPlusTree::NumNodesTree(BPlusNode *current, int &numNodes) {
    if (current != NULL) {
        numNodes++;
        if (current->isLeaf == false) { 
            for (int i=0; i<current->sizeNode+1; i++) { 
                NumNodesTree(current->ptrs[i], numNodes); 
            }
        }
    }
    return numNodes;
}

void BPlusTree::showRootNodes(BPlusNode *current) {
    cout << "The root nodes are: ";

    if (current != NULL) {
        for (int i=0; i<current->sizeNode; i++) {
            cout << current->keys[i] << " ";
        }
        cout << endl;
    }
}

BPlusNode *BPlusTree::getRoot() {
    return rootOfTree;
}

void BPlusTree::insertv1(int key, Record * dataBlock) {
    int * newKey = &key;
    
    BPlusNode * tmpPtr = NULL;
    BPlusNode ** newPtr = &tmpPtr;
    bool internalNode = false; 
    bool insertingNode = true;

    bool * insertingN = &insertingNode;
    bool * internalN = &internalNode;

    insertv2(newKey, dataBlock, rootOfTree, newPtr, internalN, insertingN);

}

void BPlusTree::insertv2(int * newKey, Record * block, BPlusNode * curr, BPlusNode ** newPtr, bool * internalN, bool * insertingN) {
    if (rootOfTree == NULL) {
        rootOfTree = new BPlusNode(true);
        rootOfTree -> sizeNode = 1;
        rootOfTree -> keys[0] = *newKey;
        rootOfTree -> dataBlocks[0] = block;
        return; 
    }
    if (curr->isLeaf == false && *internalN == false){

        BPlusNode * current = curr;
        BPlusNode ** currentPtr = current->ptrs;
        int * currentKey = current->keys;

        for (int a = 0; a < current -> sizeNode; a++) {
            if (*newKey < *currentKey){
                current = *currentPtr;
                break;
            }

            currentPtr++;
            currentKey++;

            if (a == current->sizeNode - 1) { 
                current = *currentPtr;
                break;
            }
        }
        insertv2(newKey, block, current, newPtr, internalN, insertingN);
    }

    if (*insertingN == true && *internalN == false) {
        BPlusNode * current = curr;
        int * currentKey = current->keys;
        Record ** currentBlock = current->dataBlocks;
        Record * insertBlock = block;
        int insertKey = *newKey;

        for (int a = 0; a < curr -> sizeNode; a++, currentKey++, currentBlock++) {
            if (insertKey == *currentKey){ 
                
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

            else if (insertKey < *currentKey ){ 
                int tmpKey = *currentKey;
                Record * temp_block = *currentBlock;
                *currentKey = insertKey;
                insertKey = tmpKey;
                *currentBlock = insertBlock;
                insertBlock = temp_block;

            }
        }

        if (curr->sizeNode < n){ 
            *currentBlock = insertBlock;
            *currentKey = insertKey;

            current->sizeNode++;
            *insertingN = false;
        }
        else {
            int halfMin = ceil((float) n/2); 
            BPlusNode * newNode = new BPlusNode(true);

            for (int i = halfMin, j = 0; i<n; i++, j++) {
                newNode->keys[j] = current->keys[i];
                newNode->dataBlocks[j] = current->dataBlocks[i];
                newNode->sizeNode++;
                current->sizeNode--;
            }

            int lastMin = floor((float) n/2);
            newNode-> keys[lastMin] = insertKey;
            newNode-> dataBlocks[lastMin] = insertBlock;
            newNode->sizeNode++;
            newNode->ptrs[0] = current->ptrs[0];
            current->ptrs[0] = newNode;

            *newKey = newNode->keys[0]; 
            *newPtr = newNode; 
            *insertingN = true; 

            if (rootOfTree == curr){
                BPlusNode * newroot = new BPlusNode(false);
                newroot->isLeaf = false;
                newroot->sizeNode = 1;
                newroot->ptrs[0] = curr;
                newroot->ptrs[1] = newNode;
                newroot->keys[0] = *newKey;
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

        for (int i = 0; i < curr -> sizeNode; i++, currentKey++, currentPtr++) {
            if (keytoinsert < *currentKey){
                int temp = *currentKey;
                BPlusNode * temp_ptr = *currentPtr;
                *currentKey = keytoinsert;
                *currentPtr = ptrtoinsert;
                keytoinsert = temp;
                ptrtoinsert = temp_ptr;
            }
        }
        if (current->sizeNode < n){
            *currentKey = keytoinsert;
            *currentPtr = ptrtoinsert;
            *insertingN = false;
            current->sizeNode++;
        }
        else {
            int halfMin = ceil((float) n/2);
            BPlusNode * newNode = new BPlusNode(false);

            *newKey = current->keys[halfMin]; 
            current->sizeNode--;

            for (int i = halfMin, j = 0; i<n; i++, j++){
                if (i + 1 < n){
                    newNode->keys[j] = current->keys[i+1];
                    current->sizeNode--;
                    newNode->sizeNode++;
                }
                newNode->ptrs[j] = current->ptrs[i+1];

            }

            int lastMin = floor((float) n/2);
            newNode -> keys[lastMin-1] = keytoinsert; 
            newNode -> ptrs[lastMin] = ptrtoinsert;
            *insertingN = true;
            newNode -> sizeNode++; 

            *newPtr = newNode;
            if (rootOfTree == curr){
                BPlusNode * newroot = new BPlusNode(false);
                newroot->sizeNode = 1;
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
        cout << "Tree is empty" << endl;
        return;
    } else {
        BPlusNode *parentOfCurrent;
        BPlusNode *current = rootOfTree;
        int indexLeft, indexRight;

        while (current->isLeaf == false) {
            for (int a=0; a<current->sizeNode; a++) {
                indexLeft = a - 1; 
                indexRight = a + 1;
                parentOfCurrent = current;

                if (y < current->keys[a]) {
                    current = current->ptrs[a]; 
                    break;
                }
                if (a == current->sizeNode-1) {
                    indexLeft = a; 
                    indexRight = a + 2; 
                    current = current->ptrs[a+1];
                    break;
                }
            }
        }
        bool foundLast = false;
        int position;
        for (position=0; position<current->sizeNode; position++) {
            if (current->keys[position] == y) {
                foundLast = true; 
                break;
            }
        }
        if (!foundLast) {
            cout << "Key is not found" << endl;
            return;
        }
        for (int a=position; a<current->sizeNode; a++) {
            current->keys[a] = current->keys[a+1];
        }

        Record *followRecord = current->dataBlocks[position];
        for (int i=0; i<maxNumRecords; i++) {
            Record *rec = &followRecord[i];

            if (rec->numVotes == y) {
                rec->deleted = true;
            }
        }
        current->sizeNode--;

        if (current == rootOfTree) {
            current->ptrs[0] = NULL;
            if (current->sizeNode == 0) {
                cout << "BPlus tree doesn't exist" << endl;
                rootOfTree = NULL;
            }
            return;
        }
        if (current->sizeNode >= (n+1)/2) {
            return;
        }

        if (indexLeft >= 0) { 
            BPlusNode *nodeLeft = parentOfCurrent->ptrs[indexLeft];
            if (nodeLeft->sizeNode-1 >= (n+1)/2) { 
                for (int i=current->sizeNode; i>0; i--) {
                    current->keys[i] = current->keys[i-1];
                }
                current->sizeNode++;
                current->keys[0] = nodeLeft->keys[nodeLeft->sizeNode-1];
                nodeLeft->sizeNode--;
                parentOfCurrent->keys[indexLeft] = current->keys[0];
                return;
            }
        }
        
        if (indexRight <= parentOfCurrent->sizeNode) { 
            BPlusNode *nodeRight = parentOfCurrent->ptrs[indexRight];
            if (nodeRight->sizeNode-1 >= (n+1)/2) { 
                current->sizeNode++;
                current->keys[current->sizeNode-1] = nodeRight->keys[0];
                nodeRight->sizeNode--;
                for (int i=0; i<nodeRight->sizeNode; i++) {
                    nodeRight->keys[i] = nodeRight->keys[i+1];
                }
                parentOfCurrent->keys[indexRight-1] = nodeRight->keys[0];
                return;
            }
        }

        if (indexLeft >= 0) {
            BPlusNode *leftNode = parentOfCurrent->ptrs[indexLeft];
            for (int i=leftNode->sizeNode, j=0; j<current->sizeNode; i++, j++) {
                leftNode->keys[i] = current->keys[j];
            }
            leftNode->sizeNode += current->sizeNode;
            leftNode->ptrs[0] = current->ptrs[0];
            deleteInternal(parentOfCurrent->keys[indexLeft], parentOfCurrent, current);
            
        } else if (indexRight <= parentOfCurrent->sizeNode) {
            BPlusNode *rightNode = parentOfCurrent->ptrs[indexRight];
            
            for (int a=current->sizeNode, j=0; j<rightNode->sizeNode; a++, j++) {
                current->keys[a] = rightNode->keys[j];
            }
            current->sizeNode += rightNode->sizeNode;
            current->ptrs[0] = rightNode->ptrs[0];
            deleteInternal(parentOfCurrent->keys[indexRight-1], parentOfCurrent, rightNode);
        }
    }
}

BPlusNode *BPlusTree::getTheParent(BPlusNode *current, BPlusNode* child) {
    BPlusNode *parentNode;
    if (current->isLeaf || current->ptrs[0]->isLeaf) { 
        return NULL;
    }
    for (int a=0; a<current->sizeNode+1; a++) {
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

void BPlusTree::deleteInternal(int keyParent, BPlusNode *current, BPlusNode *child) {
    if (current == rootOfTree) {
        if (current->sizeNode == 1) {
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
    for (ind=0; ind<current->sizeNode; ind++) {
        if (current->keys[ind] == keyParent) {
            break;
        }
    }
    for (int i=ind; i<current->sizeNode; i++) {
        current->keys[i] = current->keys[i+1];
    }

    for (ind=0; ind<current->sizeNode+1; ind++) {
        if (current->ptrs[ind] == child) {
            break;
        }
    }
    for (int i=ind; i<current->sizeNode+1; i++) {
        current->ptrs[i] = current->ptrs[i+1];
    }
    current->sizeNode--;
    if (current->sizeNode >= n/2) {
        return;
    }
    if (current == rootOfTree) {
        return;
    }

    BPlusNode *parentNode = getTheParent(rootOfTree, current);

    int indexLeft, indexRight;
    for (ind=0; ind<parentNode->sizeNode+1; ind++) {
        if (parentNode->ptrs[ind] == current) {
            indexLeft = ind - 1;
            indexRight = ind + 1;

            break;
        }
    }

    if (indexLeft >= 0) { 
        BPlusNode *leftNode = parentNode->ptrs[indexLeft];
        if (leftNode->sizeNode >= (n+1)/2) { 
            for (int a=current->sizeNode; a>0; a--) {
                current->keys[a] = current->keys[a-1];
            }
            current->keys[0] = parentNode->keys[indexLeft];
            parentNode->keys[indexLeft] = leftNode->keys[leftNode->sizeNode-1];

            for (int a=current->sizeNode+1; a>0; a--) {
                current->ptrs[a] = current->ptrs[a-1];
            }
            current->ptrs[0] = leftNode->ptrs[leftNode->sizeNode];
            leftNode->sizeNode--;
            current->sizeNode++;
            return;
        }
    }
    if (indexRight <= parentNode->sizeNode) { 
        BPlusNode *rightNode = parentNode->ptrs[indexRight]; 
        if (rightNode->sizeNode >= (n+1)/2) { 
            current->keys[current->sizeNode] = parentNode->keys[ind];
            parentNode->keys[ind] = rightNode->keys[0];
            for (int a=0; a<rightNode->sizeNode-1; a++) {
                rightNode->keys[a] = rightNode->keys[a+1];
            }

            current->ptrs[current->sizeNode+1] = rightNode->ptrs[0];
            for (int a=0; a<rightNode->sizeNode+1; a++) {
                rightNode->ptrs[a] = rightNode->ptrs[a+1];
            }

            rightNode->sizeNode--;
            current->sizeNode++;

            return;
        }
    }

    if (indexRight <= parentNode->sizeNode) {
        BPlusNode *rightNode = parentNode->ptrs[indexRight];

        current->keys[current->sizeNode] = parentNode->keys[indexRight-1];
        for (int i=current->sizeNode+1, j=0; j<rightNode->sizeNode; j++) {
            current->keys[i] = rightNode->keys[j];
        }

        for (int i=current->sizeNode+1, j=0; j<rightNode->sizeNode+1; j++) {
            current->ptrs[i] = rightNode->ptrs[j]; 
            rightNode->ptrs[j] = NULL;
        }

        current->sizeNode += rightNode->sizeNode + 1;
        rightNode->sizeNode = 0;

        deleteInternal(parentNode->keys[indexRight-1], parentNode, rightNode);

    } else if (indexLeft >= 0) { 

        BPlusNode *leftNode = parentNode->ptrs[indexLeft];
        leftNode->keys[leftNode->sizeNode] = parentNode->keys[indexLeft];
        for (int a=leftNode->sizeNode+1, j=0; j<current->sizeNode; j++) {
            leftNode->keys[a] = current->keys[j];
        }
        for (int a=leftNode->sizeNode+1, j=0; j<current->sizeNode+1; j++) {
            leftNode->ptrs[a] = current->ptrs[j];
            current->ptrs[j] = NULL;
        }

        leftNode->sizeNode += current->sizeNode + 1;
        current->sizeNode = 0;
        deleteInternal(parentNode->keys[indexLeft], parentNode, current);
    }
};