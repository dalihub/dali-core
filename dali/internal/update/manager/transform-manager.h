
#ifndef TRANSFORM_MANAGER_H_
#define TRANSFORM_MANAGER_H_

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/vector3.h>
#include <dali/internal/update/manager/free-list.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

/**
 * Struct to store the animatable part of a component (Scale, orientation and position)
 */
struct TransformComponentAnimatable
{
  TransformComponentAnimatable()
  :mScale(1.0f,1.0f,1.0f),
   mOrientation(1.0f,0.0f,0.0f,0.0f),
   mPosition(0.0f,0.0f,0.0f)
  {}

  Vector3 mScale;
  Quaternion mOrientation;
  Vector3 mPosition;
};

/**
 * Struct to store the non-animatable part of a component (AnchorPoint and ParentOrigin)
 */
struct TransformComponentStatic
{
  TransformComponentStatic()
  :mAnchorPoint(0.5f,0.5f,0.5f),
   mParentOrigin(0.0f,0.0f,0.5f)
  {}

  Vector3 mAnchorPoint;
  Vector3 mParentOrigin;
};

enum InheritanceMode
{
  DONT_INHERIT_TRANSFORM  = 0,
  INHERIT_POSITION        = 1,
  INHERIT_SCALE           = 2,
  INHERIT_ORIENTATION     = 4,
  INHERIT_ALL             = INHERIT_POSITION | INHERIT_SCALE | INHERIT_ORIENTATION,
};

enum TransformManagerProperty
{
  TRANSFORM_PROPERTY_POSITION = 0,
  TRANSFORM_PROPERTY_SCALE,
  TRANSFORM_PROPERTY_ANCHOR_POINT,
  TRANSFORM_PROPERTY_PARENT_ORIGIN,
  TRANSFORM_PROPERTY_SIZE,
  TRANSFORM_PROPERTY_WORLD_POSITION,
  TRANSFORM_PROPERTY_WORLD_SCALE,
  TRANSFORM_PROPERTY_WORLD_ORIENTATION,
  TRANSFORM_PROPERTY_WORLD_MATRIX,
  TRANSFORM_PROPERTY_COUNT,
};

typedef unsigned int TransformId;
static const TransformId INVALID_TRANSFORM_ID = -1;

} //SceneGraph
} //Internal

// Allow TransformComponentAnimatable to be treated as a POD type
template <> struct TypeTraits< Dali::Internal::SceneGraph::TransformComponentAnimatable >: public Dali::BasicTypes< Dali::Internal::SceneGraph::TransformComponentAnimatable > { enum { IS_TRIVIAL_TYPE = true }; };

namespace Internal
{

namespace SceneGraph
{

/**
 * Transform manager computes the local to world transformations
 * of all the nodes in the scene. All the transformation data is stored contiguously
 * in memory which minimizes cache misses during updates
 */
class TransformManager
{
public:

  /**
   * Default constructor
   */
  TransformManager();

  /**
   * Class destructor
   */
  ~TransformManager();

  /**
   * Add a new transform component to the manager
   * @return A TransformId used to access the component
   */
  TransformId CreateTransform();

  /**
   * Removes an existing transform component
   * @param[in] id Id of the transform to remove
   */
  void RemoveTransform(TransformId id);

  /**
   * Sets the parent transform of an existing component
   * @param[in] id Id of the transform
   * @param[in] parentId Id of the new parent
   */
  void SetParent( TransformId id, TransformId parentId );

  /**
   * Gets the world transform matrix of an exisiting transform component
   * @param[in] id Id of the transform component
   * @return The local to world transformation matrix of the component
   */
  const Matrix& GetWorldMatrix( TransformId id ) const;

  /**
   * Gets the world transform matrix of an exisiting transform component
   * @param[in] id Id of the transform component
   * @return The local to world transformation matrix of the component
   */
  Matrix& GetWorldMatrix( TransformId id );

  /**
   * Checks if the local transform was updated in the last Update
   * @param[in] id Id of the transform
   * @return true if local matrix changed in the last update, false otherwise
   */
  bool IsLocalMatrixDirty( TransformId id ) const
  {
    return mLocalMatrixDirty[mIds[id]];
  }

  /**
   * Sets position inheritance mode.
   * @param[in] id Id of the transform
   * @param[in] inherit True if position is inherited from parent, false otherwise
   */
  void SetInheritPosition( TransformId id, bool inherit );

  /**
   * Sets scale inheritance mode.
   * @param[in] id Id of the transform
   * @param[in] inherit True if scale is inherited from parent, false otherwise
   */
  void SetInheritScale( TransformId id, bool inherit );

  /**
   * Sets orientation inheritance mode.
   * @param[in] id Id of the transform
   * @param[in] inherit True if orientation is inherited from parent, false otherwise
   */
  void SetInheritOrientation( TransformId id, bool inherit );

  /**
   * Recomputes all world transform matrices
   */
  void Update();

  /**
   * Resets all the animatable properties to its base value
   */
  void ResetToBaseValue();

  /**
   * Get the value of a Vector3 property
   * @param[in] id Id of the transform component
   * @param[in] property The property
   */
  Vector3& GetVector3PropertyValue( TransformId id, TransformManagerProperty property );

  /**
   * Get the value of a Vector3 property
   * @param[in] id Id of the transform component
   * @param[in] property The property
   */
  const Vector3& GetVector3PropertyValue( TransformId id, TransformManagerProperty property ) const;

  /**
   * Get the value of a component of Vector3 property
   * @param[in] id Id of the transform component
   * @param[in] property The property
   * param[in] component The component (0,1,2)
   */
  const float& GetVector3PropertyComponentValue(TransformId id, TransformManagerProperty property, unsigned int component ) const;

  /**
   * Set the value of a Vector3 property
   * @param[in] id Id of the transform component
   * @param[in] property The property
   * @param[in] value The new value
   */
  void SetVector3PropertyValue( TransformId id, TransformManagerProperty property, const Vector3& value );

  /**
   * Set the value of a component of a Vector3 property
   * @param[in] id Id of the transform component
   * @param[in] property The property
   * @param[in] value The new value
   * param[in] component The component (0,1,2)
   */
  void SetVector3PropertyComponentValue( TransformId id, TransformManagerProperty property, float value, unsigned int component );

  /**
   * Bakes the value of a Vector3 property
   * @param[in] id Id of the transform component
   * @param[in] property The property
   * @param[in] value The new value
   */
  void BakeVector3PropertyValue( TransformId id, TransformManagerProperty property, const Vector3& value );

  /**
   * Bakes the value of a Vector3 property relative to the current value
   * @param[in] id Id of the transform component
   * @param[in] property The property
   * @param[in] value The new value
   */
  void BakeRelativeVector3PropertyValue( TransformId id, TransformManagerProperty property, const Vector3& value );

  /**
   * Bakes the value of a Vector3 property by multiplying the current value and the new value
   * @param[in] id Id of the transform component
   * @param[in] property The property
   * @param[in] value The new value
   */
  void BakeMultiplyVector3PropertyValue( TransformId id, TransformManagerProperty property, const Vector3& value );

  /**
   * Bakes the value of a component of Vector3 property
   * @param[in] id Id of the transform component
   * @param[in] property The property
   * @param[in] value The new value
   */
  void BakeVector3PropertyComponentValue( TransformId id, TransformManagerProperty property, float value, unsigned int component );

  /**
   * Bakes the value of the x component of Vector3 property
   * @param[in] id Id of the transform component
   * @param[in] property The property
   * @param[in] value The new value
   */
  void BakeXVector3PropertyValue( TransformId id, TransformManagerProperty property, float value );

  /**
   * Bakes the value of the y component of Vector3 property
   * @param[in] id Id of the transform component
   * @param[in] property The property
   * @param[in] value The new value
   */
  void BakeYVector3PropertyValue( TransformId id, TransformManagerProperty property, float value );

  /**
   * Bakes the value of the z component of Vector3 property
   * @param[in] id Id of the transform component
   * @param[in] property The property
   * @param[in] value The new value
   */
  void BakeZVector3PropertyValue( TransformId id, TransformManagerProperty property, float value );

  /**
   * Get the value of a quaternion property
   * @param[in] id Id of the transform component
   */
  Quaternion& GetQuaternionPropertyValue( TransformId id );

  /**
   * Get the value of a quaternion property
   * @param[in] id Id of the transform component
   */
  const Quaternion& GetQuaternionPropertyValue( TransformId id ) const;

  /**
   * Set the value of a quaternion property
   * @param[in] id Id of the transform component
   * @param[in] q The new value
   */
  void SetQuaternionPropertyValue( TransformId id, const Quaternion& q );

  /**
   * Bake the value of a quaternion property
   * @param[in] id Id of the transform component
   * @param[in] q The new value
   */
  void BakeQuaternionPropertyValue( TransformId id, const Quaternion& q );

  /**
   * Bake the value of a quaternion property relative to its current value
   * @param[in] id Id of the transform component
   * @param[in] q The new value
   */
  void BakeRelativeQuaternionPropertyValue( TransformId id, const Quaternion& q );

private:

  //Helper struct to order components
  struct SOrderItem
  {
    bool operator<(const SOrderItem& item) const {return level < item.level;}

    TransformId  id;
    unsigned int level;
  };

  /**
   * Swaps two components in the vectors
   * @param[in] i Index of a component
   * @param[in] j Index of a component
   */
  void SwapComponents( unsigned int i, unsigned int j );

  /**
   * Reorders components in hierarchical order so update can iterate sequentially
   * updating the world transforms
   */
  void ReorderComponents();

  unsigned int mComponentCount;                                ///< Total number of components
  FreeList mIds;                                               ///< FreeList of Ids
  Vector<TransformComponentAnimatable> mTxComponentAnimatable; ///< Animatable part of the components
  Vector<TransformComponentStatic> mTxComponentStatic;         ///< Static part of the components
  Vector<unsigned int> mInheritanceMode;                       ///< Inheritance mode of the components
  Vector<TransformId> mComponentId;                            ///< Ids of the components
  Vector<Vector3> mSize;                                       ///< Size of the components
  Vector<TransformId> mParent;                                 ///< Parent of the components
  Vector<Matrix> mWorld;                                       ///< Local to world transform of the components
  Vector<Matrix> mLocal;                                       ///< Local to parent space transform of the components
  Vector<TransformComponentAnimatable> mTxComponentAnimatableBaseValue;  ///< Base values for the animatable part of the components
  Vector<Vector3> mSizeBase;                                             ///< Base value for the size of the components
  Vector<bool> mComponentDirty;    ///< 1u if some of the parts of the component has changed in this frame, 0 otherwise
  Vector<bool> mLocalMatrixDirty;  ///< 1u if the local matrix has been updated in this frame, 0 otherwise
  Vector<SOrderItem> mOrderedComponents;   ///< Used to reorder components when hierarchy changes
  bool mReorder;                           ///< Flag to determine if the components have to reordered in the next Update

};

} //namespace SceneGraph

} //namespace Internal


} //namespace Dali


#endif /* TRANSFORM_MANAGER_H_ */
