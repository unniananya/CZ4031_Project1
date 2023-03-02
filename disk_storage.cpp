#include "disk_storage.h"
#include <iostream>
#include <tuple>

using namespace std;

// Constructor
DiskStorage::DiskStorage(unsigned int storageSize, unsigned int blockSize)
{
    this->storageSize = storageSize; // Total size of disk storage
    this->blockSize = blockSize;     // Size of each block

    unsigned char *storagePtr = nullptr;
    this->storagePtr = new unsigned char[storageSize]; // Creates an array of storageSize number of unsigned characters (pointer to each byte in disk)
    this->blockPtr = nullptr;

    this->allocatedRecords = 0;                       // Total number of allocated records
    this->allocatedBlocks = 0;                       // Total number of allocated blocks
    this->availableBlocks = storageSize / blockSize; // Total number of available blocks
    this->currentBlockSpaceUsed = 0;                 // Total space used in current block in bytes
}

// Destructor
DiskStorage::~DiskStorage()
{
    delete storagePtr;
    storagePtr = nullptr;
}

// Function to allocate a new block in disk storage
int DiskStorage::allocateBlock()
{
    if (availableBlocks > 0)
    {
        blockPtr = storagePtr + (allocatedBlocks * blockSize); // Pointer arithmetic to find starting byte of block
        allocatedBlocks += 1;
        availableBlocks -= 1;
        currentBlockSpaceUsed = 0;

        blockList.push_back(blockPtr); // Add block pointer to the list

        cout << "Block " << allocatedBlocks << " Allocation Success" << endl;
        return 1;
    }

    else
    {
        cout << "Block allocation unsuccessful" << endl;
        return 0;
    }
}


int DiskStorage::searchNumVotes(unsigned int targetNumVotes)
{
    int numBlocksAccessed = 0;
    int numRecordsFound = 0;

    // Iterate through all allocated blocks
    for (auto block : blockList)
    {
        numBlocksAccessed++;

        // Iterate through all records in the block
        for (unsigned int offset = 0; offset < blockSize; offset += sizeof(Record))
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


int DiskStorage::searchNumVotesBetween(unsigned int minNumVotes, unsigned int maxNumVotes) {
    int blocksAccessed = 0;
    int recordsSearched = 0;

    // Iterate through all allocated blocks
    for (auto block : blockList) {
        Record *currentRecord = (Record *)block;
        int numRecordsInBlock = blockSize / sizeof(Record);

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

// Function to allocate space for a new record in disk storage
tuple<void *, unsigned int> DiskStorage::allocateRecord(unsigned int recordSize)
{
    if (recordSize > (blockSize - currentBlockSpaceUsed) || (allocatedBlocks == 0)) // Allocate new block if current block has no space for record or no blocks has been allocated
    {
        if (!allocateBlock())
            throw "No more free space in current block and all blocks have been allocated";
    }

    if (blockSize < recordSize)
    {
        throw "Unable to allocate record to block as record size is larger than block size";
    }

    tuple<void *, unsigned int> recordAddress(blockPtr, currentBlockSpaceUsed);

    recordList.push_back(blockPtr+currentBlockSpaceUsed); // Add block pointer to the list

    currentBlockSpaceUsed += recordSize;

    allocatedRecords += 1;

    return recordAddress;
}

// Function to delete a record from an address
int DiskStorage::deleteRecord(unsigned char *blockAddress, unsigned int offset, unsigned int recordSize)
try
{
    fill(blockAddress + offset, blockAddress + offset + recordSize, '\0');
    unsigned char *recordAddress = (unsigned char *)(blockAddress + offset);
    recordList.remove(recordAddress);
    allocatedRecords -= 1;
    return 1;
}

catch (exception &e)
{
    cout << "Exception" << e.what() << endl;
    cout << "Deletion unsuccessful" << endl;
    return 0;
}
