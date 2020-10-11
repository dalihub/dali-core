/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/animation/key-frames-impl.h>

namespace Dali
{
namespace Internal
{

KeyFrames* KeyFrames::New()
{
  return new KeyFrames();
}

void KeyFrames::CreateKeyFramesSpec(Property::Type type)
{
  mType = type;
  // Now we have a type, can create specific implementation
  switch(type)
  {
    case Property::BOOLEAN:
    {
      mKeyFrames = std::make_unique<Internal::KeyFrameBoolean>();
      break;
    }
    case Property::INTEGER:
    {
      mKeyFrames = std::make_unique<Internal::KeyFrameInteger>();
      break;
    }
    case Property::FLOAT:
    {
      mKeyFrames = std::make_unique<Internal::KeyFrameNumber>();
      break;
    }
    case Property::VECTOR2:
    {
      mKeyFrames = std::make_unique<Internal::KeyFrameVector2>();
      break;
    }
    case Property::VECTOR3:
    {
      mKeyFrames = std::make_unique<Internal::KeyFrameVector3>();
      break;
    }
    case Property::VECTOR4:
    {
      mKeyFrames = std::make_unique<Internal::KeyFrameVector4>();
      break;
    }
    case Property::ROTATION:
    {
      mKeyFrames = std::make_unique<Internal::KeyFrameQuaternion>();
      break;
    }
    default:
    {
      DALI_ABORT( "Property type is not animatable" );
      break;
    }
  }
}

Property::Type KeyFrames::GetType() const
{
  return mType;
}

void KeyFrames::Add(float time, const Property::Value& value, AlphaFunction alpha)
{
  if(mType == Property::NONE)
  {
    CreateKeyFramesSpec(value.GetType());
  }

  // Once we have created a type, can only add values of the same type
  DALI_ASSERT_ALWAYS( mType == value.GetType() && "Can only add values of the same type to a KeyFrame" );

  auto keyframes = mKeyFrames.get();

  DALI_ASSERT_DEBUG(keyframes);

  switch(mType)
  {
    case Property::BOOLEAN:
    {
      Internal::KeyFrameBoolean* kf = static_cast<Internal::KeyFrameBoolean*>(keyframes);
      kf->AddKeyFrame(time, value.Get<bool>(), alpha);
      break;
    }
    case Property::INTEGER:
    {
      Internal::KeyFrameInteger* kf = static_cast<Internal::KeyFrameInteger*>(keyframes);
      kf->AddKeyFrame(time, value.Get<int>(), alpha);
      break;
    }
    case Property::FLOAT:
    {
      Internal::KeyFrameNumber* kf = static_cast<Internal::KeyFrameNumber*>(keyframes);
      kf->AddKeyFrame(time, value.Get<float>(), alpha);
      break;
    }
    case Property::VECTOR2:
    {
      Internal::KeyFrameVector2* kf = static_cast<Internal::KeyFrameVector2*>(keyframes);
      kf->AddKeyFrame(time, value.Get<Vector2>(), alpha);
      break;
    }
    case Property::VECTOR3:
    {
      Internal::KeyFrameVector3* kf = static_cast<Internal::KeyFrameVector3*>(keyframes);
      kf->AddKeyFrame(time, value.Get<Vector3>(), alpha);
      break;
    }
    case Property::VECTOR4:
    {
      Internal::KeyFrameVector4* kf = static_cast<Internal::KeyFrameVector4*>(keyframes);
      kf->AddKeyFrame(time, value.Get<Vector4>(), alpha);
      break;
    }
    case Property::ROTATION:
    {
      Internal::KeyFrameQuaternion* kf = static_cast<Internal::KeyFrameQuaternion*>(keyframes);
      kf->AddKeyFrame(time, value.Get<Quaternion>(), alpha);
      break;
    }
    default:
      DALI_ASSERT_DEBUG(!"Type not supported");
      break;
  }
}

KeyFrameSpec* KeyFrames::GetKeyFramesBase() const
{
  return mKeyFrames.get();
}

Property::Value KeyFrames::GetLastKeyFrameValue() const
{
  Property::Value value;

  std::size_t noOfKeyFrames = mKeyFrames->GetNumberOfKeyFrames();
  if( noOfKeyFrames )
  {
    mKeyFrames->GetKeyFrameAsValue( noOfKeyFrames - 1, value );
  }

  return value;
}

} // Internal
} // Dali
