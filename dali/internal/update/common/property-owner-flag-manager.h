#ifndef DALI_INTERNAL_SCENEGRAPH_PROPERTY_OWNER_FLAG_MANAGER_H
#define DALI_INTERNAL_SCENEGRAPH_PROPERTY_OWNER_FLAG_MANAGER_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
 */

// INTERNAL INCLDUES

namespace Dali::Internal::SceneGraph
{
class PropertyOwner;

/**
 * Abstract interface for passing a PropertyOwner's dirty flag management.
 */
class PropertyOwnerFlagManager
{
public:
  /**
   * Request to call RequestResetUpdated() for this frame at the end of update/render loop.
   * @param[in] owner The property owner that will be called ResetUpdated().
   */
  virtual void RequestResetUpdated(const PropertyOwner& owner) = 0;

  /**
   * Discard property owner. Actuall memory will be released after 2 update/render loop.
   * @param[in] discardedOwner The property owner that discarded this frame.
   */
  virtual void DiscardPropertyOwner(PropertyOwner* discardedOwner) = 0;

protected:
  /**
   * Destructor. Protected as no derived class should ever be deleted
   * through a reference to this pure abstract interface.
   */
  virtual ~PropertyOwnerFlagManager() = default;
};

} // namespace Dali::Internal::SceneGraph

#endif // DALI_INTERNAL_SCENEGRAPH_PROPERTY_OWNER_FLAG_MANAGER_H
