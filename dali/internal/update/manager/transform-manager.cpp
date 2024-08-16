/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/internal/update/manager/transform-manager.h>

//EXTERNAL INCLUDES
#include <algorithm>
#include <cstring>
#include <type_traits>

//INTERNAL INCLUDES
#include <dali/internal/common/math.h>
#include <dali/internal/common/matrix-utils.h>
#include <dali/internal/update/common/animatable-property.h> ///< for SET_FLAG and BAKE_FLAG
#include <dali/public-api/common/constants.h>

#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
namespace
{
//Default values for scale (1.0,1.0,1.0), orientation (Identity) and position (0.0,0.0,0.0)
static const float gDefaultTransformComponentAnimatableData[] = {1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f};

//Default values for anchor point (CENTER) and parent origin (TOP_LEFT)
static const float gDefaultTransformComponentStaticData[] = {0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.5f, true};

static const uint32_t STATIC_COMPONENT_FLAG = 0x01; ///< Indicates that the value when we change static transform components, so need to update at least 1 frame.

static_assert(sizeof(gDefaultTransformComponentAnimatableData) == sizeof(TransformComponentAnimatable), "gDefaultTransformComponentAnimatableData should have the same number of floats as specified in TransformComponentAnimatable");
static_assert(sizeof(gDefaultTransformComponentStaticData) == sizeof(TransformComponentStatic), "gDefaultTransformComponentStaticData should have the same number of floats as specified in TransformComponentStatic");

/**
 * @brief Calculates the center position for the transform component
 * @param[out] centerPosition The calculated center-position of the transform component
 * @param[in] transformComponentStatic A const reference to the static component transform struct
 * @param[in] scale scale factor to be applied to the center position.
 * @param[in] orientation orientation factor to be applied to the center position.
 * @param[in] size The size of the current transform component
 * @param[in] half Halfway point of the transform
 * @param[in] topLeft The top-left coords of the transform
 */
inline void CalculateCenterPosition(
  Vector3&                        centerPosition,
  const TransformComponentStatic& transformComponentStatic,
  const Vector3&                  scale,
  const Quaternion&               orientation,
  const Vector3&                  size,
  const Vector3&                  half,
  const Vector3&                  topLeft)
{
  // Calculate the center-point by applying the scale and rotation on the anchor point.
  centerPosition = (half - transformComponentStatic.mAnchorPoint) * size * scale;
  centerPosition *= orientation;

  // If the position is ignoring the anchor-point, then remove the anchor-point shift from the position.
  if(!transformComponentStatic.mPositionUsesAnchorPoint)
  {
    centerPosition -= (topLeft - transformComponentStatic.mAnchorPoint) * size;
  }
}

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_UPDATE_PROCESS, false);

/**
 * @brief Helper codes to set / bake the dirty flags for the components.
 * It will be useful when we want to reduce the number of lines.
 */

/**
 * @brief Append dirty flags to both component and global dirty flags.
 * @param[in,out] componentDirtyFlag The dirty flag of the component.
 * @param[in,out] globalDirtyFlag The dirty flag of the global.
 * @param[in] flag The flag what we want to append.
 */
inline void AppendDirtyFlags(uint8_t& componentDirtyFlag, uint8_t& globalDirtyFlag, const uint8_t flag)
{
  componentDirtyFlag |= flag;
  globalDirtyFlag |= flag;
}

/**
 * @brief Change the inherit mode for each mode and update dirty flags.
 * @param[in,out] inheritanceMode The inheritance mode of the component.
 * @param[in,out] componentDirtyFlag The dirty flag of the component.
 * @param[in,out] globalDirtyFlag The dirty flag of the global.
 * @param[in] inherit True if we want to set the inherit mode as true. False otherwise.
 */
inline void UpdateInheritMode(uint32_t& inheritanceMode, uint8_t& componentDirtyFlag, uint8_t& globalDirtyFlag, const InheritanceMode inheritModeFlag, const bool inherit)
{
  if(inherit)
  {
    if(!(inheritanceMode & inheritModeFlag))
    {
      AppendDirtyFlags(componentDirtyFlag, globalDirtyFlag, STATIC_COMPONENT_FLAG);
      inheritanceMode |= inheritModeFlag;
    }
  }
  else
  {
    if(inheritanceMode & inheritModeFlag)
    {
      AppendDirtyFlags(componentDirtyFlag, globalDirtyFlag, STATIC_COMPONENT_FLAG);
      inheritanceMode &= ~inheritModeFlag;
    }
  }
}

/**
 * @brief Set transform property value and update dirty flags.
 * @tparam T The type of the transform component. It can be either float or Vector3.
 * @param[in,out] current The current value of the transform component. It would be changed as input vlaue.
 * @param[in,out] componentDirtyFlag The dirty flag of the component.
 * @param[in,out] globalDirtyFlag The dirty flag of the global.
 * @param[in] value The value what we want to set.
 * @param[in] flag The flag what we want to append.
 */
template<typename T>
inline void SetTransfromProperty(T& current, uint8_t& componentDirtyFlag, uint8_t& globalDirtyFlag, const T& value, const uint8_t flag = SET_FLAG)
{
  current = value;
  AppendDirtyFlags(componentDirtyFlag, globalDirtyFlag, flag);
}

/**
 * @brief Bake transform property value and update dirty flags.
 * @tparam T The type of the transform component. It can be either float or Vector3.
 * @param[in,out] current The current value of the transform component.
 * @param[in,out] base The base value of the transform component.
 * @param[in,out] componentDirtyFlag The dirty flag of the component.
 * @param[in,out] globalDirtyFlag The dirty flag of the global.
 * @param[in] value The value what we want to bake.
 * @param[in] flag The flag what we want to append.
 */
template<typename T>
inline void BakeTransfromProperty(T& current, T& base, uint8_t& componentDirtyFlag, uint8_t& globalDirtyFlag, const T& value)
{
  current = base = value;
  AppendDirtyFlags(componentDirtyFlag, globalDirtyFlag, BAKED_FLAG);
}

/**
 * @brief Set transform property value and update dirty flags only if current value changed.
 * @tparam T The type of the transform component. It can be either float or Vector3.
 * @param[in,out] current The current value of the transform component.
 * @param[in,out] componentDirtyFlag The dirty flag of the component.
 * @param[in,out] globalDirtyFlag The dirty flag of the global.
 * @param[in] value The value what we want to set.
 * @param[in] flag The flag what we want to append.
 */
template<typename T>
inline void SetTransfromPropertyIfChanged(T& current, uint8_t& componentDirtyFlag, uint8_t& globalDirtyFlag, const T& value, const uint8_t flag = SET_FLAG)
{
  if constexpr(std::is_same_v<T, float>)
  {
    if(!Equals(current, value))
    {
      SetTransfromProperty(current, componentDirtyFlag, globalDirtyFlag, value, flag);
    }
  }
  else
  {
    if(current != value)
    {
      SetTransfromProperty(current, componentDirtyFlag, globalDirtyFlag, value, flag);
    }
  }
}

/**
 * @brief Bake transform property value and update dirty flags only if base value changed.
 * @tparam T The type of the transform component. It can be either float or Vector3.
 * @param[in,out] current The current value of the transform component.
 * @param[in,out] base The base value of the transform component.
 * @param[in,out] componentDirtyFlag The dirty flag of the component.
 * @param[in,out] globalDirtyFlag The dirty flag of the global.
 * @param[in] value The value what we want to bake.
 * @param[in] flag The flag what we want to append.
 */
template<typename T>
inline void BakeTransfromPropertyIfChanged(T& current, T& base, uint8_t& componentDirtyFlag, uint8_t& globalDirtyFlag, const T& value)
{
  if constexpr(std::is_same_v<T, float>)
  {
    if(!Equals(base, value))
    {
      BakeTransfromProperty(current, base, componentDirtyFlag, globalDirtyFlag, value);
    }
  }
  else
  {
    if(base != value)
    {
      BakeTransfromProperty(current, base, componentDirtyFlag, globalDirtyFlag, value);
    }
  }
}

} // unnamed namespace

TransformManager::TransformManager()
: mComponentCount(0),
  mDirtyFlags(CLEAN_FLAG),
  mReorder(false),
  mUpdated(false)
{
}

TransformManager::~TransformManager() = default;

TransformId TransformManager::CreateTransform()
{
  //Get id for the new component
  TransformId id = mIds.Add(mComponentCount);

  if(mTxComponentAnimatable.Size() <= mComponentCount)
  {
    //Make room for another component
    mTxComponentAnimatable.PushBack(TransformComponentAnimatable());
    mTxComponentStatic.PushBack(TransformComponentStatic());
    mInheritanceMode.PushBack(INHERIT_ALL);
    mComponentId.PushBack(id);
    mSize.PushBack(Vector3(0.0f, 0.0f, 0.0f));
    mParent.PushBack(INVALID_TRANSFORM_ID);
    mWorld.PushBack(Matrix::IDENTITY);
    mLocal.PushBack(Matrix::IDENTITY);
    mBoundingSpheres.PushBack(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    mTxComponentAnimatableBaseValue.PushBack(TransformComponentAnimatable());
    mSizeBase.PushBack(Vector3(0.0f, 0.0f, 0.0f));
    mComponentDirty.PushBack(CLEAN_FLAG);
    mWorldMatrixDirty.PushBack(false);
  }
  else
  {
    //Set default values
    memcpy(&mTxComponentAnimatable[mComponentCount], &gDefaultTransformComponentAnimatableData, sizeof(TransformComponentAnimatable));
    memcpy(&mTxComponentStatic[mComponentCount], &gDefaultTransformComponentStaticData, sizeof(TransformComponentStatic));
    memcpy(&mTxComponentAnimatableBaseValue[mComponentCount], &gDefaultTransformComponentAnimatableData, sizeof(TransformComponentAnimatable));
    mInheritanceMode[mComponentCount] = INHERIT_ALL;
    mComponentId[mComponentCount]     = id;
    mSize[mComponentCount]            = Vector3(0.0f, 0.0f, 0.0f);
    mParent[mComponentCount]          = INVALID_TRANSFORM_ID;
    mLocal[mComponentCount].SetIdentity();
    mWorld[mComponentCount].SetIdentity();
    mBoundingSpheres[mComponentCount]  = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
    mSizeBase[mComponentCount]         = Vector3(0.0f, 0.0f, 0.0f);
    mComponentDirty[mComponentCount]   = CLEAN_FLAG;
    mWorldMatrixDirty[mComponentCount] = false;
  }

  mComponentCount++;
  return id;
}

void TransformManager::RemoveTransform(TransformId id)
{
  //Move the last element to the gap
  mComponentCount--;
  TransformId index                      = mIds[id];
  mTxComponentAnimatable[index]          = mTxComponentAnimatable[mComponentCount];
  mTxComponentStatic[index]              = mTxComponentStatic[mComponentCount];
  mInheritanceMode[index]                = mInheritanceMode[mComponentCount];
  mSize[index]                           = mSize[mComponentCount];
  mParent[index]                         = mParent[mComponentCount];
  mWorld[index]                          = mWorld[mComponentCount];
  mLocal[index]                          = mLocal[mComponentCount];
  mTxComponentAnimatableBaseValue[index] = mTxComponentAnimatableBaseValue[mComponentCount];
  mSizeBase[index]                       = mSizeBase[mComponentCount];
  mComponentDirty[index]                 = mComponentDirty[mComponentCount];
  mWorldMatrixDirty[index]               = mWorldMatrixDirty[mComponentCount];
  mBoundingSpheres[index]                = mBoundingSpheres[mComponentCount];

  TransformId lastItemId = mComponentId[mComponentCount];
  mIds[lastItemId]       = index;
  mComponentId[index]    = lastItemId;
  mIds.Remove(id);

  mReorder = true;
}

void TransformManager::SetParent(TransformId id, TransformId parentId)
{
  DALI_ASSERT_ALWAYS(id != parentId);

  TransformId index = mIds[id];

  if(DALI_LIKELY(mParent[index] != parentId))
  {
    mParent[index] = parentId;
    AppendDirtyFlags(mComponentDirty[index], mDirtyFlags, STATIC_COMPONENT_FLAG); ///< Need to calculate local matrix, at least 1 frame.

    mReorder = true;
  }
}

const Matrix& TransformManager::GetWorldMatrix(TransformId id) const
{
  return mWorld[mIds[id]];
}

Matrix& TransformManager::GetWorldMatrix(TransformId id)
{
  return mWorld[mIds[id]];
}

void TransformManager::SetInheritPosition(TransformId id, bool inherit)
{
  TransformId index = mIds[id];
  UpdateInheritMode(mInheritanceMode[index], mComponentDirty[index], mDirtyFlags, INHERIT_POSITION, inherit);
}

void TransformManager::SetInheritScale(TransformId id, bool inherit)
{
  TransformId index = mIds[id];
  UpdateInheritMode(mInheritanceMode[index], mComponentDirty[index], mDirtyFlags, INHERIT_SCALE, inherit);
}

void TransformManager::SetInheritOrientation(TransformId id, bool inherit)
{
  TransformId index = mIds[id];
  UpdateInheritMode(mInheritanceMode[index], mComponentDirty[index], mDirtyFlags, INHERIT_ORIENTATION, inherit);
}

void TransformManager::ResetToBaseValue()
{
  if(mComponentCount)
  {
    if(mDirtyFlags != CLEAN_FLAG)
    {
      memcpy(&mTxComponentAnimatable[0], &mTxComponentAnimatableBaseValue[0], sizeof(TransformComponentAnimatable) * mComponentCount);
      memcpy(&mSize[0], &mSizeBase[0], sizeof(Vector3) * mComponentCount);
    }

    if(mUpdated)
    {
      memset(&mWorldMatrixDirty[0], false, sizeof(bool) * mComponentCount);
    }
  }
}

bool TransformManager::Update()
{
  mUpdated = false;

  if(mDirtyFlags == CLEAN_FLAG)
  {
    DALI_LOG_DEBUG_INFO("Transform value is not changed. Skip transform update.\n");
    return false;
  }

  DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_TRANSFORM_UPDATE", [&](std::ostringstream& oss) {
    oss << "[" << mComponentCount << "]";
  });

  if(mReorder)
  {
    DALI_TRACE_SCOPE(gTraceFilter, "DALI_TRANSFORM_REORDER");
    //If some transform component has change its parent or has been removed since last update
    //we need to reorder the vectors
    ReorderComponents();
    mReorder = false;
  }

  //Iterate through all components to compute its world matrix
  Vector3       centerPosition;
  Vector3       localPosition;
  const Vector3 half(0.5f, 0.5f, 0.5f);
  const Vector3 topLeft(0.0f, 0.0f, 0.5f);
  for(unsigned int i(0); i < mComponentCount; ++i)
  {
    if(DALI_LIKELY(mInheritanceMode[i] != DONT_INHERIT_TRANSFORM && mParent[i] != INVALID_TRANSFORM_ID))
    {
      const TransformId& parentIndex = mIds[mParent[i]];
      if(DALI_LIKELY(mInheritanceMode[i] == INHERIT_ALL))
      {
        if(mComponentDirty[i] || mWorldMatrixDirty[parentIndex])
        {
          // TODO : Skip world matrix comparision. Is it improve performance?
          mWorldMatrixDirty[i] = true;

          //Full transform inherited
          CalculateCenterPosition(centerPosition, mTxComponentStatic[i], mTxComponentAnimatable[i].mScale, mTxComponentAnimatable[i].mOrientation, mSize[i], half, topLeft);
          localPosition = mTxComponentAnimatable[i].mPosition + centerPosition + (mTxComponentStatic[i].mParentOrigin - half) * mSize[parentIndex];
          mLocal[i].SetTransformComponents(mTxComponentAnimatable[i].mScale, mTxComponentAnimatable[i].mOrientation, localPosition);

          //Update the world matrix
          MatrixUtils::MultiplyTransformMatrix(mWorld[i], mLocal[i], mWorld[parentIndex]);
        }
      }
      else
      {
        // Keep previous worldMatrix for comparison.
        Matrix previousWorldMatrix = mWorld[i];

        // Get Parent information.
        Vector3       parentPosition, parentScale;
        Quaternion    parentOrientation;
        const Matrix& parentMatrix = mWorld[parentIndex];
        parentMatrix.GetTransformComponents(parentPosition, parentOrientation, parentScale);

        // Compute intermediate Local information
        CalculateCenterPosition(centerPosition, mTxComponentStatic[i], mTxComponentAnimatable[i].mScale, mTxComponentAnimatable[i].mOrientation, mSize[i], half, topLeft);
        Vector3 intermediateLocalPosition = mTxComponentAnimatable[i].mPosition + centerPosition + (mTxComponentStatic[i].mParentOrigin - half) * mSize[parentIndex];
        Matrix  intermediateLocalMatrix;
        intermediateLocalMatrix.SetTransformComponents(mTxComponentAnimatable[i].mScale, mTxComponentAnimatable[i].mOrientation, intermediateLocalPosition);

        // Compute intermediate world information
        Matrix intermediateWorldMatrix;
        MatrixUtils::MultiplyTransformMatrix(intermediateWorldMatrix, intermediateLocalMatrix, mWorld[parentIndex]);

        Vector3    intermediateWorldPosition, intermediateWorldScale;
        Quaternion intermediateWorldOrientation;
        intermediateWorldMatrix.GetTransformComponents(intermediateWorldPosition, intermediateWorldOrientation, intermediateWorldScale);

        // Compute final world information
        Vector3    finalWorldPosition    = intermediateWorldPosition;
        Vector3    finalWorldScale       = intermediateWorldScale;
        Quaternion finalWorldOrientation = intermediateWorldOrientation;
        // worldScale includes the influence of local scale, local rotation, and parent scale.
        // So, for the final world matrix, if this node inherits its parent scale, use worldScale.
        // If not, use local scale for the final world matrix.
        if((mInheritanceMode[i] & INHERIT_SCALE) == 0)
        {
          finalWorldScale = mTxComponentAnimatable[i].mScale;
        }

        // For the final world matrix, if this node inherits its parent orientation, use worldOrientation.
        // If not, use local orientation for the final world matrix.
        if((mInheritanceMode[i] & INHERIT_ORIENTATION) == 0)
        {
          finalWorldOrientation = mTxComponentAnimatable[i].mOrientation;
        }

        // The final world position of this node is computed as a sum of
        // parent origin position in world space and relative position of center from parent origin.
        // If this node doesn't inherit its parent position, simply use the relative position as a final world position.
        Vector3 localCenterPosition;
        CalculateCenterPosition(localCenterPosition, mTxComponentStatic[i], finalWorldScale, finalWorldOrientation, mSize[i], half, topLeft);
        finalWorldPosition = mTxComponentAnimatable[i].mPosition * finalWorldScale;
        finalWorldPosition *= finalWorldOrientation;
        finalWorldPosition += localCenterPosition;
        if((mInheritanceMode[i] & INHERIT_POSITION) != 0)
        {
          Vector4 parentOriginPosition((mTxComponentStatic[i].mParentOrigin - half) * mSize[parentIndex]);
          parentOriginPosition.w = 1.0f;
          finalWorldPosition += Vector3(parentMatrix * parentOriginPosition);
        }

        mWorld[i].SetTransformComponents(finalWorldScale, finalWorldOrientation, finalWorldPosition);

        Matrix inverseParentMatrix;
        inverseParentMatrix.SetInverseTransformComponents(parentScale, parentOrientation, parentPosition);
        mLocal[i] = inverseParentMatrix * mWorld[i];

        // TODO : We need to check mComponentDirty since we have to check the size changeness.
        //        Could we check size changeness only?
        mWorldMatrixDirty[i] = mComponentDirty[i] || (previousWorldMatrix != mWorld[i]);
      }
    }
    else //Component has no parent or doesn't inherit transform
    {
      if(mComponentDirty[i])
      {
        // TODO : We need to check mComponentDirty since we have to check the size changeness.
        //        Could we check size changeness only?
        mWorldMatrixDirty[i] = true;

        CalculateCenterPosition(centerPosition, mTxComponentStatic[i], mTxComponentAnimatable[i].mScale, mTxComponentAnimatable[i].mOrientation, mSize[i], half, topLeft);
        localPosition = mTxComponentAnimatable[i].mPosition + centerPosition;
        mLocal[i].SetTransformComponents(mTxComponentAnimatable[i].mScale, mTxComponentAnimatable[i].mOrientation, localPosition);
        mWorld[i] = mLocal[i];
      }
    }

    //Update the bounding sphere
    Vec3 centerToEdge = {mSize[i].Length() * 0.5f, 0.0f, 0.0f};
    Vec3 centerToEdgeWorldSpace;
    TransformVector3(centerToEdgeWorldSpace, mWorld[i].AsFloat(), centerToEdge);

    mBoundingSpheres[i]   = mWorld[i].GetTranslation();
    mBoundingSpheres[i].w = Length(centerToEdgeWorldSpace);

    mUpdated = mUpdated || mWorldMatrixDirty[i];

    mComponentDirty[i] >>= 1u; ///< age down.
  }

  mDirtyFlags >>= 1u; ///< age down.

  DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_TRANSFORM_UPDATE", [&](std::ostringstream& oss) {
    oss << "[componentsChanged:" << mUpdated << "]";
  });

  return mUpdated;
}

void TransformManager::SwapComponents(unsigned int i, unsigned int j)
{
  std::swap(mTxComponentAnimatable[i], mTxComponentAnimatable[j]);
  std::swap(mTxComponentStatic[i], mTxComponentStatic[j]);
  std::swap(mInheritanceMode[i], mInheritanceMode[j]);
  std::swap(mSize[i], mSize[j]);
  std::swap(mParent[i], mParent[j]);
  std::swap(mComponentId[i], mComponentId[j]);
  std::swap(mTxComponentAnimatableBaseValue[i], mTxComponentAnimatableBaseValue[j]);
  std::swap(mSizeBase[i], mSizeBase[j]);
  std::swap(mLocal[i], mLocal[j]);
  std::swap(mComponentDirty[i], mComponentDirty[j]);
  std::swap(mBoundingSpheres[i], mBoundingSpheres[j]);
  std::swap(mWorld[i], mWorld[j]);

  mIds[mComponentId[i]] = i;
  mIds[mComponentId[j]] = j;
}

void TransformManager::ReorderComponents()
{
  mOrderedComponents.Resize(mComponentCount);

  uint16_t    sceneId = 0u;
  TransformId parentId;

  // Create sceneId first
  for(TransformId i = 0; i < mComponentCount; ++i)
  {
    mOrderedComponents[i].id    = mComponentId[i];
    mOrderedComponents[i].level = 0u;

    parentId = mParent[i];
    if(parentId == INVALID_TRANSFORM_ID)
    {
      mOrderedComponents[i].sceneId = sceneId++;
    }
  }

  // Propagate sceneId and level
  for(TransformId i = 0; i < mComponentCount; ++i)
  {
    parentId = mParent[i];
    while(parentId != INVALID_TRANSFORM_ID)
    {
      const uint32_t parentIndex = mIds[parentId];
      ++mOrderedComponents[i].level;
      mOrderedComponents[i].sceneId = mOrderedComponents[parentIndex].sceneId;
      if(parentIndex < i)
      {
        // Parent information update done. We can reuse it.
        mOrderedComponents[i].level += mOrderedComponents[parentIndex].level;
        break;
      }
      else
      {
        parentId = mParent[parentIndex];
      }
    }
  }

  std::stable_sort(mOrderedComponents.Begin(), mOrderedComponents.End());
  TransformId previousIndex = 0;
  for(TransformId newIndex = 0; newIndex < mComponentCount - 1; ++newIndex)
  {
    previousIndex = mIds[mOrderedComponents[newIndex].id];
    if(previousIndex != newIndex)
    {
      SwapComponents(previousIndex, newIndex);
    }
  }

#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
  // Since Resize() operation will make overhead when we create new Transform::Data, let we check shrink to fit trigger like this.
  if(DALI_UNLIKELY(static_cast<decltype(mTxComponentAnimatable)::SizeType>(mComponentCount) * Dali::VectorBase::SHRINK_REQUIRED_RATIO < mTxComponentAnimatable.Capacity()))
  {
    // Reduce the capacity of vector.
    // Note that reduce vector capacity will make performance down.
    // Please do not doing this frequencly.

    // NOTE : We cannot reduce FreeList type for now.

    mTxComponentAnimatable.Resize(mComponentCount);
    mTxComponentStatic.Resize(mComponentCount);
    mInheritanceMode.Resize(mComponentCount);
    mComponentId.Resize(mComponentCount);
    mSize.Resize(mComponentCount);
    mParent.Resize(mComponentCount);
    mWorld.Resize(mComponentCount);
    mLocal.Resize(mComponentCount);
    mBoundingSpheres.Resize(mComponentCount);
    mTxComponentAnimatableBaseValue.Resize(mComponentCount);
    mSizeBase.Resize(mComponentCount);
    mComponentDirty.Resize(mComponentCount);
    mWorldMatrixDirty.Resize(mComponentCount);
    mOrderedComponents.Resize(mComponentCount);

    mTxComponentAnimatable.ShrinkToFit();
    mTxComponentStatic.ShrinkToFit();
    mInheritanceMode.ShrinkToFit();
    mComponentId.ShrinkToFit();
    mSize.ShrinkToFit();
    mParent.ShrinkToFit();
    mWorld.ShrinkToFit();
    mLocal.ShrinkToFit();
    mBoundingSpheres.ShrinkToFit();
    mTxComponentAnimatableBaseValue.ShrinkToFit();
    mSizeBase.ShrinkToFit();
    mComponentDirty.ShrinkToFit();
    mWorldMatrixDirty.ShrinkToFit();
    mOrderedComponents.ShrinkToFit();
  }
#endif
}

Vector3& TransformManager::GetVector3PropertyValue(TransformId id, TransformManagerProperty property)
{
  switch(property)
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      return mTxComponentAnimatable[mIds[id]].mPosition;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      return mTxComponentAnimatable[mIds[id]].mScale;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      return mTxComponentStatic[mIds[id]].mParentOrigin;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      return mTxComponentStatic[mIds[id]].mAnchorPoint;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      return mSize[mIds[id]];
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
      return mTxComponentAnimatable[mIds[id]].mPosition;
    }
  }
}

const Vector3& TransformManager::GetVector3PropertyValue(TransformId id, TransformManagerProperty property) const
{
  switch(property)
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      return mTxComponentAnimatable[mIds[id]].mPosition;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      return mTxComponentAnimatable[mIds[id]].mScale;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      return mTxComponentStatic[mIds[id]].mParentOrigin;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      return mTxComponentStatic[mIds[id]].mAnchorPoint;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      return mSize[mIds[id]];
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
      return mTxComponentAnimatable[mIds[id]].mPosition;
    }
  }
}

const float& TransformManager::GetVector3PropertyComponentValue(TransformId id, TransformManagerProperty property, unsigned int component) const
{
  switch(property)
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      return mTxComponentAnimatable[mIds[id]].mPosition[component];
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      return mTxComponentAnimatable[mIds[id]].mScale[component];
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      return mTxComponentStatic[mIds[id]].mParentOrigin[component];
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      return mTxComponentStatic[mIds[id]].mAnchorPoint[component];
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      return mSize[mIds[id]][component];
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
      return mTxComponentAnimatable[mIds[id]].mPosition[component];
    }
  }
}

void TransformManager::SetVector3PropertyValue(TransformId id, TransformManagerProperty property, const Vector3& value)
{
  TransformId index(mIds[id]);

  switch(property)
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      SetTransfromProperty(mTxComponentAnimatable[index].mPosition, mComponentDirty[index], mDirtyFlags, value);
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      SetTransfromProperty(mTxComponentAnimatable[index].mScale, mComponentDirty[index], mDirtyFlags, value);
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      SetTransfromProperty(mTxComponentStatic[index].mParentOrigin, mComponentDirty[index], mDirtyFlags, value, STATIC_COMPONENT_FLAG);
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      SetTransfromProperty(mTxComponentStatic[index].mAnchorPoint, mComponentDirty[index], mDirtyFlags, value, STATIC_COMPONENT_FLAG);
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      SetTransfromProperty(mSize[index], mComponentDirty[index], mDirtyFlags, value);
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

void TransformManager::SetVector3PropertyComponentValue(TransformId id, TransformManagerProperty property, float value, unsigned int component)
{
  TransformId index(mIds[id]);

  switch(property)
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      SetTransfromPropertyIfChanged(mTxComponentAnimatable[index].mPosition[component], mComponentDirty[index], mDirtyFlags, value);
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      SetTransfromPropertyIfChanged(mTxComponentAnimatable[index].mScale[component], mComponentDirty[index], mDirtyFlags, value);
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      SetTransfromPropertyIfChanged(mTxComponentStatic[index].mParentOrigin[component], mComponentDirty[index], mDirtyFlags, value, STATIC_COMPONENT_FLAG);
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      SetTransfromPropertyIfChanged(mTxComponentStatic[index].mAnchorPoint[component], mComponentDirty[index], mDirtyFlags, value, STATIC_COMPONENT_FLAG);
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      SetTransfromPropertyIfChanged(mSize[index][component], mComponentDirty[index], mDirtyFlags, value);
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

void TransformManager::BakeVector3PropertyValue(TransformId id, TransformManagerProperty property, const Vector3& value)
{
  TransformId index(mIds[id]);

  switch(property)
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      BakeTransfromProperty(mTxComponentAnimatable[index].mPosition, mTxComponentAnimatableBaseValue[index].mPosition, mComponentDirty[index], mDirtyFlags, value);
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      BakeTransfromProperty(mTxComponentAnimatable[index].mScale, mTxComponentAnimatableBaseValue[index].mScale, mComponentDirty[index], mDirtyFlags, value);
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      SetTransfromProperty(mTxComponentStatic[index].mParentOrigin, mComponentDirty[index], mDirtyFlags, value, STATIC_COMPONENT_FLAG);
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      SetTransfromProperty(mTxComponentStatic[index].mAnchorPoint, mComponentDirty[index], mDirtyFlags, value, STATIC_COMPONENT_FLAG);
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      BakeTransfromProperty(mSize[index], mSizeBase[index], mComponentDirty[index], mDirtyFlags, value);
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

void TransformManager::BakeRelativeVector3PropertyValue(TransformId id, TransformManagerProperty property, const Vector3& value)
{
  TransformId index(mIds[id]);
  AppendDirtyFlags(mComponentDirty[index], mDirtyFlags, BAKED_FLAG);

  switch(property)
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      mTxComponentAnimatable[index].mPosition = mTxComponentAnimatableBaseValue[index].mPosition = mTxComponentAnimatable[index].mPosition + value;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      mTxComponentAnimatable[index].mScale = mTxComponentAnimatableBaseValue[index].mScale = mTxComponentAnimatable[index].mScale + value;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      mTxComponentStatic[index].mParentOrigin = mTxComponentStatic[index].mParentOrigin + value;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      mTxComponentStatic[index].mAnchorPoint = mTxComponentStatic[index].mAnchorPoint + value;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      mSize[index] = mSizeBase[index] = mSize[index] + value;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

void TransformManager::BakeMultiplyVector3PropertyValue(TransformId id, TransformManagerProperty property, const Vector3& value)
{
  TransformId index(mIds[id]);
  AppendDirtyFlags(mComponentDirty[index], mDirtyFlags, BAKED_FLAG);

  switch(property)
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      mTxComponentAnimatable[index].mPosition = mTxComponentAnimatableBaseValue[index].mPosition = mTxComponentAnimatable[index].mPosition * value;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      mTxComponentAnimatable[index].mScale = mTxComponentAnimatableBaseValue[index].mScale = mTxComponentAnimatable[index].mScale * value;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      mTxComponentStatic[index].mParentOrigin = mTxComponentStatic[index].mParentOrigin * value;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      mTxComponentStatic[index].mAnchorPoint = mTxComponentStatic[index].mAnchorPoint * value;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      mSize[index] = mSizeBase[index] = mSize[index] * value;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

void TransformManager::BakeVector3PropertyComponentValue(TransformId id, TransformManagerProperty property, float value, unsigned int component)
{
  TransformId index(mIds[id]);

  switch(property)
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      BakeTransfromPropertyIfChanged(mTxComponentAnimatable[index].mPosition[component], mTxComponentAnimatableBaseValue[index].mPosition[component], mComponentDirty[index], mDirtyFlags, value);
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      BakeTransfromPropertyIfChanged(mTxComponentAnimatable[index].mScale[component], mTxComponentAnimatableBaseValue[index].mScale[component], mComponentDirty[index], mDirtyFlags, value);
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      SetTransfromPropertyIfChanged(mTxComponentStatic[index].mParentOrigin[component], mComponentDirty[index], mDirtyFlags, value, STATIC_COMPONENT_FLAG);
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      SetTransfromPropertyIfChanged(mTxComponentStatic[index].mAnchorPoint[component], mComponentDirty[index], mDirtyFlags, value, STATIC_COMPONENT_FLAG);
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      BakeTransfromPropertyIfChanged(mSize[index][component], mSizeBase[index][component], mComponentDirty[index], mDirtyFlags, value);
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

void TransformManager::BakeXVector3PropertyValue(TransformId id, TransformManagerProperty property, float value)
{
  TransformId index(mIds[id]);

  switch(property)
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      BakeTransfromPropertyIfChanged(mTxComponentAnimatable[index].mPosition.x, mTxComponentAnimatableBaseValue[index].mPosition.x, mComponentDirty[index], mDirtyFlags, value);
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      BakeTransfromPropertyIfChanged(mTxComponentAnimatable[index].mScale.x, mTxComponentAnimatableBaseValue[index].mScale.x, mComponentDirty[index], mDirtyFlags, value);
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      SetTransfromPropertyIfChanged(mTxComponentStatic[index].mParentOrigin.x, mComponentDirty[index], mDirtyFlags, value, STATIC_COMPONENT_FLAG);
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      SetTransfromPropertyIfChanged(mTxComponentStatic[index].mAnchorPoint.x, mComponentDirty[index], mDirtyFlags, value, STATIC_COMPONENT_FLAG);
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      BakeTransfromPropertyIfChanged(mSize[index].x, mSizeBase[index].x, mComponentDirty[index], mDirtyFlags, value);
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

void TransformManager::BakeYVector3PropertyValue(TransformId id, TransformManagerProperty property, float value)
{
  TransformId index(mIds[id]);

  switch(property)
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      BakeTransfromPropertyIfChanged(mTxComponentAnimatable[index].mPosition.y, mTxComponentAnimatableBaseValue[index].mPosition.y, mComponentDirty[index], mDirtyFlags, value);
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      BakeTransfromPropertyIfChanged(mTxComponentAnimatable[index].mScale.y, mTxComponentAnimatableBaseValue[index].mScale.y, mComponentDirty[index], mDirtyFlags, value);
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      SetTransfromPropertyIfChanged(mTxComponentStatic[index].mParentOrigin.y, mComponentDirty[index], mDirtyFlags, value, STATIC_COMPONENT_FLAG);
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      SetTransfromPropertyIfChanged(mTxComponentStatic[index].mAnchorPoint.y, mComponentDirty[index], mDirtyFlags, value, STATIC_COMPONENT_FLAG);
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      BakeTransfromPropertyIfChanged(mSize[index].y, mSizeBase[index].y, mComponentDirty[index], mDirtyFlags, value);
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

void TransformManager::BakeZVector3PropertyValue(TransformId id, TransformManagerProperty property, float value)
{
  TransformId index(mIds[id]);

  switch(property)
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      BakeTransfromPropertyIfChanged(mTxComponentAnimatable[index].mPosition.z, mTxComponentAnimatableBaseValue[index].mPosition.z, mComponentDirty[index], mDirtyFlags, value);
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      BakeTransfromPropertyIfChanged(mTxComponentAnimatable[index].mScale.z, mTxComponentAnimatableBaseValue[index].mScale.z, mComponentDirty[index], mDirtyFlags, value);
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      SetTransfromPropertyIfChanged(mTxComponentStatic[index].mParentOrigin.z, mComponentDirty[index], mDirtyFlags, value, STATIC_COMPONENT_FLAG);
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      SetTransfromPropertyIfChanged(mTxComponentStatic[index].mAnchorPoint.z, mComponentDirty[index], mDirtyFlags, value, STATIC_COMPONENT_FLAG);
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      BakeTransfromPropertyIfChanged(mSize[index].z, mSizeBase[index].z, mComponentDirty[index], mDirtyFlags, value);
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

Quaternion& TransformManager::GetQuaternionPropertyValue(TransformId id)
{
  return mTxComponentAnimatable[mIds[id]].mOrientation;
}

const Quaternion& TransformManager::GetQuaternionPropertyValue(TransformId id) const
{
  return mTxComponentAnimatable[mIds[id]].mOrientation;
}

void TransformManager::SetQuaternionPropertyValue(TransformId id, const Quaternion& q)
{
  TransformId index(mIds[id]);
  SetTransfromProperty(mTxComponentAnimatable[index].mOrientation, mComponentDirty[index], mDirtyFlags, q);
}

void TransformManager::BakeQuaternionPropertyValue(TransformId id, const Quaternion& q)
{
  TransformId index(mIds[id]);
  BakeTransfromProperty(mTxComponentAnimatable[index].mOrientation, mTxComponentAnimatableBaseValue[index].mOrientation, mComponentDirty[index], mDirtyFlags, q);
}

void TransformManager::BakeRelativeQuaternionPropertyValue(TransformId id, const Quaternion& q)
{
  TransformId index(mIds[id]);
  AppendDirtyFlags(mComponentDirty[index], mDirtyFlags, BAKED_FLAG);

  mTxComponentAnimatable[index].mOrientation = mTxComponentAnimatableBaseValue[index].mOrientation = mTxComponentAnimatable[index].mOrientation * q;
}

const Vector4& TransformManager::GetBoundingSphere(TransformId id) const
{
  return mBoundingSpheres[mIds[id]];
}

void TransformManager::GetWorldMatrixAndSize(TransformId id, Matrix& worldMatrix, Vector3& size) const
{
  TransformId index = mIds[id];
  worldMatrix       = mWorld[index];
  size              = mSize[index];
}

void TransformManager::SetPositionUsesAnchorPoint(TransformId id, bool value)
{
  TransformId index(mIds[id]);

  SetTransfromPropertyIfChanged(mTxComponentStatic[index].mPositionUsesAnchorPoint, mComponentDirty[index], mDirtyFlags, value, STATIC_COMPONENT_FLAG);
}

} //namespace SceneGraph
} //namespace Internal
} //namespace Dali
