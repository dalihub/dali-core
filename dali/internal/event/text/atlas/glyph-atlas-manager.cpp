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
#include <dali/internal/event/text/atlas/glyph-atlas-manager.h>

// INTERNAL INCLUDES
#include <dali/internal/event/text/atlas/atlas-ranking.h>
#include <dali/internal/event/text/atlas/atlas-size.h>
#include <dali/internal/event/text/atlas/debug/atlas-debug.h>
#include <dali/internal/common/text-array.h>
#include <dali/integration-api/resource-declarations.h>

namespace Dali
{

namespace Internal
{


GlyphAtlasManager::GlyphAtlasManager( const FontLookupInterface& fontLookup )
: mGlyphResourceManager( fontLookup ),
  mAtlasesChanged(false)
{
}

GlyphAtlasManager::~GlyphAtlasManager()
{
  // Atlases automatically get destroyed.
}

TextVertexBuffer* GlyphAtlasManager::TextRequired( const TextArray& text ,
                                                   const TextFormat& format,
                                                   FontMetricsInterface& metrics )
{
  // get the font id
  FontId fontId = metrics.GetFontId();

  AtlasRanking bestRank( text.size() );

  // find the atlas which is best suited to displaying the text string
  GlyphAtlas* atlas  = FindAtlas( text, format, fontId, bestRank);

  DALI_ASSERT_DEBUG( atlas && "Find atlas should always return a valid atlas." );

  // if the atlas is full, create a new larger one
  if( bestRank.GetSpaceStatus() == AtlasRanking::FULL_CAN_BE_RESIZED )
  {
    atlas = CreateLargerAtlas( atlas );
  }

  // assign the text to it
  return atlas->AssignText( text, format, fontId, metrics );
}

void GlyphAtlasManager::TextNotRequired( const TextArray& text,
                                         const TextFormat& format,
                                         FontId fontId,
                                         unsigned int textureId )
{
  GlyphAtlas& atlas = GetAtlas( textureId );

  atlas.TextNoLongerUsed( text, format, fontId  );
}


bool GlyphAtlasManager::IsTextLoaded( const TextArray& text,
                                      const TextFormat& format,
                                      FontId fontId,
                                      unsigned int textureId ) const
{

  GlyphAtlas& atlas = GetAtlas( textureId );

  return atlas.IsTextLoaded( text, format, fontId );
}

void GlyphAtlasManager::AddTextObserver( TextObserver& observer)
{
  mGlyphResourceManager.AddTextObserver( observer );
}

void GlyphAtlasManager::RemoveTextObserver( TextObserver& observer)
{
  mGlyphResourceManager.RemoveTextObserver( observer );
}

void GlyphAtlasManager::AddTextureObserver( GlyphTextureObserver& observer)
{
  DALI_ASSERT_DEBUG( std::find(mTextureObservers.Begin(), mTextureObservers.End(), &observer) == mTextureObservers.End() && "Observer already exists" );
  mTextureObservers.PushBack(&observer);
}

void GlyphAtlasManager::RemoveTextureObserver( GlyphTextureObserver& observer)
{
  TextureObserverList::Iterator iter = std::find(mTextureObservers.Begin(), mTextureObservers.End(), &observer);

  DALI_ASSERT_DEBUG( iter != mTextureObservers.End() && "Observer missing" );
  mTextureObservers.Erase(iter);
}

void GlyphAtlasManager::SendTextRequests()
{
  if( mAtlasesChanged )
  {
    NotifyAtlasObservers();
    mAtlasesChanged = false;
  }

  // this is called at the end of an event cycle.
  // Each atlas builds up a list of text load requests
  // We grab the requests here and pass them on to glyph-resource-manager
  for( std::size_t i = 0, atlasCount = mAtlasList.Size() ; i < atlasCount ; ++i )
  {
    GlyphAtlas& atlas( *mAtlasList[i] );

    if( atlas.HasPendingRequests() )
    {
      const GlyphRequestList& requestList( atlas.GetRequestList() );

      mGlyphResourceManager.AddRequests( requestList, atlas, atlas.GetTextureId() );

      atlas.ClearRequestLists();
    }
  }
}

GlyphLoadObserver& GlyphAtlasManager::GetLoadObserver()
{
  return mGlyphResourceManager;
}

GlyphAtlas* GlyphAtlasManager::CreateAtlas( unsigned int size )
{
  GlyphAtlas* atlas =  GlyphAtlas::New( size  );

  AddAtlas( atlas );

  return atlas;
}

GlyphAtlas* GlyphAtlasManager::FindAtlas( const TextArray& text,
                                          const TextFormat& format,
                                          FontId fontId,
                                          AtlasRanking &bestRank )
{
  // if the text is underlined, add the underline character to the text string
  TextArray searchText( text );
  if( format.IsUnderLined() )
  {
    searchText.push_back( format.GetUnderLineCharacter() );
  }

  if( mAtlasList.Count() == 0 )
  {
    // make sure the initial atlas size holds the requested text.
    unsigned int size = GlyphAtlasSize::GetInitialSize( searchText.size() );

    return CreateAtlas( size );
  }

  // go through each atlas finding the best match
  GlyphAtlas* bestMatch( NULL );

  for( std::size_t i = 0, atlasCount = mAtlasList.Size() ; i < atlasCount ; ++i )
  {
    GlyphAtlas& atlas( *mAtlasList[i] );

    AtlasRanking rank =  atlas.GetRanking( searchText , fontId );

    if( bestRank.HigherRanked( rank ) == false)
    {
      bestMatch = &atlas;
      bestRank = rank;
    }

    if( rank.AllCharactersMatched() )
    {
      // break if an atlas is found which has all the glyphs loaded
      break;
    }
  }
  return bestMatch;
}

void GlyphAtlasManager::AddAtlas( GlyphAtlas* atlas)
{
  // create a texture for the atlas.
  unsigned int textureID = mGlyphResourceManager.CreateTexture( atlas->GetSize() );

  // assign the texture id
  atlas->SetTextureId( textureID );

  mAtlasList.PushBack( atlas );

  // resource manager will inform the atlas when glyphs are loaded or uploaded to a texture
  mGlyphResourceManager.AddObserver( *atlas );
}

void GlyphAtlasManager::RemoveAtlas( GlyphAtlas* atlas  )
{
  for( std::size_t i = 0; i< mAtlasList.Size(); ++i )
  {
    if( mAtlasList[i] == atlas)
    {
      // remove it from the resource manager observer list
      mGlyphResourceManager.RemoveObserver( *atlas );

      // remove the item from the list & delete it
      mAtlasList.Erase( mAtlasList.Begin()+i );
      return;
    }
  }
  DALI_ASSERT_DEBUG( 0 && "atlas not found");
}

GlyphAtlas& GlyphAtlasManager::GetAtlas( unsigned int textureId ) const
{
  for( std::size_t i = 0, atlasCount = mAtlasList.Size() ; i < atlasCount ; ++i )
  {
    GlyphAtlas& atlas( *mAtlasList[i] );

    if( atlas.GetTextureId() == textureId )
    {
      return atlas;
    }

    // check if the texture id is for an old atlas that has been replaced by this atlas
    if( atlas.HasReplacedTexture( textureId ) )
    {
      return atlas;
    }
  }
  DALI_ASSERT_ALWAYS( 0 && "Atlas not found");
}

GlyphAtlas* GlyphAtlasManager::CreateLargerAtlas( GlyphAtlas* atlas )
{
  if( atlas->GetSize() ==  GlyphAtlasSize::GetMaxSize() )
  {
    // @todo implement atlas splitting
    DALI_ASSERT_ALWAYS(0 && "Atlas reached max size");
  }

  // Create a new bigger atlas
  unsigned int biggerSize = GlyphAtlasSize::GetNextSize( atlas->GetSize() );

  GlyphAtlas* newAtlas = GlyphAtlas::New( biggerSize );

  // clone the contents of the old atlas
  newAtlas->CloneContents( atlas );

  // remove the old atlas
  RemoveAtlas( atlas );

  // add the new atlas
  AddAtlas( newAtlas );

  mAtlasesChanged = true;

  return newAtlas;
}

void GlyphAtlasManager::NotifyAtlasObservers()
{
  DALI_LOG_INFO(gTextAtlasLogFilter, Debug::General, "GlyphAtlasManager::NotifyAtlasObservers()\n");

  for( std::size_t i = 0, atlasCount = mAtlasList.Size() ; i < atlasCount ; ++i )
  {
    GlyphAtlas& atlas( *mAtlasList[i] );

    Integration::ResourceId newTexture;
    TextureIdList oldTextures;
    atlas.GetNewTextureId( oldTextures, newTexture );

    // copy this list so, the observers can remove themselves during the call back
    TextureObserverList observerList( mTextureObservers );

    TextureObserverList::Iterator iter( observerList.Begin() );
    TextureObserverList::ConstIterator endIter( observerList.End() );
    for( ; iter != endIter; ++iter )
    {
      GlyphTextureObserver* observer((*iter));
      observer->TextureResized( oldTextures, newTexture );
    }
  }
}

} // namespace Internal

} // namespace Dali
