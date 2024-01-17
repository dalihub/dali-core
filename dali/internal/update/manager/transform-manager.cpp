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

// TODO : The name of trace marker name is from VD specific. We might need to change it future.
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_COMBINED, false);
} // unnamed namespace

TransformManager::TransformManager()
: mComponentCount(0),
  mReorder(false)
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
    mComponentDirty.PushBack(false);
    mLocalMatrixDirty.PushBack(false);
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
    mComponentDirty[mComponentCount]   = false;
    mLocalMatrixDirty[mComponentCount] = false;
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
  mLocalMatrixDirty[index]               = mLocalMatrixDirty[mComponentCount];
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
  TransformId index      = mIds[id];
  mParent[index]         = parentId;
  mComponentDirty[index] = true;
  mReorder               = true;
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
  if(inherit)
  {
    mInheritanceMode[index] |= INHERIT_POSITION;
  }
  else
  {
    mInheritanceMode[index] &= ~INHERIT_POSITION;
  }

  mComponentDirty[index] = true;
}

void TransformManager::SetInheritScale(TransformId id, bool inherit)
{
  TransformId index = mIds[id];
  if(inherit)
  {
    mInheritanceMode[index] |= INHERIT_SCALE;
  }
  else
  {
    mInheritanceMode[index] &= ~INHERIT_SCALE;
  }

  mComponentDirty[index] = true;
}

void TransformManager::SetInheritOrientation(TransformId id, bool inherit)
{
  TransformId index = mIds[id];
  if(inherit)
  {
    mInheritanceMode[index] |= INHERIT_ORIENTATION;
  }
  else
  {
    mInheritanceMode[index] &= ~INHERIT_ORIENTATION;
  }

  mComponentDirty[index] = true;
}

void TransformManager::ResetToBaseValue()
{
  if(mComponentCount)
  {
    DALI_TRACE_SCOPE(gTraceFilter, "DALI_TRANSFORM_RESET_TO_BASE");
    memcpy(&mTxComponentAnimatable[0], &mTxComponentAnimatableBaseValue[0], sizeof(TransformComponentAnimatable) * mComponentCount);
    memcpy(&mSize[0], &mSizeBase[0], sizeof(Vector3) * mComponentCount);
    memset(&mLocalMatrixDirty[0], false, sizeof(bool) * mComponentCount);
  }
}

bool TransformManager::Update()
{
  bool componentsChanged = false;

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
        if(mComponentDirty[i] || mLocalMatrixDirty[parentIndex])
        {
          //Full transform inherited
          mLocalMatrixDirty[i] = true;
          CalculateCenterPosition(centerPosition, mTxComponentStatic[i], mTxComponentAnimatable[i].mScale, mTxComponentAnimatable[i].mOrientation, mSize[i], half, topLeft);
          localPosition = mTxComponentAnimatable[i].mPosition + centerPosition + (mTxComponentStatic[i].mParentOrigin - half) * mSize[parentIndex];
          mLocal[i].SetTransformComponents(mTxComponentAnimatable[i].mScale, mTxComponentAnimatable[i].mOrientation, localPosition);
        }

        //Update the world matrix
        MatrixUtils::MultiplyTransformMatrix(mWorld[i], mLocal[i], mWorld[parentIndex]);
      }
      else
      {
        // Keep previous localMatrix for comparison.
        Matrix previousLocalMatrix = mLocal[i];

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

        mLocalMatrixDirty[i] = mComponentDirty[i] || (previousLocalMatrix != mLocal[i]);
      }
    }
    else //Component has no parent or doesn't inherit transform
    {
      Matrix localMatrix = mLocal[i];
      CalculateCenterPosition(centerPosition, mTxComponentStatic[i], mTxComponentAnimatable[i].mScale, mTxComponentAnimatable[i].mOrientation, mSize[i], half, topLeft);
      localPosition = mTxComponentAnimatable[i].mPosition + centerPosition;
      mLocal[i].SetTransformComponents(mTxComponentAnimatable[i].mScale, mTxComponentAnimatable[i].mOrientation, localPosition);
      mWorld[i]            = mLocal[i];
      mLocalMatrixDirty[i] = mComponentDirty[i] || (localMatrix != mLocal[i]);
    }

    //Update the bounding sphere
    Vec3 centerToEdge = {mSize[i].Length() * 0.5f, 0.0f, 0.0f};
    Vec3 centerToEdgeWorldSpace;
    TransformVector3(centerToEdgeWorldSpace, mWorld[i].AsFloat(), centerToEdge);

    mBoundingSpheres[i]   = mWorld[i].GetTranslation();
    mBoundingSpheres[i].w = Length(centerToEdgeWorldSpace);

    componentsChanged  = componentsChanged || mComponentDirty[i];
    mComponentDirty[i] = false;
  }

  DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_TRANSFORM_UPDATE", [&](std::ostringstream& oss) {
    oss << "[componentsChanged:" << componentsChanged << "]";
  });

  return componentsChanged;
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
}

Vector3& TransformManager::GetVector3PropertyValue(TransformId id, TransformManagerProperty property)
{
  switch(property)
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      TransformId index(mIds[id]);
      return mTxComponentAnimatable[index].mPosition;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      TransformId index(mIds[id]);
      return mTxComponentAnimatable[index].mScale;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      TransformId index(mIds[id]);
      return mTxComponentStatic[index].mParentOrigin;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      TransformId index(mIds[id]);
      return mTxComponentStatic[index].mAnchorPoint;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      TransformId index(mIds[id]);
      return mSize[index];
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
  mComponentDirty[index] = true;

  switch(property)
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      mTxComponentAnimatable[index].mPosition = value;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      mTxComponentAnimatable[index].mScale = value;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      mTxComponentStatic[index].mParentOrigin = value;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      mTxComponentStatic[index].mAnchorPoint = value;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      mSize[index] = value;
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
  mComponentDirty[index] = true;

  switch(property)
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      mTxComponentAnimatable[index].mPosition[component] = value;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      mTxComponentAnimatable[index].mScale[component] = value;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      mTxComponentStatic[index].mParentOrigin[component] = value;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      mTxComponentStatic[index].mAnchorPoint[component] = value;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      mSize[index][component] = value;
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
  mComponentDirty[index] = true;

  switch(property)
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      mTxComponentAnimatable[index].mPosition = mTxComponentAnimatableBaseValue[index].mPosition = value;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      mTxComponentAnimatable[index].mScale = mTxComponentAnimatableBaseValue[index].mScale = value;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      mTxComponentStatic[index].mParentOrigin = value;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      mTxComponentStatic[index].mAnchorPoint = value;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      mSize[index] = mSizeBase[index] = value;
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
  mComponentDirty[index] = true;

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
  mComponentDirty[index] = true;

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
  mComponentDirty[index] = true;

  switch(property)
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      mTxComponentAnimatable[index].mPosition[component] = mTxComponentAnimatableBaseValue[index].mPosition[component] = value;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      mTxComponentAnimatable[index].mScale[component] = mTxComponentAnimatableBaseValue[index].mScale[component] = value;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      mTxComponentStatic[index].mParentOrigin[component] = value;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      mTxComponentStatic[index].mAnchorPoint[component] = value;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      mSize[index][component] = mSizeBase[index][component] = value;
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
  mComponentDirty[index] = true;

  switch(property)
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      mTxComponentAnimatable[index].mPosition.x = mTxComponentAnimatableBaseValue[index].mPosition.x = value;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      mTxComponentAnimatable[index].mScale.x = mTxComponentAnimatableBaseValue[index].mScale.x = value;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      mTxComponentStatic[index].mParentOrigin.x = value;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      mTxComponentStatic[index].mAnchorPoint.x = value;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      mSize[index].x = mSizeBase[index].x = value;
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
  mComponentDirty[index] = true;

  switch(property)
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      mTxComponentAnimatable[index].mPosition.y = mTxComponentAnimatableBaseValue[index].mPosition.y = value;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      mTxComponentAnimatable[index].mScale.y = mTxComponentAnimatableBaseValue[index].mScale.y = value;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      mTxComponentStatic[index].mParentOrigin.y = value;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      mTxComponentStatic[index].mAnchorPoint.y = value;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      mSize[index].y = mSizeBase[index].y = value;
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
  mComponentDirty[index] = true;

  switch(property)
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      mTxComponentAnimatable[index].mPosition.z = mTxComponentAnimatableBaseValue[index].mPosition.z = value;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      mTxComponentAnimatable[index].mScale.z = mTxComponentAnimatableBaseValue[index].mScale.z = value;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      mTxComponentStatic[index].mParentOrigin.z = value;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      mTxComponentStatic[index].mAnchorPoint.z = value;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      mSize[index].z = mSizeBase[index].z = value;
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
  TransformId index(mIds[id]);
  return mTxComponentAnimatable[index].mOrientation;
}

const Quaternion& TransformManager::GetQuaternionPropertyValue(TransformId id) const
{
  return mTxComponentAnimatable[mIds[id]].mOrientation;
}

void TransformManager::SetQuaternionPropertyValue(TransformId id, const Quaternion& q)
{
  TransformId index(mIds[id]);
  mTxComponentAnimatable[index].mOrientation = q;
  mComponentDirty[index]                     = true;
}

void TransformManager::BakeQuaternionPropertyValue(TransformId id, const Quaternion& q)
{
  TransformId index(mIds[id]);
  mTxComponentAnimatable[index].mOrientation = mTxComponentAnimatableBaseValue[index].mOrientation = q;
  mComponentDirty[index]                                                                           = true;
}

void TransformManager::BakeRelativeQuaternionPropertyValue(TransformId id, const Quaternion& q)
{
  TransformId index(mIds[id]);
  mTxComponentAnimatable[index].mOrientation = mTxComponentAnimatableBaseValue[index].mOrientation = mTxComponentAnimatable[index].mOrientation * q;
  mComponentDirty[index]                                                                           = true;
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
  mComponentDirty[index]                             = true;
  mTxComponentStatic[index].mPositionUsesAnchorPoint = value;
}

} //namespace SceneGraph
} //namespace Internal
} //namespace Dali
