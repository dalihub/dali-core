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
#include <dali/internal/event/text/glyph-status/glyph-status-container.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/event/text/glyph-status/debug/glyph-status-container-debug.h>
#include <dali/internal/event/text/special-characters.h>

// EXTERNAL INCLUDES
#include <algorithm> // std::sort

namespace Dali
{

namespace Internal
{

namespace // un-named namespace
{

/**
 * @return the unique characters in a text array
 */
TextArray GetUniqueCharacters( const TextArray &text  )
{
  TextArray utfCodes( text );

  std::sort( utfCodes.begin(), utfCodes.end() );
  TextArray::iterator it = std::unique( utfCodes.begin(), utfCodes.end() );
  utfCodes.resize(it - utfCodes.begin());
  return utfCodes;
}

} // un-named namespace

GlyphStatusContainer::GlyphStatusContainer( unsigned int numberOfCharacters )
: mContainerSize( numberOfCharacters ),
  mEmptySpace( numberOfCharacters ),
  mCharacterLookup( GlyphStatus::CharCodeAndFontComparator ),
  mDeadCharacters( GlyphStatus::DeadTimeComparator ),
  mTimeStamp( 0 )
{
  // make sure the dead time has enough range
  // to handle every item in the atlas  being dead ( cached).
  DALI_ASSERT_DEBUG( GlyphStatus::GetMaximumDeadTime() > numberOfCharacters );
}

GlyphStatusContainer::~GlyphStatusContainer()
{
#ifdef DEBUG_ENABLED
  // Check every character has ref-count of zero
  StatusSet::const_iterator endIter =  mCharacterLookup.end();

  for( StatusSet::iterator iter = mCharacterLookup.begin(); iter != endIter; ++iter)
  {
    const GlyphStatus& status( (*iter) );
    DALI_ASSERT_DEBUG( status.GetRefCount() == 0 && "glyph still referenced (leaking)");
  }
#endif
}

void GlyphStatusContainer::IncreaseRefCount( uint32_t charCode, FontId fontId)
{
  const GlyphStatus& status = GetStatus(  charCode, fontId );

  if( status.GetRefCount() == 0 )
  {
    // if ref-count == zero, then it will have previously been added to the dead characters list.
    // which means nothing is using it, but it's distance field is still loaded in to the texture atlas.
    // As it is now been used, remove it from the dead list
    RemoveDeadCharacter( &status);
  }

  status.IncreaseRefCount();  // increase the reference count

#ifdef DEBUG_GLYPH_STATUS_CONTAINER
  DebugGlyphContainer( mContainerSize, mCharacterLookup, mDeadCharacters );
#endif

}

void GlyphStatusContainer::DecreaseRefCount( uint32_t charCode, FontId fontId)
{
  if( charCode < SpecialCharacters::FIRST_VISIBLE_CHAR )
  {
    return;
  }

  const GlyphStatus* status = FindGlyphStatus(  charCode, fontId );

  if( status == NULL)
  {
    DALI_LOG_ERROR("Glyph not found in container %d, font  %d\n",charCode, fontId);
    DALI_ASSERT_ALWAYS( status && "glyph not found (dec)");
  }

  status->DecreaseRefCount();

  // if it's got a ref count of zero, then it's no longer used.
  // Add it to the dead list.
  if( status->GetRefCount() == 0 )
  {
    AddDeadCharacter( status );
  }

#ifdef DEBUG_GLYPH_STATUS_CONTAINER
  DebugGlyphContainer( mContainerSize, mCharacterLookup, mDeadCharacters );
#endif
}

GlyphStatusContainer::InsertResult GlyphStatusContainer::InsertNewCharacter( uint32_t charCode, FontId fontId, unsigned int& deadUniqueId )
{
  // if there's free space, use it.
  if( mEmptySpace > 0 )
  {
    mEmptySpace--;

    InsertCharacterIntoLookup( charCode, fontId );

    return INSERTED_OK;
  }
  else
  {
    // there's no empty space, only used and cached space
    // the new character has to replace an existing un-used character in the texture atlas

    DALI_ASSERT_ALWAYS( ( mDeadCharacters.empty() == false ) && "no glyph container space");

    // Erase the oldest  character from dead character list.
    const GlyphStatus* deadCharacter = RemoveOldestDeadCharacter();

    // get the unique id, so the caller knows which character was deleted
    deadUniqueId = deadCharacter->GetUniqueId();

    // erase it from the lookup.
    mCharacterLookup.erase( *deadCharacter );

    // insert the new one
    InsertCharacterIntoLookup( charCode, fontId );

    DALI_ASSERT_DEBUG( mEmptySpace == (mContainerSize - mCharacterLookup.size()));

    return REPLACE_DEAD_CHARACTER;
  }
}

const GlyphStatus* GlyphStatusContainer::FindGlyphStatus( uint32_t charCode, FontId fontId) const
{
  GlyphStatus searchStatus( charCode, fontId );

  StatusSet::iterator iter = mCharacterLookup.find( searchStatus ) ;

  if( iter == mCharacterLookup.end() )
  {
    return NULL;
  }
  else
  {
    return &(*iter);
  }
}

const GlyphStatus& GlyphStatusContainer::GetStatus( uint32_t charCode, FontId fontId ) const
{
  const GlyphStatus* status = FindGlyphStatus( charCode, fontId );

  DALI_ASSERT_ALWAYS( status!= NULL && "glyph status code not found" );

  return *status;
}

const GlyphStatusContainer::StatusSet& GlyphStatusContainer::GetStatusSet() const
{
  return mCharacterLookup;
}

bool GlyphStatusContainer::IsTextLoaded( const TextArray& text, FontId fontId) const
{
  for( std::size_t n = 0, size = text.size(); n < size; ++n)
  {
    uint32_t charCode = text[n];

    // ignore invisible characters
    if( charCode < SpecialCharacters::FIRST_VISIBLE_CHAR )
    {
      continue;
    }
    const GlyphStatus& status = GetStatus( charCode, fontId );

    if( status.IsUploadedToTexture() == false )
    {
      return false;
    }
  }
  return true;
}

void GlyphStatusContainer::GetTextStatus( const TextArray& text,
                    FontId fontId,
                    unsigned int& charsNotLoaded,
                    bool& fitsInContainer ) const
{
  charsNotLoaded = 0;
  fitsInContainer = false;

  TextArray uniqueText = GetUniqueCharacters( text );

  for( std::size_t n = 0, size = uniqueText.size(); n < size; ++n)
  {
    uint32_t charCode = uniqueText[n];

    // ignore invisible characters
    if( charCode < SpecialCharacters::FIRST_VISIBLE_CHAR )
    {
      continue;
    }

    const GlyphStatus* status = FindGlyphStatus( charCode, fontId );

    // check if the glyph exists in the container and isn't marked as dead ( ref = 0)
    if( (status == NULL ) || (status->GetRefCount() == 0) )
    {
      charsNotLoaded++;
    }
  }

  // total available space = free space + space used by  dead characters that can be used
  if( charsNotLoaded <= TotalAvailableSpace() )
  {
    fitsInContainer = true;
  }
}

void GlyphStatusContainer::CloneContents( const GlyphStatusContainer& clone )
{
  // copy the lookup
  mCharacterLookup = clone.mCharacterLookup;


  // adjust the size
  mEmptySpace-= mCharacterLookup.size();

  // for each entry set the loaded status

  StatusSet::const_iterator endIter =  mCharacterLookup.end();
  for( StatusSet::iterator iter = mCharacterLookup.begin(); iter != endIter; ++iter)
  {
    const GlyphStatus& status = (*iter);

    status.SetLoadStatus( GlyphStatus::GLYPH_NOT_LOADED);
    status.SetTextureStatus( GlyphStatus::GLYPH_NOT_UPLOADED_TO_TEXTURE );
  }
}


void GlyphStatusContainer::GetDeadCharacters( std::vector< unsigned int >& deadList )
{
  deadList.reserve( mDeadCharacters.size() );

  // iterate through the dead character list

  StatusPointerSet::const_iterator endIter =  mDeadCharacters.end();
  for( StatusPointerSet::iterator iter = mDeadCharacters.begin(); iter != endIter; ++iter )
  {
    const GlyphStatus* glyphStatus(*iter);
    deadList.push_back( glyphStatus->GetUniqueId());
  }

}

void GlyphStatusContainer::ClearDeadCharacters()
{
  // iterate through the dead character list, and erase all dead characters from
  // the main lookup.

  StatusPointerSet::const_iterator endIter =  mDeadCharacters.end();
  for( StatusPointerSet::iterator iter = mDeadCharacters.begin(); iter != endIter; ++iter )
  {
    const GlyphStatus* glyphStatus(*iter);
    DALI_ASSERT_DEBUG( glyphStatus->GetRefCount() == 0 && "dead character found with ref > 0?");
    mCharacterLookup.erase( *glyphStatus );
  }

  // clear the dead characters
  mDeadCharacters.clear();
}

bool GlyphStatusContainer::Empty() const
{
  return ( mContainerSize ==TotalAvailableSpace() );
}

unsigned int GlyphStatusContainer::GetSize() const
{
  return mContainerSize;
}

void GlyphStatusContainer::ClearContents()
{
  mCharacterLookup.clear();
  mDeadCharacters.clear();
  mEmptySpace = mContainerSize;
  mTimeStamp = 0;
}

void GlyphStatusContainer::AddDeadCharacter( const GlyphStatus* deadCharacter)
{

  if( mTimeStamp >= GlyphStatus::GetMaximumDeadTime() )
  {
    // the counter has reached the maximum. reset to 0
    ResetTimeStamps();
  }

  // set the time stamp
  deadCharacter->SetDeadTime( mTimeStamp++ );

  // add a pointer to the list, the original data is still held in mCharacterLookup
  mDeadCharacters.insert( deadCharacter );

}
void GlyphStatusContainer::RemoveDeadCharacter( const GlyphStatus* deadCharacter )
{
  DALI_ASSERT_DEBUG( mDeadCharacters.find(deadCharacter) != mDeadCharacters.end() );

  mDeadCharacters.erase( deadCharacter );
}

const GlyphStatus* GlyphStatusContainer::RemoveOldestDeadCharacter()
{
  // oldest item, is the first in the set
  StatusPointerSet::iterator iter = mDeadCharacters.begin();

  // calling code will assert before this ever happens
  // added to prevent a klocwork warning
  if( iter == mDeadCharacters.end() )
  {
    return NULL;
  }

  const GlyphStatus* glyphStatus = (*iter);

  // remove it from the list
  mDeadCharacters.erase( glyphStatus );

  return glyphStatus;
}

void GlyphStatusContainer::InsertCharacterIntoLookup(uint32_t charCode, FontId fontId )
{
  // create a glyph status and set to low quality requested
  GlyphStatus status( charCode, fontId,  GlyphStatus::GLYPH_LOAD_REQUESTED );

  // reference it
  status.IncreaseRefCount();

  // add to the lookup
  mCharacterLookup.insert( status );

#ifdef DEBUG_GLYPH_STATUS_CONTAINER
  DebugGlyphContainer( mContainerSize, mCharacterLookup, mDeadCharacters );
#endif
}

void GlyphStatusContainer::ResetTimeStamps()
{
  // This should only happen when we've had the reference-count of character(s) hit
  // zero 16,384 times (GlyphStatus::GetMaximumDeadTime())

  // Set the time stamp back to zero
  //
  mTimeStamp = 0;

  // The dead list is sorted from oldest -> newest.
  // We go through the list setting the time stamps from Zero -> Number of items
  //
  // E.g. if the list was this
  //
  //   'C'  TimeStamp 3500, 'Y' TimeStamp 5604,  'A' = TimeStamp 8000
  //
  // The list will be set to
  //
  //   'C'  TimeStamp 0, 'Y' TimeStamp 1,  'A' = TimeStamp 2
  //

  StatusPointerSet::const_iterator endIter =  mDeadCharacters.end();
  for( StatusPointerSet::iterator iter = mDeadCharacters.begin(); iter != endIter; ++iter )
  {
    const GlyphStatus* glyphStatus(*iter);
    glyphStatus->SetDeadTime( mTimeStamp++);
  }
}

unsigned int GlyphStatusContainer::TotalAvailableSpace() const
{
  // return number of un-used spaces, and number of spaces
  // that have been used, but arean't any more.
  return ( mEmptySpace + mDeadCharacters.size() );
}

} // namespace Internal

} // namespace Dali

