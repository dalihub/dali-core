#ifndef DALI_INTERNAL_SAMPLER_H
#define DALI_INTERNAL_SAMPLER_H

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

// INTERNAL INCLUDES
#include <dali/public-api/actors/sampling.h>
#include <dali/public-api/common/dali-common.h> // DALI_ASSERT_ALWAYS
#include <dali/public-api/common/intrusive-ptr.h> // Dali::IntrusivePtr
#include <dali/public-api/rendering/sampler.h> // Dali::Sampler
#include <dali/internal/event/common/connectable.h> // Dali::Internal::Connectable
#include <dali/internal/event/common/object-connector.h> // Dali::Internal::ObjectConnector
#include <dali/internal/event/common/object-impl.h> // Dali::Internal::Object

namespace Dali
{
namespace Internal
{
namespace Render
{
class Sampler;
}

class Sampler;
typedef IntrusivePtr<Sampler> SamplerPtr;

/**
 * Sampler is an object that contains an array of structures of values that
 * can be accessed as properties.
 */
class Sampler : public BaseObject
{
public:

  /**
   * Create a new Sampler.
   * @return A smart-pointer to the newly allocated Sampler.
   */
  static SamplerPtr New( );

  /**
   * @copydoc Dali::Sampler::SetFilterMode()
   */
  void SetFilterMode( Dali::FilterMode::Type minFilter, Dali::FilterMode::Type magFilter );

  /**
   * @copydoc Dali::Sampler::SetWrapMode()
   */
  void SetWrapMode( Dali::WrapMode::Type rWrap, Dali::WrapMode::Type sWrap, Dali::WrapMode::Type tWrap );

  /**
   * Get the render thread sampler
   *
   * @return The render thread sampler
   */
  Render::Sampler* GetSamplerRenderObject();

private:
  Sampler();

  /**
   * Second stage initialization
   */
  void Initialize( );

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Sampler();

private: // data
  EventThreadServices& mEventThreadServices;    ///<Used to send messages to the render thread via the update thread
  Render::Sampler* mRenderObject;               ///<Render thread sampler for this sampler

};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Internal::Sampler& GetImplementation(Dali::Sampler& handle)
{
  DALI_ASSERT_ALWAYS(handle && "Sampler handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::Sampler&>(object);
}

inline const Internal::Sampler& GetImplementation(const Dali::Sampler& handle)
{
  DALI_ASSERT_ALWAYS(handle && "Sampler handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::Sampler&>(object);
}

} // namespace Dali

#endif // DALI_INTERNAL_SAMPLER_H
