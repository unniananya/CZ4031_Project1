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

    recordList.push_back(blockPtr+currentBlockSpaceUsed);

    currentBlockSpaceUsed += recordSize;

    return recordAddress;
}

// Function to delete a record from an address
int DiskStorage::deleteRecord(unsigned char *blockAddress, unsigned int offset, unsigned int recordSize)
try
{
    fill(blockAddress + offset, blockAddress + offset + recordSize, '\0');
    unsigned char *recordAddress = (unsigned char *)(blockAddress + offset);
    recordList.remove(recordAddress);
    return 1;
}

catch (exception &e)
{
    cout << "Exception" << e.what() << endl;
    cout << "Deletion unsuccessful" << endl;
    return 0;
}
