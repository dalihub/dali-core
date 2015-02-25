#ifndef __DALI_INTERNAL_MODEL_H__
#define __DALI_INTERNAL_MODEL_H__

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
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/modeling/material.h>
#include <dali/public-api/modeling/model.h>
#include <dali/public-api/object/base-object.h>
#include <dali/internal/event/modeling/model-data-impl.h>
#include <dali/internal/event/resources/resource-ticket.h>
#include <dali/internal/event/resources/resource-ticket-observer.h>
#include <dali/internal/event/resources/resource-client.h>

namespace Dali
{

namespace Internal
{
class ModelData;
class Model;
class ResourceClient;
typedef IntrusivePtr<Model> ModelPtr;

/**
 * Encapsulates a Dali 3D model/scene.
 */
class Model : public BaseObject, public ResourceTicketObserver
{
public:
  /**
   * Create a model from the given filename
   * @param[in] name The filename of the model.
   * @return A smart-pointer to a newly allocated model
   */
  static ModelPtr New(const std::string& name);

public: // From ResourceTicketObserver
  /**
   * @copydoc Dali::Internal::ResourceTicketObserver::ResourceLoadingFailed()
   */
  virtual void ResourceLoadingFailed(const ResourceTicket& ticket);

  /**
   * @copydoc Dali::Internal::ResourceTicketObserver::ResourceLoadingSucceeded()
   */
  virtual void ResourceLoadingSucceeded(const ResourceTicket& ticket);

  /**
   * @copydoc Dali::Internal::ResourceTicketObserver::ResourceUploaded()
   */
  virtual void ResourceUploaded(const ResourceTicket& ticket);

  /**
   * @copydoc Dali::Internal::ResourceTicketObserver::ResourceSavingFailed()
   */
  virtual void ResourceSavingFailed(const ResourceTicket& ticket);

  /**
   * @copydoc Dali::Internal::ResourceTicketObserver::ResourceSavingSucceeded()
   */
  virtual void ResourceSavingSucceeded(const ResourceTicket& ticket);


public: // Query methods from external model
  /**
   * @copydoc Dali::Model::GetLoadingState()
   */
  LoadingState GetLoadingState()
  {
    return mTicket->GetLoadingState();
  }

  /**
   * @copydoc Dali::Model::LoadingFinishedSignal()
   */
  Dali::Model::ModelSignalType& LoadingFinishedSignal()
  {
    return mLoadingFinished;
  };

  /**
   * @copydoc Dali::Model::SavingFinishedSignal()
   */
  Dali::Model::ModelSaveSignalType& SavingFinishedSignal()
  {
    return mSavingFinished;
  };

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

  /**
   * returns the Id used for lookups
   * @return the unique ID of the image data resource
   */
  ResourceId GetResourceId() const;

  /**
   * @copydoc Dali::Model::NumberOfAnimations().
   */
  unsigned int NumberOfAnimations() const;

  /**
   * @copydoc Dali::Model::FindAnimation().
   */
  bool FindAnimation(const std::string& animationName, unsigned int & animationIndex);

  /**
   * @copydoc Dali::Model::Write()
   */
  void Write();

  /**
   * @copydoc Dali::Model::Save()
   */
  void Save(const std::string& url);

public:
  /**
   * Accessor for internal classes only.
   * @return The model data, or NULL if not loaded yet
   */
  Internal::ModelDataPtr GetModelData() const;

private:

  /**
   * Constructor
   * @param[in] name The filename of the model.
   * @return          None.
   */
  Model(const std::string& name);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Model();

private:

  ResourceTicketPtr mTicket;

  Dali::Model::ModelSignalType     mLoadingFinished;
  Dali::Model::ModelSaveSignalType mSavingFinished;

  ResourceClient* mResourceClient;
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Internal::Model& GetImplementation(Dali::Model& model)
{
  DALI_ASSERT_ALWAYS( model && "Model handle is empty" );

  BaseObject& handle = model.GetBaseObject();

  return static_cast<Internal::Model&>(handle);
}

inline const Internal::Model& GetImplementation(const Dali::Model& model)
{
  DALI_ASSERT_ALWAYS( model && "Model handle is empty" );

  const BaseObject& handle = model.GetBaseObject();

  return static_cast<const Internal::Model&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_MODEL_H__
