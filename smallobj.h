#ifndef SMALLOBJ_H
#define SMALLOBJ_H

#include <cassert>
#include <climits>
#include <vector>
#include <bitset>

namespace Loki {
/** @struct Chunk
    @ingroup SmallObjectGroupInternal
 Contains info about each allocated Chunk - which is a collection of
 contiguous blocks.  Each block is the same size, as specified by the
 FixedAllocator.  The number of blocks in a Chunk depends upon page size.
 This is a POD-style struct with value-semantics.  All functions and data
 are private so that they can not be changed by anything other than the
 FixedAllocator which owns the Chunk.

 @par Minimal Interface
 For the sake of runtime efficiency, no constructor, destructor, or
 copy-assignment operator is defined. The inline functions made by the
 compiler should be sufficient, and perhaps faster than hand-crafted
 functions.  The lack of these functions allows vector to create and copy
 Chunks as needed without overhead.  The Init and Release functions do
 what the default constructor and destructor would do.  A Chunk is not in
 a usable state after it is constructed and before calling Init.  Nor is
 a Chunk usable after Release is called, but before the destructor.

 @par Efficiency
 Down near the lowest level of the allocator, runtime efficiencies trump
 almost all other considerations.  Each function does the minimum required
 of it.  All functions should execute in constant time to prevent higher-
 level code from unwittingly using a version of Shlemiel the Painter's
 Algorithm.

 @par Stealth Indexes
 The first char of each empty block contains the index of the next empty
 block.  These stealth indexes form a singly-linked list within the blocks.
 A Chunk is corrupt if this singly-linked list has a loop or is shorter
 than blocksAvailable_.  Much of the allocator's time and space efficiency
 comes from how these stealth indexes are implemented.
 */
class Chunk
{
private:
    friend class FixedAllocator;

    /** Initializes a just-constructed Chunk.
     @param blockSize Number of bytes per block.
     @param blocks Number of blocks per Chunk.
     @return True for success, false for failure.
     */
    bool Init( std::size_t blockSize, unsigned char blocks );

    /** Allocate a block within the Chunk.  Complexity is always O(1), and
     this will never throw.  Does not actually "allocate" by calling
     malloc, new, or any other function, but merely adjusts some internal
     indexes to indicate an already allocated block is no longer available.
     @return Pointer to block within Chunk.
     */
    void * Allocate( std::size_t blockSize );

    /** Deallocate a block within the Chunk. Complexity is always O(1), and
     this will never throw.  For efficiency, this assumes the address is
     within the block and aligned along the correct byte boundary.  An
     assertion checks the alignment, and a call to HasBlock is done from
     within VicinityFind.  Does not actually "deallocate" by calling free,
     delete, or other function, but merely adjusts some internal indexes to
     indicate a block is now available.
     */
    void Deallocate( void * p, std::size_t blockSize );

    /** Resets the Chunk back to pristine values. The available count is
     set back to zero, and the first available index is set to the zeroth
     block.  The stealth indexes inside each block are set to point to the
     next block. This assumes the Chunk's data was already using Init.
     */
    void Reset( std::size_t blockSize, unsigned char blocks );

    /// Releases the allocated block of memory.
    void Release();

    /** Determines if the Chunk has been corrupted.
     @param numBlocks Total # of blocks in the Chunk.
     @param blockSize # of bytes in each block.
     @param checkIndexes True if caller wants to check indexes of available
      blocks for corruption.  If false, then caller wants to skip some
      tests tests just to run faster.  (Debug version does more checks, but
      release version runs faster.)
     @return True if Chunk is corrupt.
     */
    bool IsCorrupt( unsigned char numBlocks, std::size_t blockSize,
        bool checkIndexes ) const;

    /** Determines if block is available.
     @param p Address of block managed by Chunk.
     @param numBlocks Total # of blocks in the Chunk.
     @param blockSize # of bytes in each block.
     @return True if block is available, else false if allocated.
     */
    bool IsBlockAvailable( void * p, unsigned char numBlocks,
        std::size_t blockSize ) const;

    /// Returns true if block at address P is inside this Chunk.
    inline bool HasBlock( void * p, std::size_t chunkLength ) const
    {
        unsigned char * pc = static_cast< unsigned char * >( p );
        return ( pData_ <= pc ) && ( pc < pData_ + chunkLength );
    }

    inline bool HasAvailable( unsigned char numBlocks ) const
    { return ( blocksAvailable_ == numBlocks ); }

    inline bool IsFilled( void ) const
    { return ( 0 == blocksAvailable_ ); }

    /// Pointer to array of allocated blocks.
    unsigned char * pData_;
    /// Index of first empty block.
    unsigned char firstAvailableBlock_;
    /// Count of empty blocks.
    unsigned char blocksAvailable_;
};

/** @class FixedAllocator
    @ingroup SmallObjectGroupInternal
 Offers services for allocating fixed-sized objects.  It has a container
 of "containers" of fixed-size blocks.  The outer container has all the
 Chunks.  The inner container is a Chunk which owns some blocks.

 @par Class Level Invariants
 - There is always either zero or one Chunk which is empty.
 - If this has no empty Chunk, then emptyChunk_ is NULL.
 - If this has an empty Chunk, then emptyChunk_ points to it.
 - If the Chunk container is empty, then deallocChunk_ and allocChunk_
   are NULL.
 - If the Chunk container is not-empty, then deallocChunk_ and allocChunk_
   are either NULL or point to Chunks within the container.
 - allocChunk_ will often point to the last Chunk in the container since
   it was likely allocated most recently, and therefore likely to have an
   available block.
 */
class FixedAllocator
{
private:

    /** Deallocates the block at address p, and then handles the internal
     bookkeeping needed to maintain class invariants.  This assumes that
     deallocChunk_ points to the correct chunk.
     */
    void DoDeallocate( void * p );

    /** Creates an empty Chunk and adds it to the end of the ChunkList.
     All calls to the lower-level memory allocation functions occur inside
     this function, and so the only try-catch block is inside here.
     @return true for success, false for failure.
     */
    bool MakeNewChunk( void );

    /** Finds the Chunk which owns the block at address p.  It starts at
     deallocChunk_ and searches in both forwards and backwards directions
     from there until it finds the Chunk which owns p.  This algorithm
     should find the Chunk quickly if it is deallocChunk_ or is close to it
     in the Chunks container.  This goes both forwards and backwards since
     that works well for both same-order and opposite-order deallocations.
     (Same-order = objects are deallocated in the same order in which they
     were allocated.  Opposite order = objects are deallocated in a last to
     first order.  Complexity is O(C) where C is count of all Chunks.  This
     never throws.
     @return Pointer to Chunk that owns p, or NULL if no owner found.
     */
    //附近查找
    Chunk * VicinityFind( void * p ) const;

    /// Not implemented.
    FixedAllocator(const FixedAllocator&);
    /// Not implemented.
    FixedAllocator& operator=(const FixedAllocator&);

    /// Type of container used to hold Chunks.
    typedef std::vector< Chunk > Chunks;
    /// Iterator through container of Chunks.
    typedef Chunks::iterator ChunkIter;
    /// Iterator through const container of Chunks.
    typedef Chunks::const_iterator ChunkCIter;

    /// Fewest # of objects managed by a Chunk.
    inline static unsigned char MinObjectsPerChunk_=8;

    /// Most # of objects managed by a Chunk - never exceeds UCHAR_MAX.
    inline static unsigned char MaxObjectsPerChunk_=UCHAR_MAX;

    //两者之和是chunk总大小
    /// Number of bytes in a single block within a Chunk.
    std::size_t blockSize_;
    /// Number of blocks managed by each Chunk.
    unsigned char numBlocks_;

    /// Container of Chunks.
    Chunks chunks_;
    /// Pointer to Chunk used for last or next allocation.
    Chunk * allocChunk_;
    /// Pointer to Chunk used for last or next deallocation.
    Chunk * deallocChunk_;
    /// Pointer to the only empty Chunk if there is one, else NULL.
    Chunk * emptyChunk_;

public:
    /// Create a FixedAllocator which manages blocks of 'blockSize' size.
    FixedAllocator();

    /// Destroy the FixedAllocator and release all its Chunks.
    ~FixedAllocator();

    /// Initializes a FixedAllocator by calculating # of blocks per Chunk.
    void Initialize( std::size_t blockSize, std::size_t pageSize );

    /** Returns pointer to allocated memory block of fixed size - or NULL
     if it failed to allocate.
     */
    void * Allocate( void );

    /** Deallocate a memory block previously allocated with Allocate.  If
     the block is not owned by this FixedAllocator, it returns false so
     that SmallObjAllocator can call the default deallocator.  If the
     block was found, this returns true.
     */
    bool Deallocate( void * p, Chunk * hint );

    /// Returns block size with which the FixedAllocator was initialized.
    inline std::size_t BlockSize() const { return blockSize_; }

    /** Releases the memory used by the empty Chunk.  This will take
     constant time under any situation.
     @return True if empty chunk found and released, false if none empty.
     */
    bool TrimEmptyChunk( void );

    /** Releases unused spots from ChunkList.  This takes constant time
     with respect to # of Chunks, but actual time depends on underlying
     memory allocator.
     @return False if no unused spots, true if some found and released.
     */
    bool TrimChunkList( void );

    /** Returns count of empty Chunks held by this allocator.  Complexity
     is O(C) where C is the total number of Chunks - empty or used.
     */
    std::size_t CountEmptyChunks( void ) const;

    /** Determines if FixedAllocator is corrupt.  Checks data members to
     see if any have erroneous values, or violate class invariants.  It
     also checks if any Chunk is corrupt.  Complexity is O(C) where C is
     the number of Chunks.  If any data is corrupt, this will return true
     in release mode, or assert in debug mode.
     */
    bool IsCorrupt( void ) const;

    /** Returns true if the block at address p is within a Chunk owned by
     this FixedAllocator.  Complexity is O(C) where C is the total number
     of Chunks - empty or used.
     */
    const Chunk * HasBlock( void * p ) const;
    inline Chunk * HasBlock( void * p )
    {
        return const_cast< Chunk * >(
            const_cast< const FixedAllocator * >( this )->HasBlock( p ) );
    }

};

//unsigned char FixedAllocator::MinObjectsPerChunk_ = 8;
//unsigned char FixedAllocator::MaxObjectsPerChunk_ = UCHAR_MAX;
}

#endif // SMALLOBJ_H
