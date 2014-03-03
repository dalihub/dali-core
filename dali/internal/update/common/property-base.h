#ifndef __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_BASE_H__
#define __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_BASE_H__

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

// EXTERNAL INCLUDES
#include <cstddef>

// INTERNAL INCLUDES
#include <dali/public-api/object/property.h>
#include <dali/internal/common/type-abstraction.h>
#include <dali/internal/event/common/property-input-impl.h>
#include <dali/internal/update/common/scene-graph-buffers.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

/**
 * Polymorphic base class for scene-graph properties, held by Nodes etc.
 */
class PropertyBase : public PropertyInputImpl
{
public:

  /**
   * Default constructor.
   */
  PropertyBase();

  /**
   * Virtual destructor.
   */
  virtual ~PropertyBase();

  /**
   * Reset the property to a base value; only required if the property is animated.
   * @param [in] currentBufferIndex The current buffer index.
   */
  virtual void ResetToBaseValue(BufferIndex updateBufferIndex) = 0;

  /**
   * @copydoc Dali::Internal::PropertyInputImpl::InputChanged()
   */
  virtual bool InputChanged() const
  {
    return !IsClean();
  }

  /**
   * Query whether the property has changed.
   * @return True if the property has not changed.
   */
  virtual bool IsClean() const = 0;

private:

  // Undefined
  PropertyBase(const PropertyBase& property);

  // Undefined
  PropertyBase& operator=(const PropertyBase& rhs);
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_BASE_H__
