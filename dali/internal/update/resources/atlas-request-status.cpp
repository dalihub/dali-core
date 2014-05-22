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
#include <dali/internal/update/resources/atlas-request-status.h>

// INTERNAL INCLUDES
#include <dali/integration-api/glyph-set.h>

using namespace Dali::Integration;
using std::make_pair;

namespace Dali
{
namespace Internal
{

AtlasRequestStatus::AtlasRequestStatus()
{
}

AtlasRequestStatus::~AtlasRequestStatus()
{
}

void AtlasRequestStatus::CheckAndSaveTextRequest( ResourceId id, const ResourceTypePath& typePath )
{
  if( typePath.type->id == ResourceText )
  {
    const TextResourceType* textResource = static_cast<const TextResourceType*>(typePath.type);
    ResourceId atlasId = textResource->mTextureAtlasId;

    AtlasStatusIter iter = mAtlasStatus.find(atlasId);
    if( iter == mAtlasStatus.end() )
    {
      // Create new one
      GlyphLoadStatusContainer glyphStatusMap;
      glyphStatusMap.insert(make_pair(id, RESOURCE_LOADING));
      mAtlasStatus.insert(make_pair(atlasId, glyphStatusMap));
    }
    else
    {
      // append
      iter->second.insert(make_pair(id, RESOURCE_LOADING));
    }
  }
}

void AtlasRequestStatus::Update( ResourceId id, ResourceId atlasId, LoadStatus loadStatus )
{
  AtlasStatusIter iter = mAtlasStatus.find(atlasId);
  if( iter != mAtlasStatus.end() )
  {
    GlyphLoadStatusContainer& glyphRequests = iter->second;
    GlyphLoadStatusIter glyphIter = glyphRequests.find(id);
    DALI_ASSERT_DEBUG( glyphIter != glyphRequests.end() );
    if( glyphIter != glyphRequests.end() )
    {
      glyphIter->second = loadStatus;
    }
  }
}

bool AtlasRequestStatus::IsLoadComplete( ResourceId atlasId )
{
  bool complete = false;
  if( ! mAtlasStatus.empty() )
  {
    AtlasStatusIter iter = mAtlasStatus.find(atlasId);
    if( iter != mAtlasStatus.end() )
    {
      GlyphLoadStatusContainer& glyphRequests = iter->second;
      if( ! glyphRequests.empty() )
      {
        complete = true;
        for( GlyphLoadStatusIter glyphIter = glyphRequests.begin(), glyphEnd = glyphRequests.end() ;
             glyphIter != glyphEnd ;
             glyphIter++)
        {
          if(glyphIter->second != RESOURCE_COMPLETELY_LOADED)
          {
            complete = false;
            break;
          }
        }
      }
    }
  }
  return complete;
}

LoadStatus AtlasRequestStatus::GetLoadStatus( ResourceId atlasId )
{
  // status initially set to loaded
  // if a partially loaded glyphset is encountered it is set to RESOURCE_PARTIALLY_LOADED
  // if a loading glyphset is found RESOURCE_LOADING is returned instantly
  LoadStatus loadStatus = RESOURCE_COMPLETELY_LOADED;

  if( ! mAtlasStatus.empty() )
  {
    AtlasStatusIter iter = mAtlasStatus.find(atlasId);
    if( iter != mAtlasStatus.end() )
    {
      GlyphLoadStatusContainer& glyphRequests = iter->second;
      if( ! glyphRequests.empty() )
      {
        for( GlyphLoadStatusIter glyphIter = glyphRequests.begin(), glyphEnd = glyphRequests.end() ;
                     glyphIter != glyphEnd ;
                     glyphIter++)
        {
          if( glyphIter->second == RESOURCE_LOADING )
          {
            // return if a glyphset is still being loaded
            return RESOURCE_LOADING;
          }
          else if( glyphIter->second == RESOURCE_PARTIALLY_LOADED )
          {
            loadStatus = RESOURCE_PARTIALLY_LOADED;
          }
        }
      }
    }
  }
  return loadStatus;
}


void AtlasRequestStatus::Remove( ResourceId atlasId )
{
  mAtlasStatus.erase(atlasId);
}

} // Internal
} // Dali
