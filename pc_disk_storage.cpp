#include "pc_disk_storage.h"
#include <iostream>
#include <tuple>

using namespace std;

PCDiskStorage::PCDiskStorage(unsigned int totalSize, unsigned int sizeOfBlock)
{
    this->totalSize = totalSize; 
    this->sizeOfBlock = sizeOfBlock; 

    unsigned char *storagePtr = nullptr;
    this->storagePtr = new unsigned char[totalSize]; 
    this->blockPtr = nullptr;

    this->numAllocatedRecords = 0;                     
    this->numAllocatedBlocks = 0;    
    this->numAvailableBlocks = totalSize / sizeOfBlock;                  
    this->currentBlockSpaceUsed = 0;   
    
}

PCDiskStorage::~PCDiskStorage()
{
    delete storagePtr;
    storagePtr = nullptr;
}


int PCDiskStorage::searchNumVotes(unsigned int targetNumVotes)
{
    int numBlocksAccessed = 0;
    int numRecordsFound = 0;

    // Iterate through all allocated blocks
    for (auto block : blockList)
    {
        numBlocksAccessed++;

        // Iterate through all records in the block
        for (unsigned int offset = 0; offset < sizeOfBlock; offset += sizeof(Record))
        {
            Record* record = (Record*)(block + offset);

            // If record is not deleted and numVotes matches target value, increment count
            if (!record->deleted && record->numVotes == targetNumVotes)
            {
                numRecordsFound++;
            }
        }
    }

    // cout << "Number of data blocks accessed: " << numBlocksAccessed << endl;
    // cout << "Number of records found: " << numRecordsFound << endl;

    return numBlocksAccessed;
}


int PCDiskStorage::searchNumVotesBetween(unsigned int minNumVotes, unsigned int maxNumVotes) {
    int blocksAccessed = 0;
    int recordsSearched = 0;

    // Iterate through all allocated blocks
    for (auto block : blockList) {
        Record *currentRecord = (Record *)block;
        int numRecordsInBlock = sizeOfBlock / sizeof(Record);

        // Iterate through all records in block
        for (int i = 0; i < numRecordsInBlock; i++) {
            if (currentRecord[i].deleted)
                continue;
            if (currentRecord[i].numVotes >= minNumVotes && currentRecord[i].numVotes <= maxNumVotes) {
                // Record matches search criteria
                recordsSearched++;
            }
        }
        blocksAccessed++;
    }

    // cout << "Records found: " << recordsSearched << endl;
    // cout << "Blocks accessed: " << blocksAccessed << endl;

    return blocksAccessed;
}


int PCDiskStorage::deleteARecord(unsigned char *blockAddress, unsigned int offset, unsigned int recordSize)
try
{
    fill(blockAddress + offset, blockAddress + offset + recordSize, '\0');
    unsigned char *recordAddress = (unsigned char *)(blockAddress + offset);

    recordList.remove(recordAddress);
    numAllocatedRecords -= 1;
    return 1;
}
catch (exception &e)
{
    cout << "The exception:" << e.what() << endl;
    cout << "Unsuccessful deletion" << endl;
    return 0;
}


tuple<void *, unsigned int> PCDiskStorage::allocateARecord(unsigned int recordSize)
{
    if ((numAllocatedBlocks == 0) || (recordSize > (sizeOfBlock - currentBlockSpaceUsed))) 
    {
        if (!allocateABlock())
            throw "All the blocks have been allocated and there is no more free space in the blocks.";
    }

    if (recordSize > sizeOfBlock)
    {
        throw "We are unable to allocate a record as the record size is biger than the size of the block.";
    }

    tuple<void *, unsigned int> recordAddress(blockPtr, currentBlockSpaceUsed);

    recordList.push_back(blockPtr+currentBlockSpaceUsed); 

    numAllocatedRecords += 1;
    currentBlockSpaceUsed += recordSize;

    return recordAddress;
}


int PCDiskStorage::allocateABlock()
{
    if (numAvailableBlocks <= 0)
    {
        cout << "Unsuccessful allocation" << endl;
        return 0;
    }

    blockPtr = (numAllocatedBlocks * sizeOfBlock) + storagePtr;
    numAllocatedBlocks += 1;
    blockList.push_back(blockPtr);
    numAvailableBlocks -= 1;
    currentBlockSpaceUsed = 0;

    cout << "Data block: " << numAllocatedBlocks << "" << endl;
    return 1;
}
