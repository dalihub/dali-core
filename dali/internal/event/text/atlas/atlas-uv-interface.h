#ifndef __DALI_INTERNAL_ATLAS_UV_INTERFACE_H__
#define __DALI_INTERNAL_ATLAS_UV_INTERFACE_H__

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

// INTERNAL INCLUDES
#include <dali/internal/render/common/uv-rect.h>

namespace Dali
{

namespace Internal
{

/**
 * Abstract interface for requesting the UV co-ordinates of an item held in an Atlas.
 *
 */
class AtlasUvInterface
{

public:

  /**
   * Get the uv-coordinates of an item in the atlas.
   * @param itemIndex unique index of an item in the atlas. E.g. a character code
   * @return uv co-ordinates of the item.
   */
  virtual UvRect GetUvCoordinates( unsigned int itemIndex ) const = 0;


protected:

  /**
   * Constructor
   */
  AtlasUvInterface()
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~AtlasUvInterface()
  {
  }

private:

  // Undefined copy constructor.
  AtlasUvInterface( const AtlasUvInterface& );

  // Undefined assignment operator.
  AtlasUvInterface& operator=( const AtlasUvInterface& );

};


} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ATLAS_UV_INTERFACE_H__
