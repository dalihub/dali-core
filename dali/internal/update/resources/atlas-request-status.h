#ifndef __DALI_INTERNAL_UPDATE_RESOURCES_ATLAS_REQUEST_STATUS_H__
#define __DALI_INTERNAL_UPDATE_RESOURCES_ATLAS_REQUEST_STATUS_H__

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
#include <dali/internal/event/resources/resource-type-path.h>
#include <dali/integration-api/resource-declarations.h>
#include <dali/public-api/common/map-wrapper.h>

namespace Dali
{

namespace Integration
{
class GlyphSet;
}

namespace Internal
{

/**
 * This class stores the loading status of each glyph set associated with each atlas.
 * Although the glyph set resources are transient, their load status is kept here
 * after the request is completed.
 */
class AtlasRequestStatus
{
public:
  // Structures to store the load status for each glyph set of an atlas
  typedef std::pair<Integration::ResourceId, Integration::LoadStatus>   GlyphLoadStatusPair;
  typedef std::map<Integration::ResourceId, Integration::LoadStatus>    GlyphLoadStatusContainer;
  typedef GlyphLoadStatusContainer::iterator                            GlyphLoadStatusIter;

  typedef std::pair<Integration::ResourceId, GlyphLoadStatusContainer>  AtlasStatusPair;
  typedef std::map<Integration::ResourceId, GlyphLoadStatusContainer>   AtlasStatusContainer;
  typedef AtlasStatusContainer::iterator                                AtlasStatusIter;

  /**
   * Constructor
   */
  AtlasRequestStatus();

  /**
   * Destructor
   */
  ~AtlasRequestStatus();

  /**
   * If this is a text request, add it to the atlas status
   * @param[in] id Resource id of a resource to load
   * @param[in] typePath Type of a resource to load
   */
  void CheckAndSaveTextRequest( Integration::ResourceId id, const ResourceTypePath& typePath );

  /**
   * After a text resource load response, update the atlas with it's status
   * @param[in] id Id of the text request
   * @param[in] atlasId The atlas Id
   * @param[in] loadStatus status of the glyph set
   */
  void Update( Integration::ResourceId id, Integration::ResourceId atlasId, Integration::LoadStatus loadStatus );

  /**
   * Check if all the glyph requests have completed
   * @param[in] atlasId Resource id of the atlas
   * @return true if all associated glyph requests have finished loading
   */
  bool IsLoadComplete( Integration::ResourceId atlasId );

  /**
   * Check the current load status of requests in the atlas.
   * If any requests are still loading it will return loading.
   * @param[in] atlasId Resource id of the atlas
   * @return Load status of for the atlas
   */
  Integration::LoadStatus GetLoadStatus( Integration::ResourceId atlasId );

  /**
   * Remove atlas status
   * @param[in] atlasId Resource ID of the atlas
   */
  void Remove( Integration::ResourceId atlasId );

private:
  /** Undefined copy constructor */
  AtlasRequestStatus(const AtlasRequestStatus& rhs);

  /** Undefined assignment operator */
  AtlasRequestStatus& operator=(const AtlasRequestStatus& rhs);

  AtlasStatusContainer mAtlasStatus; ///< glyph request status for each atlas
};

} // Internal
} // Dali

#endif // __DALI_INTERNAL_UPDATE_RESOURCES_ATLAS_REQUEST_STATUS_H__
