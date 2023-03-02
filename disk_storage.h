#include <iostream>
#include <tuple>
#include <list>
#ifndef disk_storage
#define disk_storage

using namespace std;

const int blockSize = 200; //change to 200 for 200B blocksize; 500 for 500B blocksize

struct Record
{
    bool deleted = false;
    char tconst[10];
    float avgRating;
    unsigned int numVotes;
    Record * nextDupe = NULL;
};

class DiskStorage
{
private:
    unsigned char *blockPtr;
    unsigned char *storagePtr;

    unsigned int storageSize;
    unsigned int blockSize;
    unsigned int storageBlocksUsed;
    unsigned int storageRecordsUsed;
    unsigned int allocatedBlocks;
    unsigned int availableBlocks;
    unsigned int allocatedRecords;
    unsigned int currentBlockSpaceUsed;

    list<unsigned char*> blockList; // A list to store all block pointers
    list<unsigned char*> recordList;

public:
    DiskStorage(unsigned int storageSize, unsigned int blockSize);

    ~DiskStorage();

    int allocateBlock();

    int searchNumVotes(unsigned int targetNumVotes);

    int searchNumVotesBetween(unsigned int minNumVotes, unsigned int maxNumVotes);

    tuple<void *, unsigned int> allocateRecord(unsigned int recordSize);

    int deleteRecord(unsigned char *blockAddress, unsigned int offset, unsigned int recordSize);

    //setters and getters
    unsigned int getblockSize(){
        return blockSize;
    }

    // unsigned char getstoragePtr(){
    //     return *storagePtr;
    // }

    unsigned int getstorageSize(){
        return storageSize;
    }

    unsigned int getallocatedBlocks(){
        return allocatedBlocks;
    }

    unsigned int getallocatedRecords(){
        return allocatedRecords;
    }

    unsigned int getavailableBlocks(){
        return availableBlocks;
    }

    // Get the list of block pointers
    list<unsigned char*> getBlockList(){
        return blockList;
    }

    list<unsigned char*> getRecordList(){
        return recordList;
    }
};
#endif
