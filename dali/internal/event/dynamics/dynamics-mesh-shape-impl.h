#ifndef __DYNAMICS_MESH_SHAPE_IMPL_H_
#define __DYNAMICS_MESH_SHAPE_IMPL_H_

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

// BASE CLASS HEADERS
#include <dali/internal/event/dynamics/dynamics-shape-impl.h>

// INTERNAL HEADERS
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/internal/event/modeling/modeling-declarations.h>


namespace Dali
{

namespace Internal
{

class Mesh;

/**
 * A triangular mesh
 */
class DynamicsMeshShape : public DynamicsShape
{
public:
  /**
   * Constructor
   * @copydoc Dali::DynamicsShape::NewMesh
   */
  DynamicsMeshShape(Mesh& mesh);

protected:
  /**
   * Destructor
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~DynamicsMeshShape();

private:
  // unimplemented copy constructor and assignment operator
  DynamicsMeshShape(const DynamicsMeshShape&);
  DynamicsMeshShape& operator=(const DynamicsMeshShape&);

public:
  /**
   * Get an axis aligned bounding box for this shape
   * @return An axis aligned bounding box for this shape
   */
  virtual Vector3 GetAABB() const;

  /**
   * Get the mesh
   */
  Mesh& GetMesh()
  {
    return mMesh;
  }

private:
  Mesh& mMesh;
}; // class DynamicsMeshShape

} // namespace Internal

} // namespace Dali

#endif /* __DYNAMICS_MESH_SHAPE_IMPL_H_ */
