#ifndef DALI_INTERNAL_SCENEGRAPH_RESETTER_MANAGER_H
#define DALI_INTERNAL_SCENEGRAPH_RESETTER_MANAGER_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/update/common/property-resetter.h>

namespace Dali::Internal::SceneGraph
{
class Node;
class Renderer;

/**
 * Abstract interface for passing a Resetter object add.
 */
class ResetterManager
{
public:
  /**
   * Add a property resetter. ResetterManager takes ownership of the object.
   * It will be killed by ResetterManager when the associated animator or
   * constraint has finished; or the property owner of the property is destroyed.
   */
  virtual void AddPropertyResetter(OwnerPointer<PropertyResetterBase>& propertyResetter) = 0;

  /**
   * Add a node resetter. ResetterManager takes ownership of the object.
   * It will be killed by ResetterManager when the node is disconnected from the scene graph;
   * or when the node is destroyed.
   */
  virtual void AddNodeResetter(const Node& node) = 0;

  /**
   * Add a renderer resetter. ResetterManager takes ownership of the object.
   * It will be killed by ResetterManager when the renderer is destroyed
   */
  virtual void AddRendererResetter(const Renderer& renderer) = 0;

protected:
  /**
   * Destructor. Protected as no derived class should ever be deleted
   * through a reference to this pure abstract interface.
   */
  virtual ~ResetterManager() = default;
};

} // namespace Dali::Internal::SceneGraph

#endif //DALI_INTERNAL_SCENEGRAPH_RESETTER_MANAGER_H
