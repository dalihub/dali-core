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

// HEADER
#include <dali/internal/event/text/atlas/debug/atlas-debug.h>

namespace Dali
{
namespace Internal
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gTextAtlasLogFilter = Debug::Filter::New(Debug::Concise, false, "LOG_TEXT_ATLAS");
#endif
}
}

#ifdef DEBUG_ATLAS

// INTERNAL INCLUDES
#include <dali/internal/event/text/glyph-status/glyph-status.h>

// EXTERNAL INCLUDES
#include <iostream>
#include <iomanip>  // for setw

namespace Dali
{

namespace Internal
{

namespace
{

unsigned int GetBlockId( unsigned int blockIndex, const BlockLookup* blockLookup)
{
  // this is a reverse lookup between the block number and the character code
  BlockLookup::const_iterator endIter = blockLookup->end();
  for(BlockLookup::const_iterator iter = blockLookup->begin(); iter != endIter; ++iter )
  {

    if( blockIndex ==  (*iter).second )
    {
      unsigned int charCode;
      FontId fontId;

      // the value stored in the lookup is encoded to CharacterCode | Font Id
      GlyphStatus::GetDecodedValue( (*iter).first , charCode, fontId);

      return charCode;
    }
  }
  return -1;
}

void PrintByte( unsigned int byteIndex, unsigned char block, const BlockLookup* blockLookup,  unsigned int blocksPerRows )
{
  for( unsigned int n = 0; n < 8; ++n)
  {
    unsigned int blockNum = (byteIndex * 8) + n;  // 8 blocks per byte

    if(( blockNum > 0) && ( blockNum % blocksPerRows) == 0 )
    {
      std::cout << std::endl;
    }

    bool bitset= block& (1<<n);

    if( bitset )
    {
      std::cout<< "1";
      if( blockLookup)
      {
        // add  (char) typecast to GetBlockId to see actual characters
        std::cout << ":" << std::setw(4) << GetBlockId( blockNum, blockLookup) << ",  ";
      }
    }
    else
    {
      std::cout<< "_";
      if( blockLookup)
      {
        std::cout<< " " << std::setw(4) <<" " << ",  ";
      }
    }
  }
}

} // un-named namespace

void DebugPrintAtlas( const FreeBlocks& blocks,
                      unsigned int blocksPerRow)
{
  std::cout << "----- Atlas Debug --- (1 = allocated) " << std::endl;

  for( std::size_t i = 0 ; i < blocks.size() ; ++i)
  {
    unsigned char block = blocks[i];
    PrintByte( i, block, NULL, blocksPerRow );
  }
  std::cout << std::endl << "-------------- " << std::endl;
}


void DebugPrintAtlasWithIds( const FreeBlocks& blocks,
                             const BlockLookup& blockLookup,
                             unsigned int blocksPerRow)

{

  std::cout <<"----- Atlas Debug --- (1 = allocated)" << std::endl;

  for( std::size_t i = 0 ; i < blocks.size() ; ++i)
  {
    unsigned char block = blocks[i];
    PrintByte( i, block, &blockLookup, blocksPerRow  );
  }
  std::cout << std::endl << "-------------- " << std::endl;

}


} // namespace Internal

} // namespace Dali

#endif // DEBUG_ATLAS
