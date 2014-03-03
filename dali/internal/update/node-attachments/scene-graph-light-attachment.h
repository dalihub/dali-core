#ifndef __DALI_INTERNAL_SCENE_GRAPH_LIGHT_ATTACHMENT_H__
#define __DALI_INTERNAL_SCENE_GRAPH_LIGHT_ATTACHMENT_H__

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
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector2.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/event-to-update.h>
#include <dali/internal/event/modeling/light-impl.h>
#include <dali/internal/update/node-attachments/node-attachment.h>
#include <dali/internal/update/controllers/light-controller.h>

namespace Dali
{

namespace Internal
{

// value types used by messages
template <> struct ParameterType< LightType > : public BasicType< LightType > {};

namespace SceneGraph
{

class RenderableAttachment;

/**
 * An attachment for light objects and their properties.
 */
class LightAttachment : public NodeAttachment
{
public:
  /**
   * Construct a new LightAttachment.
   * @return a new LightAttachment.
   */
  static LightAttachment* New()
  {
    return new LightAttachment();
  }

  /**
   * @copydoc NodeAttachment::ConnectToSceneGraph().
   */
  virtual void ConnectToSceneGraph( SceneController& sceneController, BufferIndex updateBufferIndex );

  /**
   * @copydoc NodeAttachment::DisconnectFromSceneGraph().
   */
  virtual void OnDestroy();

  /**
   * Virtual destructor
   */
  virtual ~LightAttachment();

  /**
   * @copydoc NodeAttachment::GetRenderable.
   * @return Always NULL.
   */
  virtual RenderableAttachment* GetRenderable()
  {
    return NULL;
  }

  /**
   * @copydoc Dali::Internal::LightAttachment::SetLight
   */
  void SetLight(Internal::LightPtr light)
  {
    mLight = new Internal::Light(*light.Get());
    mUpdateRequired = true;
  }

  /**
   * @copydoc Dali::Internal::LightAttachment::GetLight
   */
  Internal::LightPtr GetLight() const
  {
    return mLight;
  }

  /**
   * @copydoc Dali::Light::SetName
   */
  void SetName(const std::string& name)
  {
    mLight->SetName(name);
  }
  /**
   * @copydoc Dali::Light::GetName
   */
  const std::string& GetName() const
  {
    return mLight->GetName();
  }

  /**
   * @copydoc Dali::Light::SetType
   */
  void SetType(Dali::LightType type)
  {
    if (mLight->GetType() != type)
    {
      mLight->SetType(type);
      mUpdateRequired = true;
    }
  }

  /**
   * @copydoc Dali::Light::GetType
   */
  Dali::LightType GetType() const
  {
    return mLight->GetType();
  }

  /**
   * @copydoc Dali::Light::SetFallOff
   */
  void SetFallOff(const Vector2& fallOff)
  {
    if (mLight->GetFallOff() != fallOff)
    {
      mLight->SetFallOff(fallOff);
      mUpdateRequired = true;
    }
  }

  /**
   * @copydoc Dali::Light::GetFallOff
   */
  const Vector2& GetFallOff() const
  {
    return mLight->GetFallOff();
  }

  /**
   * @copydoc Dali::Light::SetSpotAngle
   */
  void SetSpotAngle(const Vector2& angle)
  {
    if (mLight->GetSpotAngle() != angle)
    {
      mLight->SetSpotAngle(angle);
      mUpdateRequired = true;
    }
  }

  /**
   * @copydoc Dali::Light::GetSpotAngle
   */
  const Vector2& GetSpotAngle() const
  {
    return mLight->GetSpotAngle();
  }

  /**
   * @copydoc Dali::Light::SetAmbientColor
   */
  void SetAmbientColor(const Vector3& color)
  {
    if (mLight->GetAmbientColor() != color)
    {
      mLight->SetAmbientColor(color);
      mUpdateRequired = true;
    }
  }

  /**
   * @copydoc Dali::Light::GetAmbientColor
   */
  const Vector3& GetAmbientColor() const
  {
    return mLight->GetAmbientColor();
  }

  /**
   * @copydoc Dali::Light::SetDiffuseColor
   */
  void SetDiffuseColor(const Vector3& color)
  {
    if (mLight->GetDiffuseColor() != color)
    {
      mLight->SetDiffuseColor(color);
      mUpdateRequired = true;
    }
  }

  /**
   * @copydoc Dali::Light::GetDiffuseColor
   */
  const Vector3& GetDiffuseColor() const
  {
    return mLight->GetDiffuseColor();
  }

  /**
   * @copydoc Dali::Light::SetSpecularColor
   */
  void SetSpecularColor(const Vector3& color)
  {
    if (mLight->GetSpecularColor() != color)
    {
      mLight->SetSpecularColor(color);
      mUpdateRequired = true;
    }
  }

  /**
   * @copydoc Dali::Light::GetSpecularColor
   */
  const Vector3& GetSpecularColor() const
  {
    return mLight->GetSpecularColor();
  }

  /**
   * @copydoc Dali::Light::SetDirection
   */
  void SetDirection(const Vector3& direction)
  {
    if (mLight->GetDirection() != direction)
    {
      mLight->SetDirection(direction);
      mUpdateRequired = true;
    }
  }

  /**
   * @copydoc Dali::Light::GetDirection
   */
  const Vector3& GetDirection() const
  {
    return mLight->GetDirection();
  }


  /**
   * @copydoc Dali::LightActor::SetActive
   */
  void SetActive(bool active)
  {
    Node& lightNode(GetParent());
    if (active)
    {
      mLightController->AddLight(lightNode);        // Add to list of active lights
    }
    else
    {
      mLightController->RemoveLight(lightNode);     // Remove from list of active lights
    }
  }

protected:

  /**
   * Protected constructor; see also LightAttachment::New().
   */
  LightAttachment();

private:

  // Undefined
  LightAttachment(const LightAttachment&);

  // Undefined
  LightAttachment& operator=(const LightAttachment& rhs);

  void Update()
  {
    if (mUpdateRequired)
    {
      mUpdateRequired = false;
    }
  }

private:
  Internal::LightPtr mLight;
  bool               mUpdateRequired;                ///< This is set to true if the projection matrix requires an update
  LightController*   mLightController;               ///< required to add / remove lights
};

// Messages for LightAttachment

inline void SetLightMessage( EventToUpdate& eventToUpdate, const LightAttachment& attachment, const Internal::LightPtr& parameter )
{
  typedef MessageValue1< LightAttachment, Internal::LightPtr > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &LightAttachment::SetLight, parameter );
}

inline void SetNameMessage( EventToUpdate& eventToUpdate, const LightAttachment& attachment, const std::string& parameter )
{
  typedef MessageValue1< LightAttachment, std::string > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &LightAttachment::SetName, parameter );
}

inline void SetTypeMessage( EventToUpdate& eventToUpdate, const LightAttachment& attachment, LightType parameter )
{
  typedef MessageValue1< LightAttachment, LightType > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &LightAttachment::SetType, parameter );
}

inline void SetFallOffMessage( EventToUpdate& eventToUpdate, const LightAttachment& attachment, const Vector2 &parameter )
{
  typedef MessageValue1< LightAttachment, Vector2 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &LightAttachment::SetFallOff, parameter );
}

inline void SetSpotAngleMessage( EventToUpdate& eventToUpdate, const LightAttachment& attachment, const Vector2 &parameter )
{
  typedef MessageValue1< LightAttachment, Vector2 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &LightAttachment::SetSpotAngle, parameter );
}

inline void SetAmbientColorMessage( EventToUpdate& eventToUpdate, const LightAttachment& attachment, const Vector3 &parameter )
{
  typedef MessageValue1< LightAttachment, Vector3 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &LightAttachment::SetAmbientColor, parameter );
}

inline void SetDiffuseColorMessage( EventToUpdate& eventToUpdate, const LightAttachment& attachment, const Vector3 &parameter )
{
  typedef MessageValue1< LightAttachment, Vector3 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &LightAttachment::SetDiffuseColor, parameter );
}

inline void SetSpecularColorMessage( EventToUpdate& eventToUpdate, const LightAttachment& attachment, const Vector3 &parameter )
{
  typedef MessageValue1< LightAttachment, Vector3 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &LightAttachment::SetSpecularColor, parameter );
}

inline void SetDirectionMessage( EventToUpdate& eventToUpdate, const LightAttachment& attachment, const Vector3 &parameter )
{
  typedef MessageValue1< LightAttachment, Vector3 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &LightAttachment::SetDirection, parameter );
}

inline void SetActiveMessage( EventToUpdate& eventToUpdate, const LightAttachment& attachment, bool parameter )
{
  typedef MessageValue1< LightAttachment, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &LightAttachment::SetActive, parameter );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_LIGHT_ATTACHMENT_H__
