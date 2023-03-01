#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <cstring>
#include <stdlib.h>
#include "disk_storage.cpp"
#include "BPtree.cpp"
#include "disk_storage.h"

using namespace std;

//Type declarations for unsigned values 
typedef unsigned int uint;
typedef unsigned char uchar;

int main(){

    BPTree tree = BPTree();

    cout <<"------------------- Welcome to Database Storage  -------------------\n"
           "The database was created by allocating a memory pool, divided into blocks.\n"
           "We shall make use of a fixed-size memory pool for storage.\n"  
           "This is because the trade-off between efficiency gained from variable length storage \n"
           "and the cost of storing header files is not worth it for the given dataset\n\n";

    cout <<"<------------------- Data file read started ------------------->" << "\n" << "\n";

    //loading the dataset from hard drive and storing it in the memory
    //ifstream datafile("C:\\Users\\Neel\\OneDrive\\Desktop\\University\\Y3S1\\Databases\\data.tsv");

    ifstream dataFile("data.tsv");
    
    //100,000,000 bytes of storage in disk 
    //The blockSize is predetermined by the constant blockSize, currently at 200 bytes. 
    DiskStorage diskStorage{100000000, blockSize};

    vector<tuple<void *, uint>> dataset;
    bool flag=true;
    if(dataFile.is_open()) {
        string line;

        while (getline(dataFile, line)) {
            if(flag) {
                flag=false;
                continue;
            }
            Record record;
            string tempLine;
            strcpy(record.tconst, line.substr(0, line.find('\t')).c_str());
            stringstream linestream(line);
            getline(linestream, tempLine,'\t');
            linestream >> record.avgRating >> record.numVotes;
            tuple<void *, uint> dataRecord = diskStorage.allocateRecord(sizeof(record));
            dataset.push_back(dataRecord);

            void *rcdAdr = (uchar *)get<0>(dataRecord) + get<1>(dataRecord);
            memcpy(rcdAdr, &record, sizeof(record));
            //cout << "key is " << record.numVotes << "\n";
            tree.insert(record.numVotes, (Record *) get<0>(dataRecord));
            //tree.insert(record.numVotes, &record);
            
            //cout << rcdAdr << " " << record.tconst << '\n';
        }
        cout << "------------------ Finished reading data files ------------------" << "\n" << "\n";

        cout << "------------------ Summary of Database Storage -------------------" << "\n";

        cout << "1. Size of Memory Pool: " << diskStorage.getstorageSize() << "\n";
        cout << "2. Size of 1 block: " << diskStorage.getblockSize() << "\n";
        cout << "3. Number of blocks available at start: " << diskStorage.getstorageSize() / diskStorage.getblockSize() << "\n";
        cout << "4. Number of allocated blocks: " << diskStorage.getallocatedBlocks() << "\n";
        cout << "5. Number of available blocks: " << diskStorage.getavailableBlocks() << "\n";
        cout << "6. Number of records that can be stored in a single data block: " << max_rec << "\n\n";
        dataFile.close();

        // tree.displayTree(tree.getRoot());
    }

    // key:value pair to hold pair of memory addresses.
    unordered_map<void *, void*> blocksInMemory;
    //vector of tuples of record addresses <block address, relative record address>
    vector<tuple<void *, uint>>::iterator recordsIterator;

    cout << "\n<------------------- Experiment 2 [ " << blockSize << "B ] --------------------------->\n";
    cout << "Parameter N of the B+ tree is: " << N << "\n";    

    int count = 0;
    count = tree.countNodes(tree.getRoot(), count);
    cout << "No. of nodes: " << count << "\n";

    int height = 0;
    height = tree.getHeight();
    cout << "Height of tree: " << height << "\n";

    cout << "\n";
    tree.displayTreeModified(tree.getRoot());

    cout << "\n<------------------- Experiment 3 [ " << blockSize << "B ] --------------------------->\n";
    int searchfor = 500;
    
    cout << "[Searching for records with key " << searchfor << "]\n"; 
    queryResults * storedResults = NULL;

    tree.query(searchfor, &storedResults);
    queryResults *temp = storedResults;

    // get average of “averageRating’s” of the records that are returned
    temp = storedResults;
    float avg, total = 0;
    int i = 0;
    while (temp != NULL) {
        // loop through all records in array
        //cout << "numVotes is " << temp->record.numVotes << "\n";
        total += temp->record.avgRating; 
        i++;
        temp = temp->nextRecord;
    }
    avg = total/i;
    cout << "Average of avgRatings: " << avg << "\n";

    cout << "\n<------------------- Experiment 4 [ " << blockSize << "B ] --------------------------->\n";
    int searchlow = 30000;
    int searchhigh = 40000;
    cout << "[Searching for records with key >= " << searchlow << " and <= " << searchhigh << "]\n"; 
    storedResults = NULL;
    // tree.query(9, 10, &storedResults);
    tree.query(searchlow, searchhigh, &storedResults);
    temp = storedResults;

    // while (temp != NULL){
    //     cout << "numVotes is " << temp->record.numVotes << "\n";
    //     temp = temp->nextRecord;
    // }

    // get average of “averageRating’s” of the records that are returned
    temp = storedResults;
    avg, total = 0;
    i = 0;
    while (temp != NULL) {
        // loop through all records in array
        total += temp->record.avgRating; 
        i++;
        temp = temp->nextRecord;
    }
    avg = total/i;
    cout << "Average of avgRatings: " << avg << "\n";

    cout << "\n<------------------- Experiment 5 [ " << blockSize << "B ] --------------------------->\n";
    int deletethis = 1000;
    cout << "[deleting records with key " << deletethis << "]\n"; 
    cout << "B+ Tree BEFORE deletion:\n";
    tree.displayTreeModified(tree.getRoot());
    
    int before = 0;
    before = tree.countNodes(tree.getRoot(), before);
    
    tree.deleteKey(deletethis);
    // tree.displayTree(tree.getRoot());

    int after = 0;
    after = tree.countNodes(tree.getRoot(), after);

    cout << "No. of times a node is deleted/two nodes are merged: " << before - after << "\n";
    cout << "No. of nodes in updated tree: " << after << "\n"; 

    height = 0;
    height = tree.getHeight();
    cout << "Height of updated tree: " << height << "\n";

    cout << "B+ Tree AFTER deletion:\n";
    tree.displayTreeModified(tree.getRoot());
        
    return 0;
}
