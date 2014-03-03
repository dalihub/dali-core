#ifndef __DALI_LIGHT_H__
#define __DALI_LIGHT_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class Light;
}

class Light;

/**
 * Light source types
 */
enum LightType
{
  AMBIENT,        ///< Lights everything evenly
  DIRECTIONAL,    ///< Casts light evenly in a specific direction.
  SPOT,           ///< Casts light from a single point in the shape of a cone. Objects outside the cone are unlit. (EG: a search light)
  POINT           ///< Casts light outward from a single point in all directions. (EG: a light bulb)
};

/**
 * Encapsulates the data describing a light source
 */
class Light : public BaseHandle
{
public:
  /**
   * Create an initialized Light.
   * @param[in] name The light's name
   * @return A handle to a newly allocated Dali resource.
   */
  static Light New(const std::string& name);

  /**
   * Downcast an Object handle to Light handle. If handle points to a Light object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a Light object or an uninitialized handle
   */
  static Light DownCast( BaseHandle handle );

  /**
   * Create an uninitialized light; this can be initialized with Light::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  Light()
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~Light();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

  /**
   * Set the light's name
   * @param[in] name The light's name
   */
  void SetName(const std::string& name);

  /**
   * Get the light's name
   * @return The light's name
   */
  const std::string& GetName() const;

  /**
   * Set the light's type
   * @param[in] type The type of light
   */
  void SetType(LightType type);

  /**
   * Get the lights type.
   * @return The light's type
   */
  LightType GetType() const;

  /**
   * Set the distances at which the light's intensity starts to fall off and reaches zero
   * @param[in] fallOff The fall off start and end. The start is in the x component and end is in the y component.
   */
  void SetFallOff(const Vector2& fallOff);

  /**
   * Get the distances at which the light's intensity starts to fall off and reaches zero.
   * @return The distances at which the light's intensity starts to fall off, and reaches zero.
   * See @ref SetFallOff
   */
  const Vector2& GetFallOff() const;

  /**
   * Set the spotlight's inner and outer cone angles
   * @param[in] angle The spotlight's inner and outer cone angles.
   *                  The inner is in the x component and outer is in the y component.
   */
  void SetSpotAngle(const Vector2& angle);

  /**
   * Get the spotlight's inner and outer cone angles
   * @return The spotlight's inner and outer cone angles
   */
  const Vector2& GetSpotAngle() const;

  /**
   * Set the ambient color for the light
   * The color is composed of rgb
   * @param[in] color The color to set.
   */
  void SetAmbientColor(const Vector3& color);

  /**
   * Get the light's ambient color
   * @return the light's color as rgb
   */
  const Vector3& GetAmbientColor() const;

  /**
   * Set the diffuse color for the light
   * The color is composed of rgb
   * @param[in] color The color to set.
   */
  void SetDiffuseColor(const Vector3& color);

  /**
   * Get the light's ambient color
   * @return the light's color as rgb
   */
  const Vector3& GetDiffuseColor() const;

  /**
   * Set the specular color for the light
   * The color is composed of rgb
   * @param[in] color The color to set.
   */
  void SetSpecularColor(const Vector3& color);

  /**
   * Get the light's specular color
   * @return the light's specular color as rgb
   */
  const Vector3& GetSpecularColor() const;

  /**
   * Set the direction in which the light's rays are cast.
   * Valid when the light's type is DIRECTIONAL (see @ref Dali::LightType).
   * @param [in] direction The direction in which the light's rays are cast.
   */
  void SetDirection(const Vector3& direction);

  /**
   * Get the direction in which the light's rays are cast.
   * Valid when the light's type is DIRECTIONAL (see @ref LightType).
   * @return The direction in which the light's rays are cast.
   */
  const Vector3& GetDirection() const;

public: // Not intended for application developers

  /**
   * This constructor is used by Dali New() methods
   * @param [in] light A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Light(Internal::Light* light);
};

} // namespace Dali

#endif // __DALI_LIGHT_H__
