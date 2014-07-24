#ifndef __DALI_INTERNAL_LIGHT_H__
#define __DALI_INTERNAL_LIGHT_H__

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/common/light.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/base-object.h>

namespace Dali
{

namespace Internal
{

class Light;
typedef IntrusivePtr<Light>    LightPtr;         ///< Smart pointer to Internal::Light object
typedef std::vector<LightPtr>          LightContainer;
typedef LightContainer::iterator       LightIter;
typedef LightContainer::const_iterator LightConstIter;

/**
 * @copydoc Dali::Light
 */
class Light : public BaseObject
{
public:
  /**
   * Constructor
   */
  Light(const std::string& name);

  /**
   * Copy constructor
   */
  Light(const Light& rhs);

  /**
   * Assignment operator
   */
  Light& operator=(const Light& rhs);

private:
  /**
   * Destructor - A reference counted object may only be deleted by calling Unreference()
   */
  ~Light();

public:
  /**
   * @copydoc Dali::Light::SetName
   */
  void SetName(const std::string& name)
  {
    mName = name;
  }

  /**
   * @copydoc Dali::Light::GetName
   */
  const std::string& GetName() const
  {
    return mName;
  }

  /**
   * @copydoc Dali::Light::SetType
   */
  void SetType(Dali::LightType type)
  {
    mType = type;
  }

  /**
   * @copydoc Dali::Light::GetType
   */
  Dali::LightType GetType() const
  {
    return mType;
  }

  /**
   * @copydoc Dali::Light::SetFallOff
   */
  void SetFallOff(const Vector2& fallOff)
  {
    mFallOff = fallOff;
  }

  /**
   * @copydoc Dali::Light::GetFallOff
   */
  const Vector2& GetFallOff() const
  {
    return mFallOff;
  }

  /**
   * @copydoc Dali::Light::SetSpotAngle
   */
  void SetSpotAngle(const Vector2& angle)
  {
    mSpotAngle = angle;
  }

  /**
   * @copydoc Dali::Light::GetSpotAngle
   */
  const Vector2& GetSpotAngle() const
  {
    return mSpotAngle;
  }
  /**
   * @copydoc Dali::Light::SetAmbientColor
   */
  void SetAmbientColor(const Vector3& color)
  {
    mAmbientColor = color;
  }

  /**
   * @copydoc Dali::Light::GetAmbientColor
   */
  const Vector3& GetAmbientColor() const
  {
    return mAmbientColor;
  }

  /**
   * @copydoc Dali::Light::SetDiffuseColor
   */
  void SetDiffuseColor(const Vector3& color)
  {
    mDiffuseColor = color;
  }

  /**
   * @copydoc Dali::Light::GetDiffuseColor
   */
  const Vector3& GetDiffuseColor() const
  {
    return mDiffuseColor;
  }

  /**
   * @copydoc Dali::Light::SetSpecularColor
   */
  void SetSpecularColor(const Vector3& color)
  {
    mSpecularColor = color;
  }

  /**
   * @copydoc Dali::Light::GetSpecularColor
   */
  const Vector3& GetSpecularColor() const
  {
    return mSpecularColor;
  }

  /**
   * @copydoc Dali::Light::SetDirection
   */
  void SetDirection(const Vector3& direction)
  {
    mDirection = direction;
  }

  /**
   * @copydoc Dali::Light::GetDirection
   */
  const Vector3& GetDirection() const
  {
    return mDirection;
  }

  // attributes
private:
  std::string mName;                          ///< Name of light
  LightType   mType;                          ///< Type of light
  Vector2     mFallOff;                       ///< The distance at which the light intensity starts to fall and the distance until it reaches zero
  Vector2     mSpotAngle;                     ///< The angle at which the spotlight's light starts to fall off and the angle at which it reaches zero
  Vector3     mAmbientColor;                  ///< The light's ambient color
  Vector3     mDiffuseColor;                  ///< The light's diffuse color
  Vector3     mSpecularColor;                 ///< The light's specular color
  Vector3     mDirection;                     ///< The light's direction (for LightType::DIRECTIONAL lights)
}; // class Light

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::Light& GetImplementation(Dali::Light& object)
{
  DALI_ASSERT_ALWAYS( object && "Light handle is empty" );

  BaseObject& handle = object.GetBaseObject();

  return static_cast<Internal::Light&>(handle);
}

inline const Internal::Light& GetImplementation(const Dali::Light& object)
{
  DALI_ASSERT_ALWAYS( object && "Light handle is empty" );

  const BaseObject& handle = object.GetBaseObject();

  return static_cast<const Internal::Light&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_LIGHT_H__
