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
#include <dali/internal/event/images/emoji-factory.h>

// EXTERNAL INCLUDES
#include <sstream>
#include <limits>

// INTERNAL INCLUDES
#include <dali/integration-api/platform-abstraction.h>
#include <dali/internal/event/common/thread-local-storage.h>

namespace Dali
{

namespace Internal
{

namespace
{
const std::string U( "u" );       ///<
const std::string PNG( ".png" );  ///< Used to build the emoji file name.
const std::size_t PNG_SIZE( 4u ); ///<
} // namespace

EmojiFactory::EmojiFactory()
: mColorGlyphs(),
  mMinEmoji( std::numeric_limits<uint32_t>::max() )
{
  Internal::ThreadLocalStorage& tls = Internal::ThreadLocalStorage::Get();

  // Get the color glyphs from the directory.
  Integration::PlatformAbstraction& platform = tls.GetPlatformAbstraction();

  std::vector<std::string> fileNames;
  platform.GetFileNamesFromDirectory( DALI_EMOTICON_DIR, fileNames );

  for( std::vector<std::string>::const_iterator it = fileNames.begin(), endIt = fileNames.end(); it != endIt; ++it )
  {
    const std::string& fileName( *it );
    const std::size_t fileNameSize = fileName.size();

    if( ( fileNameSize < PNG_SIZE + 1u ) ||
        ( U != fileName.substr( 0u, 1u ) ) ||
        ( PNG != fileName.substr( fileNameSize - PNG_SIZE, PNG_SIZE ) ) )
    {
      // Invalid emoji file name.
      break;
    }

    uint32_t character = 0u;

    std::istringstream( fileName.substr( 1u, fileNameSize - PNG_SIZE - 1u ) ) >> std::hex >> character;

    if( 0u != character )
    {
      if( character < mMinEmoji )
      {
        mMinEmoji = character;
      }

      mColorGlyphs[character] = fileName;
    }
  }
}

EmojiFactory::~EmojiFactory()
{
}

bool EmojiFactory::IsEmoji( const uint32_t character ) const
{
  return ( character >= mMinEmoji ) &&
         ( mColorGlyphs.find( character ) != mColorGlyphs.end() );
}

std::string EmojiFactory::GetEmojiFileNameFromCharacter( const uint32_t character ) const
{
  std::string fileName;

  std::map<uint32_t,std::string>::const_iterator it = mColorGlyphs.find( character );

  if( it != mColorGlyphs.end() )
  {
    fileName = it->second;
  }

  return fileName;
}

} // namespace Internal

} // namespace Dali
