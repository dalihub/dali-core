#ifndef __DALI_INTERNAL_KEY_FRAMES_H__
#define __DALI_INTERNAL_KEY_FRAMES_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/animation/key-frames.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/animation/alpha-functions.h>
#include <dali/internal/event/animation/progress-value.h>
#include <dali/internal/event/animation/key-frame-channel.h>

namespace Dali
{
namespace Internal
{
class KeyFrameSpec;
class KeyFrames;
typedef IntrusivePtr<KeyFrames> KeyFramesPtr;


/**
 * KeyFrames class is responsible for creating and building a specialized KeyFrame class
 * from the Property::Value type used in Add.
 */
class KeyFrames : public BaseObject
{
public:
  static KeyFrames* New();

  /**
   * Instantiate an empty KeyFrames object
   */
  KeyFrames();

protected:
  virtual ~KeyFrames();

private:
  /**
   * Don't allow copy constructor
   */
  KeyFrames(const KeyFrames& rhs);

  /**
   * Don't allow copy operator
   */
  KeyFrames& operator=(const KeyFrames& rhs);

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
  void Add(float time, Property::Value value, AlphaFunction alpha);

  /**
   * Return the key frames without specialization. The GetSpecialization methods
   * below will convert to the specialized objects.
   */
  KeyFrameSpec* GetKeyFramesBase() const;

private:
  Dali::Property::Type mType; // Type of the specialization
  IntrusivePtr<KeyFrameSpec> mKeyFrames;   // Pointer to the specialized key frame object
};


/**
 * This is the base class for the individual template specializations, allowing a ptr to be
 * stored in the handle object above. It inherits from RefObject to allow smart pointers
 * to be used for the specializations. Note that the derived template class below
 * allows for a copy constructor so that the specialization object can be cloned before
 * being passed to the scene-graph for animation.
 */
class KeyFrameSpec : public RefObject
{
public:

  KeyFrameSpec() {}

  virtual unsigned int GetNumberOfKeyFrames() const = 0;

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~KeyFrameSpec() {}
};


/**
 * The base template class for each key frame specialization. It stores a vector of
 * ProgressValue pairs in mPVs, and uses the existing interface for KeyFrameChannel
 * to point at this vector.
 * TODO: Incorporate KeyFrameChannel into this base template
 */
template<typename V>
class KeyFrameBaseSpec : public KeyFrameSpec
{
private:
  typedef ProgressValue<V> PV;
  typedef std::vector<PV> PVContainer;

  PVContainer                    mPVs;       // The ProgressValue pairs
  KeyFrameChannel<V>*            mKeyFrames; // The key frame interpolator

public:
  static KeyFrameBaseSpec<V>* New()
  {
    return new KeyFrameBaseSpec<V>();
  }

  static KeyFrameBaseSpec<V>* Clone(const KeyFrameBaseSpec<V>& keyFrames)
  {
    return new KeyFrameBaseSpec<V>(keyFrames);
  }

  /**
   * Constructor
   */
  KeyFrameBaseSpec<V>()
  {
    mKeyFrames = new KeyFrameChannel<V>(KeyFrameChannelBase::Translate, mPVs);
  }

protected:
  /**
   * Copy Constructor
   * Allow cloning of this object
   */
  KeyFrameBaseSpec<V>(const KeyFrameBaseSpec<V>& keyFrames)
  : mPVs(keyFrames.mPVs)
  {
    mKeyFrames = new KeyFrameChannel<V>(KeyFrameChannelBase::Translate, mPVs);
  }

  /**
   * Destructor. Ensure progress value pairs are cleared down
   */
  virtual ~KeyFrameBaseSpec<V>()
  {
    delete mKeyFrames;
    mPVs.clear();
  }

public:
  /**
   * Add a key frame to the progress value vector. Key frames should be added
   * in time order (this method does not sort the vector by time)
   * @param[in] t - progress
   * @param[in] v - value
   * @param[in] alpha - Alpha function for blending to the next keyframe
   */
  void AddKeyFrame(float t, V v, AlphaFunction alpha)
  {
    // TODO:: Support alpha
    mPVs.push_back(PV(t, v));
  }

  /**
   * Get the number of key frames
   * @return The size of the progress value vector
   */
  virtual unsigned int GetNumberOfKeyFrames() const
  {
    return mPVs.size();
  }

  /**
   * Get a key frame.
   * @param[in] index The index of the key frame to fetch
   * @param[out] time The progress of the given key frame
   * @param[out] value The value of the given key frame
   */
  virtual void GetKeyFrame(unsigned int index, float& time, V& value) const
  {
    DALI_ASSERT_ALWAYS( index < mPVs.size() && "KeyFrame index is out of bounds" );
    time  = mPVs[index].mProgress;
    value = mPVs[index].mValue;
  }

  /**
   * Return whether the progress is valid for the range of keyframes. (The first
   * keyframe doesn't have to start at 0, and the last doesn't have to end at 1.0)
   * @param[in] progress The progress to test
   * @return True if the progress is valid for this object
   */
  bool IsActive(float progress) const
  {
    return mKeyFrames->IsActive(progress);
  }

  /**
   * Return an interpolated value for the given progress.
   * @param[in] progress The progress to test
   * @return The interpolated value
   */
  V GetValue(float progress, Dali::Animation::Interpolation interpolation) const
  {
    return mKeyFrames->GetValue(progress, interpolation);
  }
};

typedef KeyFrameBaseSpec<float>      KeyFrameNumber;
typedef KeyFrameBaseSpec<bool>       KeyFrameBoolean;
typedef KeyFrameBaseSpec<int>        KeyFrameInteger;
typedef KeyFrameBaseSpec<Vector2>    KeyFrameVector2;
typedef KeyFrameBaseSpec<Vector3>    KeyFrameVector3;
typedef KeyFrameBaseSpec<Vector4>    KeyFrameVector4;
typedef KeyFrameBaseSpec<Quaternion> KeyFrameQuaternion;

typedef IntrusivePtr<KeyFrameBoolean>    KeyFrameBooleanPtr;
typedef IntrusivePtr<KeyFrameNumber>     KeyFrameNumberPtr;
typedef IntrusivePtr<KeyFrameInteger>    KeyFrameIntegerPtr;
typedef IntrusivePtr<KeyFrameVector2>    KeyFrameVector2Ptr;
typedef IntrusivePtr<KeyFrameVector3>    KeyFrameVector3Ptr;
typedef IntrusivePtr<KeyFrameVector4>    KeyFrameVector4Ptr;
typedef IntrusivePtr<KeyFrameQuaternion> KeyFrameQuaternionPtr;


inline void GetSpecialization(Internal::KeyFrames& keyFrames, Internal::KeyFrameBoolean*& keyFrameSpec)
{
  keyFrameSpec = static_cast<Internal::KeyFrameBoolean*>(keyFrames.GetKeyFramesBase());
}

inline void GetSpecialization(const Internal::KeyFrames& keyFrames, const Internal::KeyFrameBoolean*& keyFrameSpec)
{
  keyFrameSpec = static_cast<const Internal::KeyFrameBoolean*>(keyFrames.GetKeyFramesBase());
}

inline void GetSpecialization(Internal::KeyFrames& keyFrames, Internal::KeyFrameNumber*& keyFrameSpec)
{
  keyFrameSpec = static_cast<Internal::KeyFrameNumber*>(keyFrames.GetKeyFramesBase());
}

inline void GetSpecialization(const Internal::KeyFrames& keyFrames, const Internal::KeyFrameNumber*& keyFrameSpec)
{
  keyFrameSpec = static_cast<const Internal::KeyFrameNumber*>(keyFrames.GetKeyFramesBase());
}

inline void GetSpecialization(Internal::KeyFrames& keyFrames, Internal::KeyFrameInteger*& keyFrameSpec)
{
  keyFrameSpec = static_cast<Internal::KeyFrameInteger*>(keyFrames.GetKeyFramesBase());
}

inline void GetSpecialization(const Internal::KeyFrames& keyFrames, const Internal::KeyFrameInteger*& keyFrameSpec)
{
  keyFrameSpec = static_cast<const Internal::KeyFrameInteger*>(keyFrames.GetKeyFramesBase());
}

inline void GetSpecialization(Internal::KeyFrames& keyFrames, Internal::KeyFrameVector2*& keyFrameSpec)
{
  keyFrameSpec = static_cast<Internal::KeyFrameVector2*>(keyFrames.GetKeyFramesBase());
}

inline void GetSpecialization(const Internal::KeyFrames& keyFrames, const Internal::KeyFrameVector2*& keyFrameSpec)
{
  keyFrameSpec = static_cast<const Internal::KeyFrameVector2*>(keyFrames.GetKeyFramesBase());
}

inline void GetSpecialization(Internal::KeyFrames& keyFrames, Internal::KeyFrameVector3*& keyFrameSpec)
{
  keyFrameSpec = static_cast<Internal::KeyFrameVector3*>(keyFrames.GetKeyFramesBase());
}

inline void GetSpecialization(const Internal::KeyFrames& keyFrames, const Internal::KeyFrameVector3*& keyFrameSpec)
{
  keyFrameSpec = static_cast<const Internal::KeyFrameVector3*>(keyFrames.GetKeyFramesBase());
}

inline void GetSpecialization(Internal::KeyFrames& keyFrames, Internal::KeyFrameVector4*& keyFrameSpec)
{
  keyFrameSpec = static_cast<Internal::KeyFrameVector4*>(keyFrames.GetKeyFramesBase());
}

inline void GetSpecialization(const Internal::KeyFrames& keyFrames, const Internal::KeyFrameVector4*& keyFrameSpec)
{
  keyFrameSpec = static_cast<const Internal::KeyFrameVector4*>(keyFrames.GetKeyFramesBase());
}

inline void GetSpecialization(Internal::KeyFrames& keyFrames, Internal::KeyFrameQuaternion*& keyFrameSpec)
{
  keyFrameSpec = static_cast<Internal::KeyFrameQuaternion*>(keyFrames.GetKeyFramesBase());
}

inline void GetSpecialization(const Internal::KeyFrames& keyFrames, const Internal::KeyFrameQuaternion*& keyFrameSpec)
{
  keyFrameSpec = static_cast<const Internal::KeyFrameQuaternion*>(keyFrames.GetKeyFramesBase());
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

#endif //__DALI_INTERNAL_KEY_FRAMES_H__
