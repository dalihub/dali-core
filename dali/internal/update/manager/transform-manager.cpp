/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/constants.h>
#include <dali/internal/common/math.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

namespace
{
//Default values for scale (1.0,1.0,1.0), orientation (Identity) and position (0.0,0.0,0.0)
static const float gDefaultTransformComponentAnimatableData[] = { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f };

//Default values for anchor point (CENTER) and parent origin (TOP_LEFT)
static const float gDefaultTransformComponentStaticData[] = { 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.5f, true };

static_assert( sizeof(gDefaultTransformComponentAnimatableData) == sizeof(TransformComponentAnimatable), "gDefaultTransformComponentAnimatableData should have the same number of floats as specified in TransformComponentAnimatable" );
static_assert( sizeof(gDefaultTransformComponentStaticData) == sizeof(TransformComponentStatic), "gDefaultTransformComponentStaticData should have the same number of floats as specified in TransformComponentStatic" );

/**
 * @brief Calculates the center position for the transform component
 * @param[out] centerPosition The calculated center-position of the transform component
 * @param[in] transformComponentStatic A const reference to the static component transform struct
 * @param[in] transformComponentAnimatable A const reference to the animatable component transform struct
 * @param[in] size The size of the current transform component
 * @param[in] half Halfway point of the transform
 * @param[in] topLeft The top-left coords of the transform
 */
inline void CalculateCenterPosition(
  Vector3& centerPosition,
  const TransformComponentStatic& transformComponentStatic,
  const TransformComponentAnimatable& transformComponentAnimatable,
  const Vector3& size,
  const Vector3& half,
  const Vector3& topLeft )
{
  // Calculate the center-point by applying the scale and rotation on the anchor point.
  centerPosition = ( half - transformComponentStatic.mAnchorPoint ) * size * transformComponentAnimatable.mScale;
  centerPosition *= transformComponentAnimatable.mOrientation;

  // If the position is ignoring the anchor-point, then remove the anchor-point shift from the position.
  if( ! transformComponentStatic.mPositionUsesAnchorPoint )
  {
    centerPosition -= ( topLeft - transformComponentStatic.mAnchorPoint ) * size;
  }
}

} // unnamed namespace

TransformManager::TransformManager()
:mComponentCount(0),
 mReorder(false)
{}

TransformManager::~TransformManager()
{}

TransformId TransformManager::CreateTransform()
{
  //Get id for the new component
  TransformId id = mIds.Add(mComponentCount);

  if( mTxComponentAnimatable.Size() <= mComponentCount )
  {
    //Make room for another component
    mTxComponentAnimatable.PushBack(TransformComponentAnimatable());
    mTxComponentStatic.PushBack(TransformComponentStatic());
    mInheritanceMode.PushBack(INHERIT_ALL);
    mComponentId.PushBack(id);
    mSize.PushBack(Vector3(0.0f,0.0f,0.0f));
    mParent.PushBack(INVALID_TRANSFORM_ID);
    mWorld.PushBack(Matrix::IDENTITY);
    mLocal.PushBack(Matrix::IDENTITY);
    mBoundingSpheres.PushBack( Vector4(0.0f,0.0f,0.0f,0.0f) );
    mTxComponentAnimatableBaseValue.PushBack(TransformComponentAnimatable());
    mSizeBase.PushBack(Vector3(0.0f,0.0f,0.0f));
    mComponentDirty.PushBack(false);
    mLocalMatrixDirty.PushBack(false);
  }
  else
  {
    //Set default values
    memcpy( &mTxComponentAnimatable[mComponentCount], &gDefaultTransformComponentAnimatableData, sizeof( TransformComponentAnimatable ) );
    memcpy( &mTxComponentStatic[mComponentCount], &gDefaultTransformComponentStaticData, sizeof( TransformComponentStatic ) );
    memcpy( &mTxComponentAnimatableBaseValue[mComponentCount], &gDefaultTransformComponentAnimatableData, sizeof( TransformComponentAnimatable ) );
    mInheritanceMode[mComponentCount] = INHERIT_ALL;
    mComponentId[mComponentCount] = id;
    mSize[mComponentCount] = Vector3(0.0f,0.0f,0.0f);
    mParent[mComponentCount] = INVALID_TRANSFORM_ID;
    mLocal[mComponentCount].SetIdentity();
    mWorld[mComponentCount].SetIdentity();
    mBoundingSpheres[mComponentCount] = Vector4(0.0f,0.0f,0.0f,0.0f);
    mSizeBase[mComponentCount] = Vector3(0.0f,0.0f,0.0f);
    mComponentDirty[mComponentCount] = false;
    mLocalMatrixDirty[mComponentCount] = false;
  }

  mComponentCount++;
  return id;
}

void TransformManager::RemoveTransform(TransformId id)
{
  //Move the last element to the gap
  mComponentCount--;
  unsigned int index = mIds[id];
  mTxComponentAnimatable[index] = mTxComponentAnimatable[mComponentCount];
  mTxComponentStatic[index] = mTxComponentStatic[mComponentCount];
  mInheritanceMode[index] = mInheritanceMode[mComponentCount];
  mSize[index] = mSize[mComponentCount];
  mParent[index] = mParent[mComponentCount];
  mWorld[index] = mWorld[mComponentCount];
  mLocal[index] = mLocal[mComponentCount];
  mTxComponentAnimatableBaseValue[index] = mTxComponentAnimatableBaseValue[mComponentCount];
  mSizeBase[index] = mSizeBase[mComponentCount];
  mComponentDirty[index] = mComponentDirty[mComponentCount];
  mLocalMatrixDirty[index] = mLocalMatrixDirty[mComponentCount];
  mBoundingSpheres[index] = mBoundingSpheres[mComponentCount];

  TransformId lastItemId = mComponentId[mComponentCount];
  mIds[ lastItemId ] = index;
  mComponentId[index] = lastItemId;
  mIds.Remove( id );

  mReorder = true;
}

void TransformManager::SetParent( TransformId id, TransformId parentId )
{
  DALI_ASSERT_ALWAYS( id != parentId );
  unsigned int index = mIds[id];
  mParent[ index ] = parentId;
  mComponentDirty[ index ] = true;
  mReorder = true;
}

const Matrix& TransformManager::GetWorldMatrix( TransformId id ) const
{
  return mWorld[ mIds[id] ];
}

Matrix& TransformManager::GetWorldMatrix( TransformId id )
{
  return mWorld[ mIds[id] ];
}

void TransformManager::SetInheritPosition( TransformId id, bool inherit )
{
  unsigned int index = mIds[id];
  if( inherit )
  {
    mInheritanceMode[ index ] |= INHERIT_POSITION;
  }
  else
  {
    mInheritanceMode[ index ] &= ~INHERIT_POSITION;
  }

  mComponentDirty[index] = true;
}

void TransformManager::SetInheritScale( TransformId id, bool inherit )
{
  unsigned int index = mIds[id];
  if( inherit )
  {
    mInheritanceMode[ index ] |= INHERIT_SCALE;
  }
  else
  {
    mInheritanceMode[ index ] &= ~INHERIT_SCALE;
  }

  mComponentDirty[index] = true;
}

void TransformManager::SetInheritOrientation( TransformId id, bool inherit )
{
  unsigned int index = mIds[id];
  if( inherit )
  {
    mInheritanceMode[ index ] |= INHERIT_ORIENTATION;
  }
  else
  {
    mInheritanceMode[ index ] &= ~INHERIT_ORIENTATION;
  }

  mComponentDirty[index] = true;
}

void TransformManager::ResetToBaseValue()
{
  if( mComponentCount )
  {
    memcpy( &mTxComponentAnimatable[0], &mTxComponentAnimatableBaseValue[0], sizeof(TransformComponentAnimatable)*mComponentCount );
    memcpy( &mSize[0], &mSizeBase[0], sizeof(Vector3)*mComponentCount );
    memset( &mLocalMatrixDirty[0], false, sizeof(bool)*mComponentCount );
  }
}

void TransformManager::Update()
{
  if( mReorder )
  {
    //If some transform component has change its parent or has been removed since last update
    //we need to reorder the vectors
    ReorderComponents();
    mReorder = false;
  }

  //Iterate through all components to compute its world matrix
  Vector3 centerPosition;
  Vector3 localPosition;
  const Vector3 half( 0.5f,0.5f,0.5f );
  const Vector3 topLeft( 0.0f, 0.0f, 0.5f );
  for( unsigned int i(0); i<mComponentCount; ++i )
  {
    if( DALI_LIKELY( mInheritanceMode[i] != DONT_INHERIT_TRANSFORM && mParent[i] != INVALID_TRANSFORM_ID ) )
    {
      const unsigned int& parentIndex = mIds[mParent[i] ];
      if( DALI_LIKELY( mInheritanceMode[i] == INHERIT_ALL ) )
      {
        if( mComponentDirty[i] || mLocalMatrixDirty[parentIndex])
        {
          //Full transform inherited
          mLocalMatrixDirty[i] = true;
          CalculateCenterPosition( centerPosition, mTxComponentStatic[ i ], mTxComponentAnimatable[ i ], mSize[ i ], half, topLeft );
          localPosition = mTxComponentAnimatable[i].mPosition + centerPosition + ( mTxComponentStatic[i].mParentOrigin - half ) *  mSize[parentIndex];
          mLocal[i].SetTransformComponents( mTxComponentAnimatable[i].mScale, mTxComponentAnimatable[i].mOrientation, localPosition );
        }

        //Update the world matrix
        Matrix::Multiply( mWorld[i], mLocal[i], mWorld[parentIndex]);
      }
      else
      {
        //Some components are not inherited
        Vector3 parentPosition, parentScale;
        Quaternion parentOrientation;
        const Matrix& parentMatrix = mWorld[parentIndex];
        parentMatrix.GetTransformComponents( parentPosition, parentOrientation, parentScale );

        Vector3 localScale = mTxComponentAnimatable[i].mScale;
        if( (mInheritanceMode[i] & INHERIT_SCALE) == 0 )
        {
          //Don't inherit scale
          localScale /= parentScale;
        }

        Quaternion localOrientation( mTxComponentAnimatable[i].mOrientation );
        if( (mInheritanceMode[i] & INHERIT_ORIENTATION) == 0 )
        {
          //Don't inherit orientation
          parentOrientation.Invert();
          localOrientation = parentOrientation * mTxComponentAnimatable[i].mOrientation;
        }

        if( (mInheritanceMode[i] & INHERIT_POSITION) == 0 )
        {
          //Don't inherit position
          CalculateCenterPosition( centerPosition, mTxComponentStatic[ i ], mTxComponentAnimatable[ i ], mSize[ i ], half, topLeft );
          mLocal[i].SetTransformComponents( localScale, localOrientation, Vector3::ZERO );
          Matrix::Multiply( mWorld[i], mLocal[i], parentMatrix );
          mWorld[i].SetTranslation( mTxComponentAnimatable[i].mPosition + centerPosition );
        }
        else
        {
          CalculateCenterPosition( centerPosition, mTxComponentStatic[ i ], mTxComponentAnimatable[ i ], mSize[ i ], half, topLeft );
          localPosition = mTxComponentAnimatable[i].mPosition + centerPosition + ( mTxComponentStatic[i].mParentOrigin - half ) *  mSize[parentIndex];
          mLocal[i].SetTransformComponents( localScale, localOrientation, localPosition );
          Matrix::Multiply( mWorld[i], mLocal[i], parentMatrix );
        }

        mLocalMatrixDirty[i] = true;
      }
    }
    else  //Component has no parent or doesn't inherit transform
    {
      CalculateCenterPosition( centerPosition, mTxComponentStatic[ i ], mTxComponentAnimatable[ i ], mSize[ i ], half, topLeft );
      localPosition = mTxComponentAnimatable[i].mPosition + centerPosition;
      mLocal[i].SetTransformComponents( mTxComponentAnimatable[i].mScale, mTxComponentAnimatable[i].mOrientation, localPosition );
      mWorld[i] = mLocal[i];
      mLocalMatrixDirty[i] = true;
    }

    //Update the bounding sphere
    Vec3 centerToEdge = { mSize[i].Length() * 0.5f, 0.0f, 0.0f };
    Vec3 centerToEdgeWorldSpace;
    TransformVector3( centerToEdgeWorldSpace, mWorld[i].AsFloat(), centerToEdge );

    mBoundingSpheres[i] = mWorld[i].GetTranslation();
    mBoundingSpheres[i].w = Length( centerToEdgeWorldSpace );

    mComponentDirty[i] = false;
  }
}

void TransformManager::SwapComponents( unsigned int i, unsigned int j )
{
  std::swap( mTxComponentAnimatable[i], mTxComponentAnimatable[j] );
  std::swap( mTxComponentStatic[i], mTxComponentStatic[j] );
  std::swap( mInheritanceMode[i], mInheritanceMode[j] );
  std::swap( mSize[i], mSize[j] );
  std::swap( mParent[i], mParent[j] );
  std::swap( mComponentId[i], mComponentId[j] );
  std::swap( mTxComponentAnimatableBaseValue[i], mTxComponentAnimatableBaseValue[j] );
  std::swap( mSizeBase[i], mSizeBase[j] );
  std::swap( mLocal[i], mLocal[j] );
  std::swap( mComponentDirty[i], mComponentDirty[j] );
  std::swap( mBoundingSpheres[i], mBoundingSpheres[j] );
  std::swap( mWorld[i], mWorld[j] );

  mIds[ mComponentId[i] ] = i;
  mIds[ mComponentId[j] ] = j;
}

void TransformManager::ReorderComponents()
{
  mOrderedComponents.Resize(mComponentCount);

  TransformId parentId;
  for( size_t i(0); i<mComponentCount; ++i )
  {
    mOrderedComponents[i].id = mComponentId[i];
    mOrderedComponents[i].level = 0u;

    parentId = mParent[i];
    while( parentId != INVALID_TRANSFORM_ID )
    {
      mOrderedComponents[i].level++;
      parentId = mParent[ mIds[parentId] ];
    }
  }

  std::stable_sort( mOrderedComponents.Begin(), mOrderedComponents.End());
  unsigned int previousIndex = 0;
  for( size_t newIndex(0); newIndex<mComponentCount-1; ++newIndex )
  {
    previousIndex = mIds[mOrderedComponents[newIndex].id];
    if( previousIndex != newIndex )
    {
      SwapComponents( previousIndex, newIndex);
    }
  }
}

Vector3& TransformManager::GetVector3PropertyValue( TransformId id, TransformManagerProperty property )
{
  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      unsigned int index( mIds[id] );
      mComponentDirty[ index ] = true;
      return mTxComponentAnimatable[ index ].mPosition;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      unsigned int index( mIds[id] );
      mComponentDirty[ index ] = true;
      return mTxComponentAnimatable[ index ].mScale;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      unsigned int index( mIds[id] );
      mComponentDirty[ index ] = true;
      return mTxComponentStatic[ index ].mParentOrigin;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      unsigned int index( mIds[id] );
      mComponentDirty[ index ] = true;
      return mTxComponentStatic[ index ].mAnchorPoint;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      unsigned int index( mIds[id] );
      mComponentDirty[ index ] = true;
      return mSize[ index ];
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
      return mTxComponentAnimatable[ mIds[id] ].mPosition;
    }
  }
}

const Vector3& TransformManager::GetVector3PropertyValue( TransformId id, TransformManagerProperty property ) const
{
  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      return mTxComponentAnimatable[ mIds[id] ].mPosition;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      return mTxComponentAnimatable[ mIds[id] ].mScale;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      return mTxComponentStatic[ mIds[id] ].mParentOrigin;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      return mTxComponentStatic[ mIds[id] ].mAnchorPoint;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      return mSize[ mIds[id] ];
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
      return mTxComponentAnimatable[ mIds[id] ].mPosition;
    }
  }
}

const float& TransformManager::GetVector3PropertyComponentValue(TransformId id, TransformManagerProperty property, unsigned int component ) const
{
  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      return mTxComponentAnimatable[ mIds[id] ].mPosition[component];
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      return mTxComponentAnimatable[ mIds[id] ].mScale[component];
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      return mTxComponentStatic[ mIds[id] ].mParentOrigin[component];
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      return mTxComponentStatic[ mIds[id] ].mAnchorPoint[component];
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      return mSize[ mIds[id] ][component];
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
      return mTxComponentAnimatable[ mIds[id] ].mPosition[component];
    }
  }
}

void TransformManager::SetVector3PropertyValue( TransformId id, TransformManagerProperty property, const Vector3& value )
{
  unsigned int index( mIds[id] );
  mComponentDirty[ index ] = true;

  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      mTxComponentAnimatable[ index ].mPosition = value;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      mTxComponentAnimatable[ index ].mScale = value;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      mTxComponentStatic[ index ].mParentOrigin = value;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      mTxComponentStatic[ index ].mAnchorPoint = value;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      mSize[ index ] = value;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

void TransformManager::SetVector3PropertyComponentValue( TransformId id, TransformManagerProperty property, float value, unsigned int component )
{
  unsigned int index( mIds[id] );
  mComponentDirty[ index ] = true;

  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      mTxComponentAnimatable[ index ].mPosition[component] = value;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      mTxComponentAnimatable[ index ].mScale[component] = value;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      mTxComponentStatic[ index ].mParentOrigin[component] = value;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      mTxComponentStatic[ index ].mAnchorPoint[component] = value;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      mSize[ index ][component] = value;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

void TransformManager::BakeVector3PropertyValue( TransformId id, TransformManagerProperty property, const Vector3& value )
{
  unsigned int index( mIds[id] );
  mComponentDirty[ index ] = true;

  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      mTxComponentAnimatable[ index ].mPosition = mTxComponentAnimatableBaseValue[index].mPosition = value;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      mTxComponentAnimatable[ index ].mScale = mTxComponentAnimatableBaseValue[index].mScale = value;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      mTxComponentStatic[ index ].mParentOrigin = value;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      mTxComponentStatic[ index ].mAnchorPoint = value;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      mSize[ index ] = mSizeBase[index] = value;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

void TransformManager::BakeRelativeVector3PropertyValue( TransformId id, TransformManagerProperty property, const Vector3& value )
{
  unsigned int index( mIds[id] );
  mComponentDirty[ index ] = true;

  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      mTxComponentAnimatable[ index ].mPosition = mTxComponentAnimatableBaseValue[index].mPosition = mTxComponentAnimatable[ index ].mPosition + value;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      mTxComponentAnimatable[ index ].mScale = mTxComponentAnimatableBaseValue[index].mScale = mTxComponentAnimatable[ index ].mScale + value;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      mTxComponentStatic[ index ].mParentOrigin = mTxComponentStatic[ index ].mParentOrigin + value;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      mTxComponentStatic[ index ].mAnchorPoint = mTxComponentStatic[ index ].mAnchorPoint + value;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      mSize[ index ] = mSizeBase[index] = mSize[ index ] + value;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

void TransformManager::BakeMultiplyVector3PropertyValue( TransformId id, TransformManagerProperty property, const Vector3& value )
{
  unsigned int index( mIds[id] );
  mComponentDirty[ index ] = true;

  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      mTxComponentAnimatable[ index ].mPosition = mTxComponentAnimatableBaseValue[index].mPosition = mTxComponentAnimatable[ index ].mPosition * value;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      mTxComponentAnimatable[ index ].mScale = mTxComponentAnimatableBaseValue[index].mScale = mTxComponentAnimatable[ index ].mScale * value;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      mTxComponentStatic[ index ].mParentOrigin = mTxComponentStatic[ index ].mParentOrigin * value;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      mTxComponentStatic[ index ].mAnchorPoint = mTxComponentStatic[ index ].mAnchorPoint * value;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      mSize[ index ] = mSizeBase[index] = mSize[ index ] * value;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

void TransformManager::BakeVector3PropertyComponentValue( TransformId id, TransformManagerProperty property, float value, unsigned int component )
{
  unsigned int index( mIds[id] );
  mComponentDirty[ index ] = true;

  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      mTxComponentAnimatable[ index ].mPosition[component] = mTxComponentAnimatableBaseValue[index].mPosition[component] = value;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      mTxComponentAnimatable[ index ].mScale[component] = mTxComponentAnimatableBaseValue[index].mScale[component] = value;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      mTxComponentStatic[ index ].mParentOrigin[component] = value;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      mTxComponentStatic[ index ].mAnchorPoint[component] = value;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      mSize[ index ][component] = mSizeBase[index][component] = value;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

void TransformManager::BakeXVector3PropertyValue( TransformId id, TransformManagerProperty property, float value )
{
  unsigned int index( mIds[id] );
  mComponentDirty[ index ] = true;

  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      mTxComponentAnimatable[ index ].mPosition.x = mTxComponentAnimatableBaseValue[index].mPosition.x = value;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      mTxComponentAnimatable[ index ].mScale.x = mTxComponentAnimatableBaseValue[index].mScale.x = value;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      mTxComponentStatic[ index ].mParentOrigin.x = value;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      mTxComponentStatic[ index ].mAnchorPoint.x = value;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      mSize[ index ].x = mSizeBase[index].x = value;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

void TransformManager::BakeYVector3PropertyValue( TransformId id, TransformManagerProperty property, float value )
{
  unsigned int index( mIds[id] );
  mComponentDirty[ index ] = true;

  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      mTxComponentAnimatable[ index ].mPosition.y = mTxComponentAnimatableBaseValue[index].mPosition.y = value;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      mTxComponentAnimatable[ index ].mScale.y = mTxComponentAnimatableBaseValue[index].mScale.y = value;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      mTxComponentStatic[ index ].mParentOrigin.y = value;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      mTxComponentStatic[ index ].mAnchorPoint.y = value;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      mSize[ index ].y = mSizeBase[index].y = value;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

void TransformManager::BakeZVector3PropertyValue( TransformId id, TransformManagerProperty property, float value )
{
  unsigned int index( mIds[id] );
  mComponentDirty[ index ] = true;

  switch( property )
  {
    case TRANSFORM_PROPERTY_POSITION:
    {
      mTxComponentAnimatable[ index ].mPosition.z = mTxComponentAnimatableBaseValue[index].mPosition.z = value;
      break;
    }
    case TRANSFORM_PROPERTY_SCALE:
    {
      mTxComponentAnimatable[ index ].mScale.z = mTxComponentAnimatableBaseValue[index].mScale.z = value;
      break;
    }
    case TRANSFORM_PROPERTY_PARENT_ORIGIN:
    {
      mTxComponentStatic[ index ].mParentOrigin.z = value;
      break;
    }
    case TRANSFORM_PROPERTY_ANCHOR_POINT:
    {
      mTxComponentStatic[ index ].mAnchorPoint.z = value;
      break;
    }
    case TRANSFORM_PROPERTY_SIZE:
    {
      mSize[ index ].z = mSizeBase[index].z = value;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false);
    }
  }
}

Quaternion& TransformManager::GetQuaternionPropertyValue( TransformId id )
{
  unsigned int index( mIds[id] );
  mComponentDirty[ index ] = true;
  return mTxComponentAnimatable[ index ].mOrientation;
}

const Quaternion& TransformManager::GetQuaternionPropertyValue( TransformId id ) const
{
  return mTxComponentAnimatable[ mIds[id] ].mOrientation;
}

void TransformManager::SetQuaternionPropertyValue( TransformId id, const Quaternion& q )
{
  unsigned int index( mIds[id] );
  mTxComponentAnimatable[ index ].mOrientation = q;
  mComponentDirty[ index ] = true;
}

void TransformManager::BakeQuaternionPropertyValue( TransformId id, const Quaternion& q )
{
  unsigned int index( mIds[id] );
  mTxComponentAnimatable[ index ].mOrientation = mTxComponentAnimatableBaseValue[index].mOrientation = q;
  mComponentDirty[ index ] = true;
}

void TransformManager::BakeRelativeQuaternionPropertyValue( TransformId id, const Quaternion& q )
{
  unsigned int index( mIds[id] );
  mTxComponentAnimatable[ index ].mOrientation = mTxComponentAnimatableBaseValue[index].mOrientation = mTxComponentAnimatable[ index ].mOrientation * q;
  mComponentDirty[ index ] = true;
}

const Vector4& TransformManager::GetBoundingSphere( TransformId id ) const
{
  return mBoundingSpheres[ mIds[id] ];
}

void TransformManager::GetWorldMatrixAndSize( TransformId id, Matrix& worldMatrix, Vector3& size ) const
{
  unsigned int index = mIds[id];
  worldMatrix = mWorld[index];
  size = mSize[index];
}

void TransformManager::SetPositionUsesAnchorPoint( TransformId id, bool value )
{
  unsigned int index( mIds[ id ] );
  mComponentDirty[ index ] = true;
  mTxComponentStatic[ index ].mPositionUsesAnchorPoint = value;
}

} //namespace SceneGraph
} //namespace Internal
} //namespace Dali
