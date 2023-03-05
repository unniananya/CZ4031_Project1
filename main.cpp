#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <cstring>
#include <chrono>
#include <stdlib.h>
#include "pc_disk_storage.cpp"
#include "BPlusTree.cpp"
#include "pc_disk_storage.h"

using namespace std;
typedef unsigned int uint;
typedef unsigned char uchar;

int main(){

    BPlusTree bplusTree = BPlusTree();
    PCDiskStorage pcDiskStorage{100000000, sizeOfBlock};

    ifstream dataFile("data.tsv");

    bool flag=true;
    vector<tuple<void *, uint>> data;
    if(dataFile.is_open()) {
        string line;

        while (getline(dataFile, line)) {
            if(flag) {
                flag=false;
                continue;
            }
            Record record;
            string temp;
            strcpy(record.tconst, line.substr(0, line.find('\t')).c_str());


            stringstream linestream(line);
            getline(linestream, temp,'\t');
            linestream >> record.avgRating >> record.numVotes;
            tuple<void *, uint> data_record = pcDiskStorage.allocateRecord(sizeof(record));
            data.push_back(data_record);

            void *rdAddr = (uchar *)get<0>(data_record) + get<1>(data_record);
            memcpy(rdAddr, &record, sizeof(record));

            bplusTree.insertv1(record.numVotes, (Record *) get<0>(data_record));
        }
        dataFile.close();
    }

    cout << "\n<------------------- Experiment 1 ------------------->\n";
    cout << "Number of records: " << pcDiskStorage.getnumAllocatedRecords() << "\n";
    cout << "Size of a record: " << sizeof(Record) << "\n";    
    cout << "Number of blocks for storing the data: " << pcDiskStorage.getnumAllocatedBlocks() << "\n";
    cout << "Number of records stored in a data block: " << maxNumRecords << "\n\n";


    cout << "\n<------------------- Experiment 2 ------------------->\n";
    cout << "Parameter n of the B+ tree is: " << n << "\n";    
    int numNodes = 0;
    numNodes = bplusTree.NumNodesTree(bplusTree.getRoot(), numNodes);
    cout << "No. of nodes of the B+ tree: " << numNodes << "\n";
    int level = 0;
    level = bplusTree.getLevelOfTree();
    cout << "No. of levels of the B+ tree: " << level << "\n";
    bplusTree.showRootNodes(bplusTree.getRoot());

    
    
    cout << "\n<------------------- Experiment 3 ------------------->\n";
    int searchNode = 500;

    recordResults * gottenResults = NULL;

    auto start = std::chrono::high_resolution_clock::now();
    auto [nodesAccessed, blocksAccessed] = bplusTree.findNumVotes(searchNode, &gottenResults);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    recordResults *tmpResults = gottenResults;
    std::cout << "Number of index nodes the process accesses: " << nodesAccessed << std::endl;
    std::cout << "Number of data blocks the process accesses: " << blocksAccessed << std::endl;
    tmpResults = gottenResults;
    int i = 0;
    float avg = 0;
    float totalAvg = 0;
    while (tmpResults != NULL) {
        totalAvg += tmpResults->record.avgRating; 
        i++;
        tmpResults = tmpResults->nextRecord;
    }
    avg = totalAvg/i;
    cout << "Average of averageRatings of the records that are returned: " << avg << "\n";
    std::cout << "Running time of the retrieval process: " << duration.count() << " microseconds\n";

    auto start3 = std::chrono::high_resolution_clock::now();
    int datablocksAccessed = pcDiskStorage.searchNumVotes(searchNode);
    auto end3 = std::chrono::high_resolution_clock::now();
    auto duration3 = std::chrono::duration_cast<std::chrono::microseconds>(end3 - start3);
    cout << "Number of data blocks accessed by linear brute scan: " << datablocksAccessed << endl;
    std::cout << "Running time of the linear brute scan: " << duration3.count() << " microseconds\n";

    cout << "\n<------------------- Experiment 4 ------------------->\n";
    int searchNodeLow = 30000;
    int searchNodeHigh = 40000; 
    gottenResults = NULL;

    auto start1 = std::chrono::high_resolution_clock::now();
    auto [nodesAccessed1, blocksAccessed1] = bplusTree.findNumVotes(searchNodeLow, searchNodeHigh, &gottenResults);
    auto end1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
    std::cout << "Number of index nodes the process accesses: " << nodesAccessed1 << std::endl;
    std::cout << "Number of data blocks the process accesses: " << blocksAccessed1 << std::endl;
    tmpResults = gottenResults;

    float avg1 = 0;
    float totalAvg1 = 0;
    i = 0;
    while (tmpResults != NULL) {
        totalAvg1 += tmpResults->record.avgRating; 
        i++;
        tmpResults = tmpResults->nextRecord;
    }
    avg1 = totalAvg1/i;
    cout << "Average of averageRatings of the records that are returned: " << avg1 << "\n";
    std::cout << "Running time of the retrieval process: " << duration1.count() << " microseconds\n";

    auto start4 = std::chrono::high_resolution_clock::now();
    int datablocksAccessed1 = pcDiskStorage.searchNumVotesBetween(searchNodeLow,searchNodeHigh);
    auto end4 = std::chrono::high_resolution_clock::now();
    auto duration4 = std::chrono::duration_cast<std::chrono::microseconds>(end4 - start4);
    cout << "Number of data blocks accessed by linear brute scan: " << datablocksAccessed1 << endl;
    std::cout << "Running time of the linear brute scan: " << duration4.count() << " microseconds\n";

    cout << "\n<------------------- Experiment 5 ------------------->\n";
    int deletethis = 1000;
   
    auto start2 = std::chrono::high_resolution_clock::now();
    bplusTree.deleteKey(deletethis);
    auto end2 = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2);

    int after = 0;
    after = bplusTree.NumNodesTree(bplusTree.getRoot(), after);

    cout << "No. of nodes in updated tree: " << after << "\n"; 

    level = 0;
    level = bplusTree.getLevelOfTree();
    cout << "No. of levels in updated B+ tree:  " << level << "\n";

    bplusTree.showRootNodes(bplusTree.getRoot());

    std::cout << "Running time of the retrieval process: " << duration2.count() << " microseconds\n";

    auto start5 = std::chrono::high_resolution_clock::now();
    int datablocksAccessed2 = pcDiskStorage.searchNumVotes(deletethis);
    auto end5 = std::chrono::high_resolution_clock::now();
    auto duration5 = std::chrono::duration_cast<std::chrono::microseconds>(end5 - start5);
    cout << "Number of data blocks accessed by linear brute scan: " << datablocksAccessed2 << endl;
    std::cout << "Running time of the linear brute scan: " << duration5.count() << " microseconds\n";
    return 0;
}
