#ifndef __DALI_INTERNAL_LIGHT_ATTACHMENT_H__
#define __DALI_INTERNAL_LIGHT_ATTACHMENT_H__

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

// INTERNAL INCLUDES
#include <dali/internal/event/actor-attachments/actor-attachment-impl.h>
#include <dali/internal/event/actor-attachments/actor-attachment-declarations.h>
#include <dali/internal/event/modeling/light-impl.h>

namespace Dali
{

namespace Internal
{

class Light;

namespace SceneGraph
{
class LightAttachment;
class Node;
}

/**
 * An attachment for managing the properties of a light in the scene
 */
class LightAttachment : public ActorAttachment
{
public:
  /**
   * Create an initialised LightAttachment.
   * @return A handle to a newly allocated Dali resource.
   */
  static LightAttachmentPtr New( const SceneGraph::Node& parentNode );

  /**
   * Set the local properties of the light
   * The light properties are copied into this attachment
   * @param[in] light The light properties
   */
  void SetLight(Internal::LightPtr light);

  /**
   * Get the local properties of the light
   * @return A copy of the light properties
   */
  Internal::LightPtr GetLight() const;

  /**
   * @copydoc Dali::Light::SetName
   */
  void SetName(const std::string& name);

  /**
   * @copydoc Dali::Light::GetName
   */
  const std::string& GetName() const;

  /**
   * @copydoc Dali::Light::SetType
   */
  void SetType(Dali::LightType type);

  /**
   * @copydoc Dali::Light::GetType
   */
  Dali::LightType GetType() const;

  /**
   * @copydoc Dali::Light::SetFallOff
   */
  void SetFallOff(const Vector2& fallOff);

  /**
   * @copydoc Dali::Light::GetFallOff
   */
  const Vector2& GetFallOff() const;

  /**
   * @copydoc Dali::Light::SetSpotAngle
   */
  void SetSpotAngle(const Vector2& angle);

  /**
   * @copydoc Dali::Light::GetSpotAngle
   */
  const Vector2& GetSpotAngle() const;

  /**
   * @copydoc Dali::Light::SetAmbientColor
   */
  void SetAmbientColor(const Vector3& color);

  /**
   * @copydoc Dali::Light::GetAmbientColor
   */
  const Vector3& GetAmbientColor() const;

  /**
   * @copydoc Dali::Light::SetDiffuseColor
   */
  void SetDiffuseColor(const Vector3& color);

  /**
   * @copydoc Dali::Light::GetDiffuseColor
   */
  const Vector3& GetDiffuseColor() const;

  /**
   * @copydoc Dali::Light::SetSpecularColor
   */
  void SetSpecularColor(const Vector3& color);

  /**
   * @copydoc Dali::Light::GetSpecularColor
   */
  const Vector3& GetSpecularColor() const;

  /**
   * @copydoc Dali::Light::SetDirection
   */
  void SetDirection(const Vector3& direction);

  /**
   * @copydoc Dali::Light::GetDirection
   */
  const Vector3& GetDirection() const;

  /**
   * @copydoc Dali::LightActor::SetActive
   */
  void SetActive(bool active);

private:

  /**
   * First stage construction of a LightAttachment.
   * @param[in] stage Used to send messages to scene-graph.
   */
  LightAttachment( Stage& stage );

  /**
   * @copydoc Dali::Internal::ActorAttachment::OnStageConnection()
   */
  virtual void OnStageConnection();

  /**
   * @copydoc Dali::Internal::ActorAttachment::OnStageDisconnection()
   */
  virtual void OnStageDisconnection();

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~LightAttachment();

private:

  const SceneGraph::LightAttachment* mSceneObject; ///< Not owned

  Internal::LightPtr mCachedLight;              ///< A local cache of the light properties
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_LIGHT_ATTACHMENT_H__
