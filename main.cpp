#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <cstring>
#include <chrono>
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
        dataFile.close();

        // tree.displayTree(tree.getRoot());
    }

    // key:value pair to hold pair of memory addresses.
    unordered_map<void *, void*> blocksInMemory;
    //vector of tuples of record addresses <block address, relative record address>
    vector<tuple<void *, uint>>::iterator recordsIterator;

    cout << "\n<------------------- Experiment 1 --------------------------->\n";
    cout << "1. Number of allocated records: " << diskStorage.getallocatedRecords() << "\n";
    cout << "2. Size of Record: " << sizeof(Record) << "\n";    
    cout << "3. Number of allocated blocks: " << diskStorage.getallocatedBlocks() << "\n";
    cout << "4. Number of records that can be stored in a single data block: " << max_rec << "\n\n";


    cout << "\n<------------------- Experiment 2 --------------------------->\n";
    cout << "Parameter N of the B+ tree is: " << N << "\n";    

    int count = 0;
    count = tree.countNodes(tree.getRoot(), count);
    cout << "No. of nodes: " << count << "\n";

    int height = 0;
    height = tree.getHeight();
    cout << "No. of levels in B+ tree: " << height << "\n";

    cout << "\n";
    tree.displayTreeModified(tree.getRoot());

    cout << "\n<------------------- Experiment 3 --------------------------->\n";
    int searchfor = 500;
    
    cout << "[Searching for records with key " << searchfor << "]\n"; 
    queryResults * storedResults = NULL;
    auto start = std::chrono::high_resolution_clock::now();
    tree.query(searchfor, &storedResults);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
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

    std::cout << "Running time: " << duration.count() << " microseconds\n";

    auto start3 = std::chrono::high_resolution_clock::now();
    int blocksAccessed = diskStorage.searchNumVotes(searchfor);
    auto end3 = std::chrono::high_resolution_clock::now();
    auto duration3 = std::chrono::duration_cast<std::chrono::microseconds>(end3 - start3);
    cout << "Number of data blocks accessed by linear brute scan: " << blocksAccessed << endl;
    std::cout << "Running time of linear brute scan: " << duration3.count() << " microseconds\n";

    cout << "\n<------------------- Experiment 4 --------------------------->\n";
    int searchlow = 30000;
    int searchhigh = 40000;
    cout << "[Searching for records with key >= " << searchlow << " and <= " << searchhigh << "]\n"; 
    storedResults = NULL;
    // tree.query(9, 10, &storedResults);
    auto start1 = std::chrono::high_resolution_clock::now();
    tree.query(searchlow, searchhigh, &storedResults);
    auto end1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
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
    std::cout << "Running time: " << duration1.count() << " microseconds\n";

    auto start4 = std::chrono::high_resolution_clock::now();
    int blocksAccessed1 = diskStorage.searchNumVotesBetween(searchlow,searchhigh);
    auto end4 = std::chrono::high_resolution_clock::now();
    auto duration4 = std::chrono::duration_cast<std::chrono::microseconds>(end4 - start4);
    cout << "Number of data blocks accessed by linear brute scan: " << blocksAccessed1 << endl;
    std::cout << "Running time of linear brute scan: " << duration4.count() << " microseconds\n";

    cout << "\n<------------------- Experiment 5 --------------------------->\n";
    int deletethis = 1000;
    cout << "[deleting records with key " << deletethis << "]\n"; 
    // cout << "B+ Tree BEFORE deletion:\n";
    // tree.displayTreeModified(tree.getRoot());
    
    // int before = 0;
    // before = tree.countNodes(tree.getRoot(), before);

    auto start2 = std::chrono::high_resolution_clock::now();
    tree.deleteKey(deletethis);
    auto end2 = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2);
    // tree.displayTree(tree.getRoot());

    int after = 0;
    after = tree.countNodes(tree.getRoot(), after);

    // cout << "No. of times a node is deleted/two nodes are merged: " << before - after << "\n";
    cout << "No. of nodes in updated tree: " << after << "\n"; 

    height = 0;
    height = tree.getHeight();
    cout << "No. of levels in updated B+ tree:  " << height << "\n";

    cout << "B+ Tree AFTER deletion:\n";
    tree.displayTreeModified(tree.getRoot());

    std::cout << "Running time: " << duration2.count() << " microseconds\n";

    auto start5 = std::chrono::high_resolution_clock::now();
    int blocksAccessed2 = diskStorage.searchNumVotes(deletethis);
    auto end5 = std::chrono::high_resolution_clock::now();
    auto duration5 = std::chrono::duration_cast<std::chrono::microseconds>(end5 - start5);
    cout << "Number of data blocks accessed by linear brute scan: " << blocksAccessed2 << endl;
    std::cout << "Running time of linear brute scan: " << duration5.count() << " microseconds\n";
    return 0;
}
