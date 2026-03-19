#ifndef DALI_INTERNAL_TRANSFORM_MANAGER_H
#define DALI_INTERNAL_TRANSFORM_MANAGER_H

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

// INTERNAL INCLUDES
#include <dali/devel-api/common/free-list.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/vector3.h>

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
  Vector3    mScale;
  Quaternion mOrientation;
  Vector3    mPosition;
};

/**
 * Struct to store the non-animatable part of a component (AnchorPoint and ParentOrigin)
 */
struct TransformComponentStatic
{
  Vector3 mAnchorPoint;
  Vector3 mParentOrigin;
};

enum InheritanceMode
{
  DONT_INHERIT_TRANSFORM = 0,
  INHERIT_POSITION       = 1,
  INHERIT_SCALE          = 2,
  INHERIT_ORIENTATION    = 4,
  INHERIT_ALL            = INHERIT_POSITION | INHERIT_SCALE | INHERIT_ORIENTATION,
};

namespace TransformComponentBitField
{
/**
 * Struct to store the non-animatable bit-field part of a component (InheritMode, DirtyFlags)
 *
 * 0xabcccdee
 * ..a....... = PositionUsesAnhorPoint : 1 bit
 * ...b...... = Ignored : 1 bit
 * ....ccc... = InheritanceMode : 3 bits
 * .......d.. = World matrix dirty flag : 1 bit
 *              1u if the world matrix has been updated in this frame, 0 otherwise
 * ........ee = Component property dirty flag : 2 bit
 *              Follow as animatable property's dirty flag.
 *              Or If we change static component changed, flag become non-zero. Age down at Update time.
 *              Note that we don't replace dirty flag as BAKE even if we call Bake operation.
 *              (Since single dirty flag controls multiple animatable properties ; Position, Size, Scale, Orientation.)
 */
using FlagType = uint8_t;

static constexpr uint32_t COMPONENT_DIRTY_BITS_COUNT            = 2u;
static constexpr uint32_t WORLD_MATRIX_DIRTY_BITS_COUNT         = 1u;
static constexpr uint32_t INHERITANCE_MODE_BITS_COUNT           = 3u;
static constexpr uint32_t IGNORED_BITS_COUNT                    = 1u;
static constexpr uint32_t POSITION_USES_ANCHOR_POINT_BITS_COUNT = 1u;

static_assert(COMPONENT_DIRTY_BITS_COUNT + WORLD_MATRIX_DIRTY_BITS_COUNT + INHERITANCE_MODE_BITS_COUNT + IGNORED_BITS_COUNT + POSITION_USES_ANCHOR_POINT_BITS_COUNT <= 8 * sizeof(FlagType));

static constexpr uint32_t WORLD_MATRIX_DIRTY_SHIFT         = COMPONENT_DIRTY_BITS_COUNT;
static constexpr uint32_t INHERITANCE_MODE_SHIFT           = WORLD_MATRIX_DIRTY_SHIFT + WORLD_MATRIX_DIRTY_BITS_COUNT;
static constexpr uint32_t IGNORED_SHIFT                    = INHERITANCE_MODE_SHIFT + INHERITANCE_MODE_BITS_COUNT;
static constexpr uint32_t POSITION_USES_ANCHOR_POINT_SHIFT = IGNORED_SHIFT + IGNORED_BITS_COUNT;

static constexpr uint32_t COMPONENT_DIRTY_MASK            = ((1u << COMPONENT_DIRTY_BITS_COUNT) - 1u);
static constexpr uint32_t WORLD_MATRIX_DIRTY_MASK         = ((1u << WORLD_MATRIX_DIRTY_BITS_COUNT) - 1u);
static constexpr uint32_t INHERITANCE_MODE_MASK           = ((1u << INHERITANCE_MODE_BITS_COUNT) - 1u);
static constexpr uint32_t IGNORED_MASK                    = ((1u << IGNORED_BITS_COUNT) - 1u);
static constexpr uint32_t POSITION_USES_ANCHOR_POINT_MASK = ((1u << POSITION_USES_ANCHOR_POINT_BITS_COUNT) - 1u);

/**
 * @brief Helper codes to set / get variables from TransformBitField::FlagType
 */

inline void SetComponentDirtyBitField(TransformComponentBitField::FlagType& flags, uint8_t dirtyFlags)
{
  flags |= (static_cast<TransformComponentBitField::FlagType>(dirtyFlags) & TransformComponentBitField::COMPONENT_DIRTY_MASK);
}

inline void SetWorldMatrixDirtyBitField(TransformComponentBitField::FlagType& flags, bool dirty)
{
  if(dirty)
  {
    flags |= (TransformComponentBitField::WORLD_MATRIX_DIRTY_MASK << TransformComponentBitField::WORLD_MATRIX_DIRTY_SHIFT);
  }
  else
  {
    flags &= ~(TransformComponentBitField::WORLD_MATRIX_DIRTY_MASK << TransformComponentBitField::WORLD_MATRIX_DIRTY_SHIFT);
  }
}

inline void SetInheritanceModeBitField(TransformComponentBitField::FlagType& flags, InheritanceMode inheritMode)
{
  flags &= ~(TransformComponentBitField::INHERITANCE_MODE_MASK << TransformComponentBitField::INHERITANCE_MODE_SHIFT);
  flags |= (static_cast<TransformComponentBitField::FlagType>(inheritMode) & TransformComponentBitField::INHERITANCE_MODE_MASK) << TransformComponentBitField::INHERITANCE_MODE_SHIFT;
}

inline void SetIgnoredBitField(TransformComponentBitField::FlagType& flags, bool ignored)
{
  if(ignored)
  {
    flags |= (TransformComponentBitField::IGNORED_MASK << TransformComponentBitField::IGNORED_SHIFT);
  }
  else
  {
    flags &= ~(TransformComponentBitField::IGNORED_MASK << TransformComponentBitField::IGNORED_SHIFT);
  }
}

inline void SetPositionUsesAnchorPointBitField(TransformComponentBitField::FlagType& flags, bool used)
{
  if(used)
  {
    flags |= (TransformComponentBitField::POSITION_USES_ANCHOR_POINT_MASK << TransformComponentBitField::POSITION_USES_ANCHOR_POINT_SHIFT);
  }
  else
  {
    flags &= ~(TransformComponentBitField::POSITION_USES_ANCHOR_POINT_MASK << TransformComponentBitField::POSITION_USES_ANCHOR_POINT_SHIFT);
  }
}

inline bool IsComponentDirtyBitField(const TransformComponentBitField::FlagType& flags)
{
  return flags & (TransformComponentBitField::COMPONENT_DIRTY_MASK | (TransformComponentBitField::WORLD_MATRIX_DIRTY_MASK << TransformComponentBitField::WORLD_MATRIX_DIRTY_SHIFT));
}

inline bool IsWorldMatrixDirtyBitField(const TransformComponentBitField::FlagType& flags)
{
  return flags & (TransformComponentBitField::WORLD_MATRIX_DIRTY_MASK << TransformComponentBitField::WORLD_MATRIX_DIRTY_SHIFT);
}

inline InheritanceMode GetInheritanceModeBitField(const TransformComponentBitField::FlagType& flags)
{
  return static_cast<InheritanceMode>((flags >> TransformComponentBitField::INHERITANCE_MODE_SHIFT) & TransformComponentBitField::INHERITANCE_MODE_MASK);
}

inline bool IsIgnoredBitField(const TransformComponentBitField::FlagType& flags)
{
  return flags & (TransformComponentBitField::IGNORED_MASK << TransformComponentBitField::IGNORED_SHIFT);
}

inline bool IsPositionUsesAnchorPointBitField(const TransformComponentBitField::FlagType& flags)
{
  return flags & (TransformComponentBitField::POSITION_USES_ANCHOR_POINT_MASK << TransformComponentBitField::POSITION_USES_ANCHOR_POINT_SHIFT);
}

inline void ComponentDirtyAging(TransformComponentBitField::FlagType& flags)
{
  // Shift down last 2 bits only.
  auto dirty = (flags & TransformComponentBitField::COMPONENT_DIRTY_MASK);
  flags &= ~(TransformComponentBitField::COMPONENT_DIRTY_MASK);
  flags |= (dirty >> 1);
}

} // namespace TransformComponentBitField

enum TransformManagerProperty
{
  TRANSFORM_PROPERTY_POSITION = 0,
  TRANSFORM_PROPERTY_SCALE,
  TRANSFORM_PROPERTY_IGNORED,
  TRANSFORM_PROPERTY_ANCHOR_POINT,
  TRANSFORM_PROPERTY_PARENT_ORIGIN,
  TRANSFORM_PROPERTY_SIZE,
  TRANSFORM_PROPERTY_WORLD_POSITION,
  TRANSFORM_PROPERTY_WORLD_SCALE,
  TRANSFORM_PROPERTY_WORLD_ORIENTATION,
  TRANSFORM_PROPERTY_WORLD_MATRIX,
  TRANSFORM_PROPERTY_COUNT,
};

using TransformId                                         = uint32_t; // 4,294,967,294 transforms supported
static const TransformId INVALID_TRANSFORM_ID             = -1;
static const TransformId PARENT_OF_OFF_SCENE_TRANSFORM_ID = -1; // Special id to specify the node is scene offed. (Same as INVALID_TRANSFORM_ID)
static const TransformId PARENT_OF_ROOT_NODE_TRANSFORM_ID = -2; // Special id to specify the root of each scene.

class TransformManager;

struct TransformManagerData
{
  TransformManager* Manager() const
  {
    return mManager;
  }

  TransformId Id() const
  {
    return mId;
  }

  TransformManager* mManager{nullptr};
  TransformId       mId{INVALID_TRANSFORM_ID};
};

} // namespace SceneGraph
} // namespace Internal

// Allow TransformComponentAnimatable to be treated as a POD type
template<>
struct TypeTraits<Dali::Internal::SceneGraph::TransformComponentAnimatable> : public Dali::BasicTypes<Dali::Internal::SceneGraph::TransformComponentAnimatable>
{
  enum
  {
    IS_TRIVIAL_TYPE = true
  };
};

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
   * Checks if the TransformId is valid or not.
   * @param[in] transformId The TransformId to check
   * @return true if the TransformId is valid, false otherwise
   */
  inline static bool IsValidTransformId(const TransformId transformId)
  {
    // @code return transformId != PARENT_OF_OFF_SCENE_TRANSFORM_ID && transformId != PARENT_OF_ROOT_NODE_TRANSFORM_ID;
    return (~(transformId | 0x1)) != 0;
  }

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
  void SetParent(TransformId id, TransformId parentId);

  /**
   * Gets the world transform matrix of an exisiting transform component
   * @param[in] id Id of the transform component
   * @return The local to world transformation matrix of the component
   */
  const Matrix& GetWorldMatrix(TransformId id) const;

  /**
   * Gets the world transform matrix of an exisiting transform component
   * @param[in] id Id of the transform component
   * @return The local to world transformation matrix of the component
   */
  Matrix& GetWorldMatrix(TransformId id);

  /**
   * Checks if the world transform was updated in the last Update
   * @note It will return valid value only after Transform::Update() called.
   * @param[in] id Id of the transform
   * @return true if world matrix changed in the last update, false otherwise
   */
  bool IsWorldMatrixDirty(TransformId id) const
  {
    return TransformComponentBitField::IsWorldMatrixDirtyBitField(mTxComponentBitField[mIds[id]]);
  }

  /**
   * Sets position inheritance mode.
   * @param[in] id Id of the transform
   * @param[in] inherit True if position is inherited from parent, false otherwise
   */
  void SetInheritPosition(TransformId id, bool inherit);

  /**
   * Sets scale inheritance mode.
   * @param[in] id Id of the transform
   * @param[in] inherit True if scale is inherited from parent, false otherwise
   */
  void SetInheritScale(TransformId id, bool inherit);

  /**
   * Sets orientation inheritance mode.
   * @param[in] id Id of the transform
   * @param[in] inherit True if orientation is inherited from parent, false otherwise
   */
  void SetInheritOrientation(TransformId id, bool inherit);

  /**
   * Recomputes all world transform matrices
   * @return true if any component has been changed in this frame, false otherwise
   */
  bool Update();

  /**
   * Resets all the animatable properties to its base value
   */
  void ResetToBaseValue();

  /**
   * Get the value of a Vector3 property
   * @param[in] id Id of the transform component
   * @param[in] property The property
   */
  Vector3& GetVector3PropertyValue(TransformId id, TransformManagerProperty property);

  /**
   * Get the value of a Vector3 property
   * @param[in] id Id of the transform component
   * @param[in] property The property
   */
  const Vector3& GetVector3PropertyValue(TransformId id, TransformManagerProperty property) const;

  /**
   * Get the value of a component of Vector3 property
   * @param[in] id Id of the transform component
   * @param[in] property The property
   * param[in] component The component (0,1,2)
   */
  const float& GetVector3PropertyComponentValue(TransformId id, TransformManagerProperty property, uint32_t component) const;

  /**
   * Set the value of a Vector3 property
   * @param[in] id Id of the transform component
   * @param[in] property The property
   * @param[in] value The new value
   */
  void SetVector3PropertyValue(TransformId id, TransformManagerProperty property, const Vector3& value);

  /**
   * Set the value of a component of a Vector3 property
   * @param[in] id Id of the transform component
   * @param[in] property The property
   * @param[in] value The new value
   * param[in] component The component (0,1,2)
   */
  void SetVector3PropertyComponentValue(TransformId id, TransformManagerProperty property, float value, uint32_t component);

  /**
   * Bakes the value of a Vector3 property
   * @param[in] id Id of the transform component
   * @param[in] property The property
   * @param[in] value The new value
   */
  void BakeVector3PropertyValue(TransformId id, TransformManagerProperty property, const Vector3& value);

  /**
   * Bakes the value of a Vector3 property relative to the current value
   * @param[in] id Id of the transform component
   * @param[in] property The property
   * @param[in] value The new value
   */
  void BakeRelativeVector3PropertyValue(TransformId id, TransformManagerProperty property, const Vector3& value);

  /**
   * Bakes the value of a Vector3 property by multiplying the current value and the new value
   * @param[in] id Id of the transform component
   * @param[in] property The property
   * @param[in] value The new value
   */
  void BakeMultiplyVector3PropertyValue(TransformId id, TransformManagerProperty property, const Vector3& value);

  /**
   * Bakes the value of a component of Vector3 property
   * @param[in] id Id of the transform component
   * @param[in] property The property
   * @param[in] value The new value
   */
  void BakeVector3PropertyComponentValue(TransformId id, TransformManagerProperty property, float value, uint32_t component);

  /**
   * Bakes the value of the x component of Vector3 property
   * @param[in] id Id of the transform component
   * @param[in] property The property
   * @param[in] value The new value
   */
  void BakeXVector3PropertyValue(TransformId id, TransformManagerProperty property, float value);

  /**
   * Bakes the value of the y component of Vector3 property
   * @param[in] id Id of the transform component
   * @param[in] property The property
   * @param[in] value The new value
   */
  void BakeYVector3PropertyValue(TransformId id, TransformManagerProperty property, float value);

  /**
   * Bakes the value of the z component of Vector3 property
   * @param[in] id Id of the transform component
   * @param[in] property The property
   * @param[in] value The new value
   */
  void BakeZVector3PropertyValue(TransformId id, TransformManagerProperty property, float value);

  /**
   * Get the value of a quaternion property
   * @param[in] id Id of the transform component
   */
  Quaternion& GetQuaternionPropertyValue(TransformId id);

  /**
   * Get the value of a quaternion property
   * @param[in] id Id of the transform component
   */
  const Quaternion& GetQuaternionPropertyValue(TransformId id) const;

  /**
   * Set the value of a quaternion property
   * @param[in] id Id of the transform component
   * @param[in] q The new value
   */
  void SetQuaternionPropertyValue(TransformId id, const Quaternion& q);

  /**
   * Bake the value of a quaternion property
   * @param[in] id Id of the transform component
   * @param[in] q The new value
   */
  void BakeQuaternionPropertyValue(TransformId id, const Quaternion& q);

  /**
   * Bake the value of a quaternion property relative to its current value
   * @param[in] id Id of the transform component
   * @param[in] q The new value
   */
  void BakeRelativeQuaternionPropertyValue(TransformId id, const Quaternion& q);

  /**
   * Get the bounding sphere, in world coordinates, of a given component
   * @param[in] id Id of the transform component
   * @return The world space bounding sphere of the component
   */
  const Vector4& GetBoundingSphere(TransformId id) const;

  /**
   * Get the world matrix and size of a given component
   * @param[in] id Id of the transform component
   * @param[out] The world matrix of the component
   * @param[out] size size of the component
   */
  void GetWorldMatrixAndSize(TransformId id, Matrix& worldMatrix, Vector3& size) const;

  /**
   * @brief Sets the boolean which states whether the position should use the anchor-point on the given transform component.
   * @param[in] id Id of the transform component
   * @param[in] value True if the position should use the anchor-point
   */
  void SetPositionUsesAnchorPoint(TransformId id, bool value);

  /**
   * @brief Sets ignored value.
   * @param[in] id Id of the transform component.
   * @param[in] value True if the component is ignored.
   */
  void SetIgnored(TransformId id, bool value);

  /**
   * @brief Gets ignored value.
   * @param[in] id Id of the transform component.
   * @return True if the component is ignored.
   */
  bool IsIgnored(TransformId id) const
  {
    return TransformComponentBitField::IsIgnoredBitField(mTxComponentBitField[mIds[id]]);
  }

  /**
   * @brief Gets world ignored value.
   * @param[in] id Id of the transform component.
   * @return True if the component is world ignored.
   */
  bool IsWorldIgnored(TransformId id) const
  {
    return mIds[id] >= mValidComponentCount;
  }

private:
  // Helper struct to order components
  struct SOrderItem
  {
    bool operator<(const SOrderItem& item) const
    {
      if(sceneId != item.sceneId)
      {
        return sceneId < item.sceneId;
      }
      else
      {
        return level < item.level;
      }
    }

    TransformId id;
    uint16_t    sceneId;
    uint16_t    level;
  };

  /**
   * Swaps two components in the vectors
   * @param[in] i Index of a component
   * @param[in] j Index of a component
   */
  void SwapComponents(uint32_t i, uint32_t j);

  /**
   * Reorders components in hierarchical order so update can iterate sequentially
   * updating the world transforms. The components are sorted in the breadth first
   * order for each scene and grouped per scene.
   */
  void ReorderComponents();

  uint32_t mComponentCount;      ///< Total number of components
  uint32_t mValidComponentCount; ///< Total number of valid components

  FreeList                                     mIds;                            ///< FreeList of Ids
  Vector<TransformComponentAnimatable>         mTxComponentAnimatable;          ///< Animatable part of the components
  Vector<TransformComponentStatic>             mTxComponentStatic;              ///< Static part of the components
  Vector<TransformComponentBitField::FlagType> mTxComponentBitField;            ///< Bit-field part of the components
  Vector<TransformId>                          mComponentId;                    ///< Ids of the components
  Vector<Vector3>                              mSize;                           ///< Size of the components
  Vector<TransformId>                          mParent;                         ///< Parent of the components
  Vector<Matrix>                               mWorld;                          ///< Local to world transform of the components
  Vector<Vector4>                              mBoundingSpheres;                ///< Bounding spheres. xyz is the center and w is the radius
  Vector<TransformComponentAnimatable>         mTxComponentAnimatableBaseValue; ///< Base values for the animatable part of the components
  Vector<Vector3>                              mSizeBase;                       ///< Base value for the size of the components

  Vector<SOrderItem> mOrderedComponents; ///< Used to reorder components when hierarchy changes

  uint8_t mDirtyFlags;  ///< Dirty flags for all transform components. Age down at Update time.
  bool    mReorder : 1; ///< Flag to determine if the components have to reordered in the next Update
  bool    mUpdated : 1; ///< Flag whether we have updated the transform components previous frame.
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_TRANSFORM_MANAGER_H
