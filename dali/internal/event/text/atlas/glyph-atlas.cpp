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
#include <dali/internal/event/text/atlas/glyph-atlas.h>

// INTERNAL INCLUDES
#include <dali/internal/event/text/generator/text-vertex-generator.h>
#include <dali/internal/event/text/text-format.h>
#include <dali/internal/event/text/special-characters.h>
#include <dali/internal/event/text/atlas/debug/atlas-debug.h>
#include <dali/internal/event/text/atlas/atlas-rank-generator.h>
#include <dali/internal/event/text/atlas/atlas-size.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

GlyphAtlas* GlyphAtlas::New( unsigned int size )
{
  return new GlyphAtlas( size ) ;
}

GlyphAtlas::~GlyphAtlas()
{
}

TextVertexBuffer* GlyphAtlas::AssignText( const Integration::TextArray& text,
                                          const TextFormat &format,
                                          FontId fontId,
                                          FontMetricsInterface& metrics )
{
  // go through each character increasing it's reference count
  // if it doesn't exist, then it will be marked to be loaded at the end of the event cycle
  ReferenceText( text, format, fontId );

  // generate the vertex information
  TextVertexBuffer* textBuffer = TextVertexGenerator::Generate( text, format, metrics, mAtlas, fontId );

  // assign the texture id
  textBuffer->mTextureId = mTextureId;

  return textBuffer;
}

void GlyphAtlas::TextNoLongerUsed( const Integration::TextArray& text, const TextFormat &format, FontId fontId )
{
  // go through each character un-referencing it
  for( Integration::TextArray::ConstIterator it = text.Begin(), endIt = text.End(); it != endIt; ++it )
  {
    mGlyphContainer.DecreaseRefCount( *it, fontId );
  }

  if( format.IsUnderLined() )
  {
    mGlyphContainer.DecreaseRefCount( format.GetUnderLineCharacter(), fontId );
  }
}

AtlasRanking GlyphAtlas::GetRanking( const Integration::TextArray& text ,  FontId fontId ) const
{
  return GetAtlasRanking(  text, fontId, mGlyphContainer, Resizable() );
}

unsigned int GlyphAtlas::GetSize() const
{
  return mAtlas.GetSize();
}

void GlyphAtlas::SetTextureId( unsigned int textureId )
{
  mTextureId = textureId;
}

bool GlyphAtlas::HasPendingRequests() const
{
  return (!mRequestList.empty());
}

const GlyphRequestList& GlyphAtlas::GetRequestList() const
{
  return mRequestList;
}

void GlyphAtlas::ClearRequestLists()
{
  mRequestList.clear();
}

bool GlyphAtlas::IsTextLoaded( const Integration::TextArray& text, const TextFormat &format, FontId fontId) const
{
  // check the underline character is needed / loaded first
  if( format.IsUnderLined() )
  {
    Integration::TextArray tempText;
    tempText.PushBack( format.GetUnderLineCharacter() );
    bool loaded = mGlyphContainer.IsTextLoaded( tempText, fontId );

    if( !loaded )
    {
      return false;
    }
  }
  // check the text is loaded
  return mGlyphContainer.IsTextLoaded( text, fontId );
}

void GlyphAtlas::CloneContents( GlyphAtlas* oldAtlas)
{
  // ensure this atlas is empty
  DALI_ASSERT_DEBUG( mGlyphContainer.Empty() );

  // Clear out any dead characters which have a ref count of zero from the old atlas
  // we don't want to re-request these, as they may not be used again
  oldAtlas->ClearDeadCharacters();

  // clone the atlas
  mAtlas.CloneContents( &oldAtlas->mAtlas );

  // clone the glyph container
  mGlyphContainer.CloneContents( oldAtlas->mGlyphContainer );

  // if we get more than one atlas resize event per event cycle we need to store
  // a list of the atlas texture id's that have been resized.
  mTextureIdOfReplacedAtlases = oldAtlas->GetTextureIdOfReplacedAtlas();
  mTextureIdOfReplacedAtlases.push_back( oldAtlas->GetTextureId() );

  // clear the old atlas
  oldAtlas->Clear();

  // do a load request for all characters

  GlyphStatusContainer::StatusSet::iterator endIter = mGlyphContainer.GetStatusSet().end();
  for( GlyphStatusContainer::StatusSet::iterator iter = mGlyphContainer.GetStatusSet().begin(); iter != endIter; ++iter)
  {
    const GlyphStatus& status = (*iter);
    uint32_t charCode = status.GetCharacterCode();
    FontId fontId = status.GetFontId();
    unsigned int xPos=0, yPos=0;
    GetGlyphTexturePosition( charCode, fontId, xPos, yPos );

    RequestToLoadCharacter( charCode, xPos, yPos, fontId, GlyphResourceRequest::LOW_QUALITY );
    status.SetLoadStatus( GlyphStatus::GLYPH_LOAD_REQUESTED );
  }

}

void GlyphAtlas::ClearDeadCharacters()
{
  // Dead characters are characters which are in the GL Texture
  // but have a reference count of zero.
  // go through each dead character, marking them as de-allocated in the atlas

  Integration::TextArray deadCharacters;
  mGlyphContainer.GetDeadCharacters( deadCharacters );

  for( Integration::TextArray::ConstIterator it = deadCharacters.Begin(), endIt = deadCharacters.End(); it != endIt; ++it )
  {
    mAtlas.Remove( *it );
  }

  // clear the dead characters from the glyph container
  mGlyphContainer.ClearDeadCharacters();
}

void GlyphAtlas::Clear()
{
  mGlyphContainer.ClearContents();
}

bool GlyphAtlas::HasReplacedTexture( unsigned int textureId )
{
  for( std::size_t i = 0, count = mTextureIdOfReplacedAtlases.size(); i < count; ++i )
  {
    if( mTextureIdOfReplacedAtlases[i] == textureId )
    {
      return true;
    }
  }
  return false;
}

void GlyphAtlas::GlyphUpLoadedToTexture( uint32_t charCode, FontId fontId )
{
  const GlyphStatus* status = mGlyphContainer.FindGlyphStatus( charCode, fontId );
  if( !status )
  {
    // the character is no longer used
    return;
  }
  status->SetTextureStatus( GlyphStatus::GLYPH_UPLOADED_TO_TEXTURE );
}

void GlyphAtlas::GlyphLoadedFromFile( uint32_t charCode, FontId fontId, Quality loadStatus )
{
  unsigned int xPos, yPos;

  // Get texture position
  bool inUse = GetGlyphTexturePosition( charCode, fontId, xPos, yPos );

  // check the glyph is still used.
  // it is possible in between a load request, and load complete that character is no longer required.
  const GlyphStatus* status = mGlyphContainer.FindGlyphStatus( charCode, fontId );
  if( !status || !inUse )
  {
    // the character is no longer used
    return;
  }

  DALI_ASSERT_DEBUG( status->IsLoadRequested() );

  if( loadStatus == HIGH_QUALITY_LOADED )
  {
    status->SetLoadStatus( GlyphStatus::GLYPH_HIGH_QUALITY_LOADED );
  }
  else
  {
    // low quality loaded, high quality in progress
    status->SetLoadStatus( GlyphStatus::GLYPH_LOW_QUALITY_LOADED_HIGH_REQUESTED );
  }
}

bool GlyphAtlas::GetGlyphTexturePosition( uint32_t charCode, FontId fontId, unsigned int &xPos, unsigned int &yPos) const
{
  // check if the character is still in use
  const GlyphStatus* status = mGlyphContainer.FindGlyphStatus(  charCode, fontId );
  if( status == NULL )
  {
    return false;
  }
  // atlas stores a unique id which is a combination of character charCode and font id.
  unsigned int uniqueId = GlyphStatus::GetEncodedValue( charCode, fontId );

  mAtlas.GetXYPosition( uniqueId, xPos, yPos);

  return true;
}

unsigned int GlyphAtlas::GetTextureId() const
{
  return mTextureId;
}

GlyphResourceObserver::TextureState GlyphAtlas::GetTextureState()
{
  if( mTextureIdOfReplacedAtlases.empty() )
  {
    return GlyphResourceObserver::NO_CHANGE;
  }
  else
  {
    return GlyphResourceObserver::TEXTURE_RESIZED;
  }
}

void GlyphAtlas::GetNewTextureId( std::vector<unsigned int>& oldTextureIds , unsigned int& newTextureId )
{
  oldTextureIds = mTextureIdOfReplacedAtlases;
  newTextureId = mTextureId;

  mTextureIdOfReplacedAtlases.clear();
}

void GlyphAtlas::ReferenceText( const Integration::TextArray& text,
                                const TextFormat &format,
                                FontId fontId)
{

  // go through each character, if it exists increase it's ref count.
  for( Integration::TextArray::ConstIterator it = text.Begin(), endIt = text.End(); it != endIt; ++it )
  {
    // this will automatically load the character if it doesn't exist
    IncreaseGlyphRefCount( *it, fontId );
  }

  if( format.IsUnderLined() )
  {
    IncreaseGlyphRefCount( format.GetUnderLineCharacter(), fontId );
  }
}

void GlyphAtlas::IncreaseGlyphRefCount( uint32_t charCode, FontId fontId)
{
  if( charCode < SpecialCharacters::FIRST_VISIBLE_CHAR )
  {
    return;
  }

  const GlyphStatus* status = mGlyphContainer.FindGlyphStatus(  charCode, fontId );
  if( status == NULL )
  {
    // insert the character in to the glyph container / atlas
    InsertNewCharacter( charCode , fontId );

    unsigned int xPos=0, yPos=0;
    GetGlyphTexturePosition( charCode, fontId, xPos, yPos );

    // queue a request for a low quality version of the character
    // if the distance field is cached for this character then a high
    // quality version will be returned
    RequestToLoadCharacter( charCode, xPos, yPos, fontId, GlyphResourceRequest::LOW_QUALITY );
  }
  else
  {
    // character is already in-use, so increase its reference count
    mGlyphContainer.IncreaseRefCount( charCode, fontId );
  }
}

void GlyphAtlas::InsertNewCharacter( uint32_t charCode , FontId fontId )
{
  // The AtlasManager will never allow more characters to fit in the
  // atlas than is available.
  //
  // However in order to fit new characters in the Atlas,
  // 'dead' characters with a ref-count of zero may need replacing.

  // atlas stores a unique id which is a combination of character code and font id.
  unsigned int uniqueId = GlyphStatus::GetEncodedValue( charCode, fontId );
  unsigned int deadUniqueId(0);

  // insert the character into the container and see if it's replacing a dead character
  GlyphStatusContainer::InsertResult insertResult;

  insertResult =  mGlyphContainer.InsertNewCharacter( charCode, fontId, deadUniqueId );

  if( insertResult == GlyphStatusContainer::INSERTED_OK )
  {
    // insert in to the layout atlas
    mAtlas.Insert( uniqueId );
  }
  else // insertResult == REPLACE_DEAD_CHARACTER
  {
    // remove the dead character  from the atlas
    mAtlas.Remove( deadUniqueId );

    // insert the new character into the atlas
    mAtlas.Insert( uniqueId );
  }
}

void GlyphAtlas::RequestToLoadCharacter(uint32_t charCode, unsigned int xPos, unsigned int yPos, FontId fontId, GlyphResourceRequest::GlyphQuality quality )
{
  // find the request list with matching font id
  // if it's not found it will be created
  GlyphResourceRequest& resourceRequest( GetResourceRequest( fontId, quality ));

  // add the character to the request list - if it's not already present
  resourceRequest.InsertCharacter( charCode, xPos, yPos );
}

GlyphResourceRequest& GlyphAtlas::GetResourceRequest( FontId fontId, GlyphResourceRequest::GlyphQuality quality )
{
  // search for the resource request, using font id, and quality

  for( std::size_t i = 0, count = mRequestList.size(); i < count;  ++i)
  {
    GlyphResourceRequest& resourceRequest( mRequestList[ i ] );

    if( (resourceRequest.GetFontId() == fontId ) &&
        (resourceRequest.GetQuality() == quality ) )
    {
      return resourceRequest;
    }
  }
  // doesn't exist, so create a new one
  mRequestList.push_back( GlyphResourceRequest(fontId, quality));

  return mRequestList[ mRequestList.size()-1 ];
}

bool GlyphAtlas::Resizable() const
{
  return (GetSize() <  GlyphAtlasSize::GetMaxSize() );
}

TextureIdList GlyphAtlas::GetTextureIdOfReplacedAtlas()
{
  return mTextureIdOfReplacedAtlases;
}

GlyphAtlas::GlyphAtlas( unsigned int size)
:mGlyphContainer( GlyphAtlasSize::GetAtlasCharacterCount( size ) ),
 mAtlas( size, GlyphAtlasSize::GetBlockSize() ),
 mTextureId(0)
{

}

} // namespace Internal

} // namespace Dali
