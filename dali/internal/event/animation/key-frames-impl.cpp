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
#include <dali/internal/event/animation/key-frames-impl.h>

namespace Dali
{
namespace Internal
{

KeyFrames* KeyFrames::New()
{
  return new KeyFrames();
}

KeyFrames::KeyFrames()
  : mType(Property::NONE),
    mKeyFrames(NULL)
{
}

KeyFrames::~KeyFrames()
{
}


void KeyFrames::CreateKeyFramesSpec(Property::Type type)
{
  mType = type;
  // Now we have a type, can create specific implementation
  switch(type)
  {
    case Property::BOOLEAN:
    {
      mKeyFrames = Internal::KeyFrameBoolean::New();
      break;
    }
    case Property::INTEGER:
    {
      mKeyFrames = Internal::KeyFrameInteger::New();
      break;
    }
    case Property::FLOAT:
    {
      mKeyFrames = Internal::KeyFrameNumber::New();
      break;
    }
    case Property::VECTOR2:
    {
      mKeyFrames = Internal::KeyFrameVector2::New();
      break;
    }
    case Property::VECTOR3:
    {
      mKeyFrames = Internal::KeyFrameVector3::New();
      break;
    }
    case Property::VECTOR4:
    {
      mKeyFrames = Internal::KeyFrameVector4::New();
      break;
    }
    case Property::ROTATION:
    {
      mKeyFrames = Internal::KeyFrameQuaternion::New();
      break;
    }
    default:
    {
      DALI_ABORT( "Type not animateable" );
      break;
    }
  }
}

Property::Type KeyFrames::GetType() const
{
  return mType;
}

void KeyFrames::Add(float time, Property::Value value, AlphaFunction alpha)
{
  if(mType == Property::NONE)
  {
    CreateKeyFramesSpec(value.GetType());
  }

  // Once we have created a type, can only add values of the same type
  DALI_ASSERT_ALWAYS( mType == value.GetType() && "Can only add values of the same type to a KeyFrame" );

  DALI_ASSERT_DEBUG(mKeyFrames);

  switch(mType)
  {
    case Property::BOOLEAN:
    {
      Internal::KeyFrameBoolean* kf = static_cast<Internal::KeyFrameBoolean*>(mKeyFrames.Get());
      kf->AddKeyFrame(time, value.Get<bool>(), alpha);
      break;
    }
    case Property::INTEGER:
    {
      Internal::KeyFrameInteger* kf = static_cast<Internal::KeyFrameInteger*>(mKeyFrames.Get());
      kf->AddKeyFrame(time, value.Get<int>(), alpha);
      break;
    }
    case Property::FLOAT:
    {
      Internal::KeyFrameNumber* kf = static_cast<Internal::KeyFrameNumber*>(mKeyFrames.Get());
      kf->AddKeyFrame(time, value.Get<float>(), alpha);
      break;
    }
    case Property::VECTOR2:
    {
      Internal::KeyFrameVector2* kf = static_cast<Internal::KeyFrameVector2*>(mKeyFrames.Get());
      kf->AddKeyFrame(time, value.Get<Vector2>(), alpha);
      break;
    }
    case Property::VECTOR3:
    {
      Internal::KeyFrameVector3* kf = static_cast<Internal::KeyFrameVector3*>(mKeyFrames.Get());
      kf->AddKeyFrame(time, value.Get<Vector3>(), alpha);
      break;
    }
    case Property::VECTOR4:
    {
      Internal::KeyFrameVector4* kf = static_cast<Internal::KeyFrameVector4*>(mKeyFrames.Get());
      kf->AddKeyFrame(time, value.Get<Vector4>(), alpha);
      break;
    }
    case Property::ROTATION:
    {
      Internal::KeyFrameQuaternion* kf = static_cast<Internal::KeyFrameQuaternion*>(mKeyFrames.Get());
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
  return mKeyFrames.Get();
}

Property::Value KeyFrames::GetLastKeyFrameValue() const
{
  Property::Value value;

  unsigned int noOfKeyFrames = mKeyFrames->GetNumberOfKeyFrames();
  if( noOfKeyFrames )
  {
    mKeyFrames->GetKeyFrameAsValue( noOfKeyFrames - 1, value );
  }

  return value;
}

} // Internal
} // Dali
