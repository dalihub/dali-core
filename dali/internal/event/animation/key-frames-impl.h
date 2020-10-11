#ifndef DALI_INTERNAL_KEY_FRAMES_H
#define DALI_INTERNAL_KEY_FRAMES_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <memory>

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/animation/key-frames.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/animation/alpha-function.h>
#include <dali/internal/event/animation/key-frame-channel.h>

namespace Dali
{
namespace Internal
{
class KeyFrameSpec;
class KeyFrames;

/**
 * KeyFrames class is responsible for creating and building a specialized KeyFrame class
 * from the Property::Value type used in Add.
 */
class KeyFrames : public BaseObject
{
public:
  static KeyFrames* New();

private:

  /**
   * Create a specialization from the given type, and store it to the mSpec
   * member variable
   */
  void CreateKeyFramesSpec(Property::Type type);

public:
  /**
   * Get the type of this key frame.
   * An empty key frame will return Property::NONE, wheras an initialised
   * key frame object will return the type of it's first element.
   * @return the Property::Type of the key frame values
   */
  Property::Type GetType() const;

  /**
   * Add a key frame. The first key frame to be added denotes the type
   * of all subsequent key frames. If a value with a different type is
   * added, it will throw a run time assert.
   * @param[in] time The time (between 0 and 1)
   * @param[in] value The value of the keyframe at the given time
   * @param[in] alpha An alpha function to blend between this key frame and the
   * next key frame.
   */
  void Add(float time, const Property::Value& value, AlphaFunction alpha);

  /**
   * Return the key frames without specialization. The GetSpecialization methods
   * below will convert to the specialized objects.
   */
  KeyFrameSpec* GetKeyFramesBase() const;

  /**
   * Return the value of the last key frame.
   */
  Dali::Property::Value GetLastKeyFrameValue() const;

private:
  Dali::Property::Type          mType{Property::NONE}; // Type of the specialization
  std::unique_ptr<KeyFrameSpec> mKeyFrames;            // Pointer to the specialized key frame object
};

/**
 * This is the base class for the individual template specializations, allowing a ptr to be
 * stored in the handle object above.
 */
class KeyFrameSpec
{
public:
  virtual ~KeyFrameSpec() = default;

  virtual std::size_t GetNumberOfKeyFrames() const = 0;

  /**
   * Get the key frame value as a Property::Value.
   * @param[in] index The index of the key frame to fetch
   * @param[out] value The value of the given key frame
   */
  virtual void GetKeyFrameAsValue( std::size_t index, Property::Value& value ) = 0;
};

/**
 * The base template class for each key frame specialization.
 */
template<typename V>
class KeyFrameBaseSpec : public KeyFrameSpec
{
  KeyFrameChannel<V> mChannel; // The key frame channel

public:
  /**
   * Add a key frame to the channel. Key frames should be added
   * in time order (this method does not sort the vector by time)
   * @param[in] t - progress
   * @param[in] v - value
   * @param[in] alpha - Alpha function for blending to the next keyframe
   */
  void AddKeyFrame(float t, V v, AlphaFunction alpha)
  {
    mChannel.mValues.push_back({t, v});
  }

  /**
   * Get the number of key frames
   * @return Channel size
   */
  std::size_t GetNumberOfKeyFrames() const override
  {
    return mChannel.mValues.size();
  }

  /**
   * Get a key frame.
   * @param[in] index The index of the key frame to fetch
   * @param[out] time The progress of the given key frame
   * @param[out] value The value of the given key frame
   */
  void GetKeyFrame(unsigned int index, float& time, V& value) const
  {
    DALI_ASSERT_ALWAYS(index < mChannel.mValues.size() && "KeyFrame index is out of bounds");
    const auto& element = mChannel.mValues[index];
    time                = element.mProgress;
    value               = element.mValue;
  }

  /**
   * @copydoc KeyFrameSpec::GetKeyFrameAsValue()
   */
  void GetKeyFrameAsValue( std::size_t index, Property::Value& value ) override
  {
    value = mChannel.mValues[index].mValue;
  }

  /**
   * Return whether the progress is valid for the range of keyframes. (The first
   * keyframe doesn't have to start at 0, and the last doesn't have to end at 1.0)
   * @param[in] progress The progress to test
   * @return True if the progress is valid for this object
   */
  bool IsActive(float progress) const
  {
    return mChannel.IsActive(progress);
  }

  /**
   * Return an interpolated value for the given progress.
   * @param[in] progress The progress to test
   * @return The interpolated value
   */
  V GetValue(float progress, Dali::Animation::Interpolation interpolation) const
  {
    return mChannel.GetValue(progress, interpolation);
  }
};

using KeyFrameNumber     = KeyFrameBaseSpec<float>;
using KeyFrameBoolean    = KeyFrameBaseSpec<bool>;
using KeyFrameInteger    = KeyFrameBaseSpec<int>;
using KeyFrameVector2    = KeyFrameBaseSpec<Vector2>;
using KeyFrameVector3    = KeyFrameBaseSpec<Vector3>;
using KeyFrameVector4    = KeyFrameBaseSpec<Vector4>;
using KeyFrameQuaternion = KeyFrameBaseSpec<Quaternion>;

template<typename DeriveClass>
auto GetSpecialization(const Internal::KeyFrames& keyFrames)
{
  return static_cast<DeriveClass>(keyFrames.GetKeyFramesBase());
}

} // Internal


// Get impl of handle
inline Internal::KeyFrames& GetImplementation(Dali::KeyFrames& keyFrames)
{
  DALI_ASSERT_ALWAYS( keyFrames && "KeyFrames handle is empty" );
  Dali::RefObject& object = keyFrames.GetBaseObject();
  return static_cast<Internal::KeyFrames&>(object);
}

inline const Internal::KeyFrames& GetImplementation(const Dali::KeyFrames& keyFrames)
{
  DALI_ASSERT_ALWAYS( keyFrames && "KeyFrames handle is empty" );
  const Dali::RefObject& object = keyFrames.GetBaseObject();
  return static_cast<const Internal::KeyFrames&>(object);
}


} // Dali

#endif // DALI_INTERNAL_KEY_FRAMES_H
