#ifndef DALI_INTERNAL_SCENE_GRAPH_PROPERTY_BASE_H
#define DALI_INTERNAL_SCENE_GRAPH_PROPERTY_BASE_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <cstddef>

// INTERNAL INCLUDES
#include <dali/internal/common/type-abstraction.h>
#include <dali/internal/event/common/property-input-impl.h>
#include <dali/public-api/object/property.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class ResetterManager;

/**
 * Polymorphic base class for scene-graph properties, held by Nodes etc.
 */
class PropertyBase : public PropertyInputImpl
{
public:
  /**
   * Default constructor.
   */
  PropertyBase() = default;

  /**
   * Virtual destructor.
   */
  ~PropertyBase() = default;

  /**
   * @brief Registers a resetter manager for whole property bases.
   *
   * @param [in] manager The manager to register.
   */
  static void RegisterResetterManager(ResetterManager& manager);

  /**
   * @brief Unregisters a resetter manager.
   */
  static void UnregisterResetterManager();

  /**
   * @brief Request to reset the property to a base value to registered ResetterManager.
   */
  void RequestResetToBaseValue();

  /**
   * Reset the property to a base value; only required if the property is animated.
   */
  virtual void ResetToBaseValue() = 0;

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

#endif // DALI_INTERNAL_SCENE_GRAPH_PROPERTY_BASE_H
