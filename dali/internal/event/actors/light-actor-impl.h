#ifndef __DALI_INTERNAL_LIGHT_ACTOR_H__
#define __DALI_INTERNAL_LIGHT_ACTOR_H__

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
#include <dali/public-api/actors/light-actor.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actor-attachments/light-attachment-impl.h>

namespace Dali
{

namespace Internal
{

/**
 * An actor with a conveniently pre-attached LightAttachment.
 * This is the simplest way to add lighting.
 */
class LightActor : public Actor
{
public:

  /**
   * Create an initialised light actor.
   * @return A smart-pointer to a newly allocated light actor.
   */
  static LightActorPtr New();

  /**
   * @copydoc Dali::Internal::Actor::OnInitialize
   */
  void OnInitialize();

  /**
   * @copydoc Dali::LightActor::SetLight
   */
  void SetLight(Dali::Light light);

  /**
   * @copydoc Dali::LightActor::GetLight
   */
  Dali::Light GetLight() const;

  /**
   * @copydoc Dali::LightActor::SetActive
   */
  void SetActive(bool active);

  /**
   * @copydoc Dali::LightActor::GetActive
   */
  bool GetActive();

public: // Default property extensions from Object
  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyCount()
   */
  virtual unsigned int GetDefaultPropertyCount() const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyIndices()
   */
  virtual void GetDefaultPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyName()
   */
  virtual const char* GetDefaultPropertyName(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyIndex()
   */
  virtual Property::Index GetDefaultPropertyIndex(const std::string& name) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyWritable()
   */
  virtual bool IsDefaultPropertyWritable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyAnimatable()
   */
  virtual bool IsDefaultPropertyAnimatable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyAConstraintInput()
   */
  virtual bool IsDefaultPropertyAConstraintInput( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyType()
   */
  virtual Property::Type GetDefaultPropertyType(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::SetDefaultProperty()
   */
  virtual void SetDefaultProperty(Property::Index index, const Property::Value& propertyValue);

  /**
   * @copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

protected:

  /**
   * Protected constructor; see also LightActor::New()
   */
  LightActor();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~LightActor();

private:

  LightAttachmentPtr mLightAttachment;
  bool mIsActive;

};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::LightActor& GetImplementation(Dali::LightActor& light)
{
  DALI_ASSERT_ALWAYS( light && "LightActor handle is empty" );

  BaseObject& handle = light.GetBaseObject();

  return static_cast<Internal::LightActor&>(handle);
}

inline const Internal::LightActor& GetImplementation(const Dali::LightActor& light)
{
  DALI_ASSERT_ALWAYS( light && "LightActor handle is empty" );

  const BaseObject& handle = light.GetBaseObject();

  return static_cast<const Internal::LightActor&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_LIGHT_ACTOR_H__
