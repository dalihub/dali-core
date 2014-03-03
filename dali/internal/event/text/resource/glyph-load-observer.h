#ifndef __DALI_INTERNAL_GLYPH_LOAD_OBSERVER_H__
#define __DALI_INTERNAL_GLYPH_LOAD_OBSERVER_H__

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
#include <dali/integration-api/resource-declarations.h> // for ResourceId
#include <dali/integration-api/glyph-set.h>   // for glyph set

namespace Dali
{

namespace Internal
{

/**
 * Abstract observer class to inform the owner that some glyph have been loaded.
 *
 */
class GlyphLoadObserver
{

public:

  /**
   * Called when a glyphset has been loaded.
   * The resoure id is the ticket id for the original request.
   * @param[in] id resource id
   * @param[in] glyphSet reference to the glyphset
   * @param[in] complete if this is the last glyph set
   */
  virtual void GlyphsLoaded( Integration::ResourceId id, const Integration::GlyphSet& glyphSet, Integration::LoadStatus loadStatus  ) = 0;


protected:

  /**
   * Constructor
   */
  GlyphLoadObserver()
  {

  }

  /**
   * Destructor.
   */
  virtual ~GlyphLoadObserver()
  {

  }

private:

  // Undefined copy constructor.
  GlyphLoadObserver( const GlyphLoadObserver& );

  // Undefined assignment operator.
  GlyphLoadObserver& operator=( const GlyphLoadObserver& );

};


} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_GLYPH_LOAD_OBSERVER_H__
