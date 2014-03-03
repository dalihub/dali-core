#ifndef __DALI_INTERNAL_ATLAS_H__
#define __DALI_INTERNAL_ATLAS_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/common/map-wrapper.h>
#include <dali/internal/render/common/uv-rect.h>
#include <dali/internal/event/text/atlas/atlas-uv-interface.h>

namespace Dali
{

namespace Internal
{

/**
 * The atlas class contains the logic for working out where a fixed size
 * block (a bitmap) should be placed in to a texture atlas.
 *
 * Note: There is no physical storage done, the class just maps out where
 * blocks should be placed in the texture.
 *
 * The class uses a bitmask to represent which blocks are used.
 * E.g if we have a simple 8 x 8 block Atlas. It would be represent like:
 *
 * 0000 0000    ( one byte)
 * 0000 0000
 * 0000 0000
 * 0000 0000.
 *
 * If a block is marked as allocated, it's bit is set to 1.
 * E.g.
 * 1110 0001
 * 0000 0100
 * .... ....
 * Means blocks 0,1,2,7 and 13 are allocated.
 *
 * Internally instead of having a 2D array, the bitmask is just a 1D array.
 *
 * The class also provides an API to access the position and texture coordinates of each block.
 *
 * To debug the class, enable DEBUG_ATLAS in atlas-debug.h
 *
 */
class Atlas : public AtlasUvInterface
{

public:

  /**
   * Constructor
   * @param[in] atlasSize The width / height of the atlas (only square atlases are supported)
   * @param[in] blockSize The width  / height of each block in the atlas
   */
  Atlas( const unsigned int atlasSize, const unsigned int blockSize );

  /**
   * Destructor
   */
  virtual ~Atlas();

  /**
   * Clone the contents of the atlas passed as a parameter into this atlas
   * @param clone the atlas to clone
   */
  void CloneContents( Atlas* clone );

  /**
   * Inserts a block in to the atlas.
   *
   * @param[in] id, a user defined unique id, which can be used to delete the block in the future
   * @return true on success, false if there is no space in the atlas
   */
  bool Insert( unsigned int id );

  /**
   * Remove a block from the atlas
   * @param[in] id, a unique id of the block to be deleted
    */
  void Remove( unsigned int id );

  /**
   * Get the atlas size in pixels. As it is square, the size = width = height.
   * @return the size of the atlas
   */
  unsigned int GetSize() const;

  /**
   * Get the x,y position of an allocated block within the atlas.
   * Used to determine the x,y position to upload the block to in a texture
   * @param[in] id, a user defined unique id of the item to get
   * @param[out] xPos x position of the block
   * @param[out] yPos y position of the block
   */
  void GetXYPosition( unsigned int id, unsigned int &xPos, unsigned int &yPos ) const;

private: // AtlasUvInterface

  /**
   * @copydoc AtlasUvInterface::GetUvCoordinates
   */
  UvRect GetUvCoordinates(  unsigned int id ) const;

private:

  /**
   * Default constructor
   */
  Atlas();

  // Undefined copy constructor.
  Atlas( const Atlas& );

  // Undefined assignment operator.
  Atlas& operator=( const Atlas& );

  /**
   * Allocate a block in the atlas
   * @param[out] blockNum assigned a free block number
   */
  bool AllocateBlock( unsigned int& blockNum );


  /**
   * De-allocate a block in the atlas
   * @param[in] blockNum the block number to mark as free
   */
  void DeAllocateBlock( unsigned int blockNum );

   /**
    * Used to control whether the uv-coordinates are generated
    * when getting an atlas item.
    */
   enum UvMode
   {
     CALCULATE_UV,
     DONT_CALCULATE_UV
   };

   /**
    * Holds the UV co-ordinates and x,y position of
    * an item in the atlas.
    */
   struct AtlasItem
   {
     AtlasItem()
     : xPos(0),
       yPos(0)
     {
     }

     ~AtlasItem()
     {
     }
     // Undefined copy constructor.
     AtlasItem( const AtlasItem& );

     // Undefined assignment operator.
     AtlasItem& operator=( const AtlasItem& );

     UvRect uv;            ///< Texture uv co-ordinates
     unsigned int xPos;    ///< x-pixel position
     unsigned int yPos;    ///< y-pixel position
   };

  /**
   * Allocate a block in the atlas
   * @param[in] blockNum the block number
   * @param[out] atlasItem filled with information about the block
   * @param[in] mode whether to generate uv-coordinates or not
   */
  void FillAtlasItem( unsigned int blockNum, AtlasItem& atlasItem, UvMode mode) const;

  /**
   * Given a unique ID, returns the block number
   * @param[in] id unique user defined id
   * @return block number
   */
  unsigned int GetBlockNumber( unsigned int id) const;

  /**
   * Gets the blocks per row. E.g. a 4 x 4 Atlas will return 4.
   * @return the number of blocks per row.
   */
  unsigned int GetBlocksPerRow( ) const;

  /**
   * Returns if a block is allocated or not, along with the unique id of the block
   * @param[in] row block row
   * @param[in] column block column
   * @param[out] id unique id
   * @return true if block is allocated
   */
  bool IsBlockAllocated( unsigned int row, unsigned int column, unsigned int& id );

  /**
   * Gets a position of the block within the atlas.
   * E.g. In a 8 x 8 Atlas, block 8 (zero based) with have row = 1, col = 0.
   *
   * @param[in] block1dPos block id
   * @param[out] row atlas row
   * @param[out] column atlas column
   */
  void GetPositionOfBlock( unsigned int block1dPos, unsigned int& row, unsigned int& column );

  /**
   * Mark a block as a allocated
   * @param[in] row atlas row
   * @param[in] column atlas column
   * @return the id of the block that was allocated
   */
  unsigned int AllocateBlock( unsigned int row, unsigned int column );

  /**
   * Lookup between a user defined unique id, and a block number
   */
  typedef std::map< unsigned int /* user defined id */, unsigned int /* block num */ >   BlockLookup;

  /**
   * Bitmask, each bit set represents an allocated block
   */
  typedef std::vector<unsigned char> FreeBlocks;

  unsigned int  mSize;                    ///< The size of the atlas
  unsigned int  mBlockSize;               ///< The block size
  FreeBlocks    mFreeBlocks;              ///< Bitmask of free blocks
  BlockLookup   mBlockLookup;             ///< lookup between block number and unique id given by user


}; // class Atlas

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ATLAS_H__
