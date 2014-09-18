/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// CLASS HEADER
#include <dali/internal/event/text/atlas/atlas.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/text/atlas/debug/atlas-debug.h>

// EXTERNAL INCLUDES
#include <math.h>

namespace Dali
{

namespace Internal
{

namespace // un-named namespace
{

void GetByteAndBitPosition( unsigned int blockNum, unsigned int& bytePos, unsigned int& bitPos )
{
  // divide by 8 to calculate the byte this block is in
  // 1 byte holds bitmask for 8 blocks
  // Example:  If block = 10, then block is in byte 1.
  //
  //   byte 0            byte 1             byte 3
  // [ 0000,0000 ],  [ 00  X  ..... ]   ,    [.........]
  //   0123 4567       89  ^10th bit set
  // blockNum / 8 = byte number
  //

  bytePos = blockNum >> 3;  // >>3 = divide by 8

  // calculate the bit, within the byte which the block uses
  // the lower 3 bits define it's position with the block
  // if block = 10, in binary this is 1010. Lower 3 bits = 010.
  // So bit position = 2 ( zero based)

  bitPos = blockNum & 7;  // AND With binary 111 to lower 3 bits
}

} // un-named namespace


Atlas::Atlas(const unsigned int atlasSize,
            const unsigned int blockSize)
: mSize(atlasSize),
  mBlockSize(blockSize)
{
  DALI_ASSERT_DEBUG(mBlockSize > 0  && atlasSize  >= blockSize);

  // Atlases are square
  unsigned int totalBlocks = GetBlocksPerRow() * GetBlocksPerRow();

  DALI_ASSERT_DEBUG(  (totalBlocks % 8) == 0 && "Atlas num blocks must be factor of 8" );

  // block allocation is using a bitmask in a 1D array.
  // e.g. 256 blocks require 256 bits to say whether a block is allocated
  // 256 / 8  = 32 bytes (or >> 3 to divide by 8 ).
  unsigned int bitMaskBytes = totalBlocks >> 3;

  mFreeBlocks.resize( bitMaskBytes );  // contents auto-initialised to zero
}

Atlas::~Atlas()
{

}

void Atlas::CloneContents( Atlas* clone )
{
  // Internally atlas allocation is done using a 1 dimensional array.
  // A single bit set in the array represents an allocation.
  //
  // So an atlas of size 8 x 8 blocks is 64 bits long.
  //
  // When cloning we keep the allocated blocks in the same 2D space.
  //
  //
  //  Original (4 x 4)  --> New Atlas ( 8 x 8)
  //
  //  1110                   1110 0000
  //  0010      ---------->  0010 0000
  //  0000                   0000 0000
  //  1001                   1001 0000
  //
  //                         0000 0000
  //                         0000 0000
  //                         0000 0000
  //                         0000 0000
  //
  // If we want the X,Y position of character 'X' in original atlas, it will be identical in the new atlas.
  // This allows the glyph bitmap to be uploaded to an identical place in the gl texture
  // to the old texture.
  // Original aim of this approach was to avoid re-calcualating uv co-ordinates.
  // However as the texture width / height has changed, the uv values also need adjusting (scaling)

  DALI_ASSERT_DEBUG( clone->mSize <= mSize);

  // go through each allocated block in the cloned atlas, and add to the this atlas.

  BlockLookup::const_iterator endIter = clone->mBlockLookup.end();
  for( BlockLookup::const_iterator iter = clone->mBlockLookup.begin(); iter != endIter; ++iter )
  {
    unsigned int key = (*iter).first;
    unsigned int block = (*iter).second;
    unsigned int row,column;

    clone->GetPositionOfBlock( block, row, column );

    unsigned int newBlockId = AllocateBlock( row, column );

    mBlockLookup[ key ] = newBlockId;
  }

#ifdef DEBUG_ATLAS
  DebugPrintAtlasWithIds( clone->mFreeBlocks, clone->mBlockLookup, clone->GetBlocksPerRow());
  DebugPrintAtlasWithIds( mFreeBlocks,mBlockLookup,  GetBlocksPerRow() );
#endif
}

bool Atlas::Insert( unsigned int id)
{
  unsigned int blockNum(0);

  bool ok = AllocateBlock( blockNum );

  if (!ok)
  {
    DALI_ASSERT_DEBUG( 0 && "Atlas full ");
    // Atlas full
    return false;
  }

  DALI_ASSERT_ALWAYS( mBlockLookup.find(id) == mBlockLookup.end() && "Inserted duplicate id into the atlas" );

  // store the link between block number and unique id
  mBlockLookup[id] = blockNum;

#ifdef DEBUG_ATLAS
  DebugPrintAtlas( mFreeBlocks, GetBlocksPerRow() );
#endif

  return true;
}

void Atlas::Remove(unsigned int id)
{
  BlockLookup::const_iterator iter = mBlockLookup.find( id );

  DALI_ASSERT_ALWAYS( iter != mBlockLookup.end() && "Failed to find id in atlas\n");

  DeAllocateBlock( (*iter).second );

  // remove the id from the lookup
  mBlockLookup.erase( id );
}

unsigned int Atlas::GetSize() const
{
  return mSize;
}

void Atlas::GetXYPosition( unsigned int id, unsigned int& xPos, unsigned int& yPos ) const
{
  AtlasItem item;

  unsigned int blockNum( GetBlockNumber( id ) );
  FillAtlasItem( blockNum, item, DONT_CALCULATE_UV );

  xPos = item.xPos;
  yPos = item.yPos;
}

UvRect Atlas::GetUvCoordinates( unsigned int id ) const
{
  AtlasItem item;

  unsigned int blockNum( GetBlockNumber( id ) );
  FillAtlasItem( blockNum, item, CALCULATE_UV );

  return item.uv;
}

Atlas::Atlas()
:mSize( 0 ),
 mBlockSize( 0 )
{

}

bool Atlas::AllocateBlock( unsigned int& blockNum )
{
  // scan the bitmask for a free block
  // each byte is a bitmask for 8 blocks, so 0000 0011, means blocks 1 and 2 are allocated
  for( size_t i = 0, end = mFreeBlocks.size(); i < end; ++i )
  {
    // check if a free bit is available
    unsigned char mask = mFreeBlocks[i];
    if( 0xFF != mask)
    {
      for( int n = 0; n < 8 ; n++)
      {
        // check if a bit is not set.
        if( ! (mask & (1 << n) ) )
        {
          // we have found a free bit, set it to 1.
          mask|= (1 << n);
          blockNum = i * 8 + n;
          mFreeBlocks[i] = mask;

          return true;
        }
      }
    }
  }
  blockNum = 0;
  return false;
}

void Atlas::DeAllocateBlock( unsigned int blockNum )
{
  unsigned int bytePos,bitPos;

  GetByteAndBitPosition( blockNum, bytePos, bitPos );

  unsigned char mask = mFreeBlocks[ bytePos ];

  // check the block was allocated
  DALI_ASSERT_DEBUG( ((mask & (1<< bitPos))) && "DeAllocated a block, that was never allocated" );

  // clear the bit
  mask &= ~(1 << bitPos);

  mFreeBlocks[ bytePos ] = mask;

}

void Atlas::FillAtlasItem( unsigned int blockNum, AtlasItem& atlasItem, UvMode mode ) const
{
  UvRect& uv(atlasItem.uv);

  unsigned int block1dPos = blockNum * mBlockSize;

  unsigned int blockX = block1dPos  % mSize;
  unsigned int blockY = mBlockSize * floor( block1dPos / mSize );

  atlasItem.xPos = blockX;
  atlasItem.yPos = blockY;

  if( mode == DONT_CALCULATE_UV )
  {
    return;
  }
  const float ratio = 1.0f / mSize;

  uv.u0 = ratio * (blockX);
  uv.v0 = ratio * (blockY);
  uv.u2 = ratio * (blockX + mBlockSize);
  uv.v2 =  ratio * (blockY + mBlockSize);

}

unsigned int Atlas::GetBlockNumber( unsigned int id) const
{
  BlockLookup::const_iterator iter = mBlockLookup.find( id );

  DALI_ASSERT_ALWAYS( iter != mBlockLookup.end() );

  return  (*iter).second;
}

unsigned int Atlas::GetBlocksPerRow() const
{
  return  mSize / mBlockSize;
}

void Atlas::GetPositionOfBlock( unsigned int block1dPos, unsigned int& row, unsigned int& column )
{
  column = 0;
  if( block1dPos > 0)
  {
    column =  block1dPos % GetBlocksPerRow();
  }
  row =  floor( block1dPos / GetBlocksPerRow() );

  unsigned int bytePos, bitPos;

  GetByteAndBitPosition( block1dPos, bytePos, bitPos );
}

unsigned int Atlas::AllocateBlock( unsigned int row, unsigned int column )
{
  unsigned int blockNum = (row * GetBlocksPerRow()) + column;

  unsigned int bytePos, bitPos;

  GetByteAndBitPosition( blockNum, bytePos, bitPos );

  unsigned char& mask = mFreeBlocks.at( bytePos);

  mask |= 1<< bitPos; // set the bit to mark as allocated

  return blockNum;
}


} // namespace Internal

} // namespace Dali
