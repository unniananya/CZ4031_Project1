#include <iostream>
#include <tuple>
#include <list>
#ifndef pc_disk_storage
#define pc_disk_storage
using namespace std;


const int sizeOfBlock = 200;

struct Record
{
    bool deleted = false;
    char tconst[10];
    float avgRating;
    unsigned int numVotes;
    Record * nextDuplicate = NULL;
};

class PCDiskStorage
{
private:
    unsigned char *blockPtr;
    unsigned char *storagePtr;

    unsigned int totalSize;
    unsigned int sizeOfBlock;

    list<unsigned char*> blockList; 
    list<unsigned char*> recordList;
    unsigned int numAvailableBlocks;
    unsigned int numAllocatedBlocks;
    unsigned int numAllocatedRecords;
    unsigned int currentBlockSpaceUsed;


public:
    PCDiskStorage(unsigned int storageSize, unsigned int sizeOfBlock);

    ~PCDiskStorage();

    int searchNumVotes(unsigned int targetNumVotes);

    int searchNumVotesBetween(unsigned int minNumVotes, unsigned int maxNumVotes);

    int deleteRecord(unsigned char *blockAddress, unsigned int offset, unsigned int recordSize);

    tuple<void *, unsigned int> allocateRecord(unsigned int recordSize);

    int allocateBlock();

    unsigned int gettotalSize(){
        return totalSize;
    }

    unsigned int getblockSize(){
        return sizeOfBlock;
    }

    unsigned int getnumAllocatedRecords(){
        return numAllocatedRecords;
    }

    list<unsigned char*> getBlockList(){
        return blockList;
    }

    list<unsigned char*> getRecordList(){
        return recordList;
    }

    unsigned int getnumAvailableBlocks(){
        return numAvailableBlocks;
    }

    unsigned int getnumAllocatedBlocks(){
        return numAllocatedBlocks;
    }

};
#endif
