#ifndef __DALI_CAMERA_ACTOR_H__
#define __DALI_CAMERA_ACTOR_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/actors/actor.h>

namespace Dali
{
/**
 * @addtogroup dali_core_actors
 * @{
 */

namespace Internal DALI_INTERNAL
{
class CameraActor;
}

namespace Camera
{
/**
 * @brief Type determines how camera operates.
 */
enum Type
{
  FREE_LOOK,      ///< Camera orientation is taken from CameraActor
  LOOK_AT_TARGET, ///< Camera is oriented to always look at a target
};

/**
 * @brief Projection modes.
 */
enum ProjectionMode
{
  PERSPECTIVE_PROJECTION,    ///< Distance causes foreshortening; objects further from the camera appear smaller
  ORTHOGRAPHIC_PROJECTION,    ///< Relative distance from the camera does not affect the size of objects
};

} // namespace Camera

/**
 * @brief Controls a camera.
 *
 * Allows the developer to use actor semantics to control a camera.
 *
 * There are two types of camera actor, FREE_LOOK and LOOK_AT_TARGET. By default
 * the camera actor will be FREE_LOOK.
 *
 * A FREE_LOOK camera uses actor's orientation to control where the camera is looking.
 * If no additional rotations are specified, the camera looks in the negative Z direction.
 *
 * For LOOK_AT_TARGET the actor's orientation is ignored, instead the camera looks at TARGET_POSITION
 * in world coordinates.
 *
 */
class DALI_IMPORT_API CameraActor : public Actor
{
public:

  /**
   * @brief An enumeration of properties belonging to the CameraActor class.
   * Properties additional to Actor.
   */
  struct Property
  {
    enum
    {
      TYPE = DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX, ///< name "type",                  type std::string
      PROJECTION_MODE,                                   ///< name "projectionMode",        type std::string
      FIELD_OF_VIEW,                                     ///< name "fieldOfView",           type float
      ASPECT_RATIO,                                      ///< name "aspectRatio",           type float
      NEAR_PLANE_DISTANCE,                               ///< name "nearPlaneDistance",     type float
      FAR_PLANE_DISTANCE,                                ///< name "farPlaneDistance",      type float
      LEFT_PLANE_DISTANCE,                               ///< name "leftPlaneDistance",     type float
      RIGHT_PLANE_DISTANCE,                              ///< name "rightPlaneDistance",    type float
      TOP_PLANE_DISTANCE,                                ///< name "topPlaneDistance",      type float
      BOTTOM_PLANE_DISTANCE,                             ///< name "bottomPlaneDistance",   type float
      TARGET_POSITION,                                   ///< name "targetPosition",        type Vector3
      PROJECTION_MATRIX,                                 ///< name "projectionMatrix",      type Matrix
      VIEW_MATRIX,                                       ///< name "viewMatrix",            type Matrix
      INVERT_Y_AXIS,                                     ///< name "invertYAxis",           type bool
    };
  };

  /**
   * @brief Create an uninitialized CameraActor handle.
   *
   * Initialise it using CameraActor::New().  Calling member functions
   * with an uninitialized Dali::Object is not allowed.
   */
  CameraActor();

  /**
   * @brief Create a CameraActor object.
   *
   * Sets the default camera perspective projection for the stage's size. @see SetPerspectiveProjection().
   * @return the newly created camera actor.
   */
  static CameraActor New();

  /**
   * @brief Create a CameraActor object.
   *
   * Sets the default camera perspective projection for the given canvas size. @see SetPerspectiveProjection().
   *
   * @param[in] size The canvas size.
   * @return the newly created camera actor.
   */
  static CameraActor New( const Size& size );

  /**
   * @brief Downcast an Object handle to CameraActor.
   *
   * If handle points to a CameraActor the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a CameraActor or an uninitialized handle
   */
  static CameraActor DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~CameraActor();

  /**
   * @brief Copy constructor
   *
   * @param [in] copy The actor to copy.
   */
  CameraActor(const CameraActor& copy);

  /**
   * @brief Assignment operator
   *
   * @param [in] rhs The actor to copy.
   */
  CameraActor& operator=(const CameraActor& rhs);

  /**
   * @brief Set the camera type.
   * The default type is Dali::Camera::FREE_LOOK
   * @param[in] type The camera type
   */
  void SetType( Dali::Camera::Type type );

  /**
   * @brief Get the type of the camera.
   *
   * @return the type of camera
   */
  Dali::Camera::Type GetType() const;

  /**
   * @brief Set the projection mode.
   *
   * @param[in] mode One of PERSPECTIVE_PROJECTION or ORTHOGRAPHIC_PROJECTION
   */
  void SetProjectionMode( Dali::Camera::ProjectionMode mode );

  /**
   * @brief Get the projection mode.
   *
   * @return One of PERSPECTIVE_PROJECTION or ORTHOGRAPHIC_PROJECTION
   */
  Dali::Camera::ProjectionMode GetProjectionMode() const;

  /**
   * @brief Set the field of view.
   *
   * @param[in] fieldOfView The field of view in radians
   */
  void SetFieldOfView( float fieldOfView );

  /**
   * @brief Get the field of view in Radians.
   *
   * The default field of view is 45 degrees
   * @return The field of view in radians
   */
  float GetFieldOfView( );

  /**
   * @brief Set the aspect ratio.
   *
   * @param[in] aspectRatio The aspect ratio
   */
  void SetAspectRatio( float aspectRatio );

  /**
   * @brief Get the aspect ratio of the camera.
   *
   * The default aspect ratio is 4.0f/3.0f
   * @return the aspect ratio
   */
  float GetAspectRatio( );

  /**
   * @brief Sets the near clipping plane distance.
   *
   * @param[in] nearClippingPlane distance of the near clipping plane
   */
  void SetNearClippingPlane( float nearClippingPlane );

  /**
   * @brief Get the near clipping plane distance.
   *
   * The default near clipping plane is 800.0f, to match the default screen height
   * Reduce this value to see objects closer to the camera
   * @return the near clipping plane value
   */
  float GetNearClippingPlane( );

  /**
   * @brief Sets the far clipping plane distance.
   *
   * @param[in] farClippingPlane distance of the far clipping plane
   */
  void SetFarClippingPlane( float farClippingPlane );

  /**
   * @brief Get the far clipping plane distance.
   *
   * The default value is the default near clipping plane + (0xFFFF>>4)
   * @return the far clipping plane value
   */
  float GetFarClippingPlane( );

  /**
   * @brief Set the target position of the camera.
   *
   * @pre Camera type is LOOK_AT_TARGET
   * @param[in] targetPosition The position of the target to look at
   */
  void SetTargetPosition( const Vector3& targetPosition );

  /**
   * @brief Get Camera Target position.
   *
   * The target position is Vector3::ZERO
   * @pre Camera type is LOOK_AT_TARGET
   * @return The target position of the camera
   */
  Vector3 GetTargetPosition() const;

  /**
   * @brief Request for an inversion on the Y axis on the projection calculation.
   *
   * The default value is not inverted.
   * @param[in] invertYAxis True if the Y axis should be inverted
   */
  void SetInvertYAxis(bool invertYAxis);

  /**
   * @brief Get whether the Y axis is inverted.
   *
   * @return True if the Y axis is inverted, false otherwise
   */
  bool GetInvertYAxis();

  /**
   * @brief Sets the default camera perspective projection for the given canvas size.
   *
   * Sets the near and far clipping planes, the field of view, the aspect ratio
   * and the Z position of the actor based on the canvas size so that 1 unit in
   * XY (z=0) plane is 1 pixel on screen.
   *
   * If the canvas size is ZERO, it sets the default camera perspective
   * projection for the stage's size.
   *
   * @pre If size is non ZERO, \e width and \e height must be greater than zero.
   *
   * @param[in] size The canvas size.
   */
  void SetPerspectiveProjection( const Size& size );

  /**
   * @brief Sets the camera projection to use orthographic projection.
   *
   * The XY plane is centered on the camera axis. The units in the X/Y
   * plane directly equate to pixels on an equivalently sized
   * framebuffer.
   *
   * The Z position of the actor, and the near and far clip planes of the
   * bounding box match those that would be created by using
   * SetPerspectiveProjection with the same size.
   *
   * @param[in] size Size of XY plane (normal to camera axis)
   */
  void SetOrthographicProjection( const Size& size );

  /**
   * @brief Sets the camera projection to use orthographic projection with the given clip planes.
   *
   * This does not change the Z value of the camera actor.
   *
   * @param[in] left Distance to left clip plane (normal to camera axis)
   * @param[in] right Distance to right clip plane (normal to camera axis)
   * @param[in] top Distance to top clip plane (normal to camera axis)
   * @param[in] bottom Distance to bottom clip plane (normal to camera axis)
   * @param[in] near Distance to the near clip plane (along camera axis)
   * @param[in] far Distance to the far clip plane (along camera axis)
   */
  void SetOrthographicProjection( float left, float right, float top, float bottom, float near, float far );

public: // Not intended for use by Application developers
  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in] actor A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL CameraActor(Internal::CameraActor* actor);
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_CAMERA_ACTOR_H__
