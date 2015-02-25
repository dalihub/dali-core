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

// CLASS HEADER
#include <dali/internal/event/modeling/model-impl.h>

// EXTERNAL INCLUDES
#include <cstdio>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/resource-cache.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/modeling/model-factory.h>
#include <dali/internal/event/modeling/model-logger.h>
#include <dali/internal/event/modeling/model-data-impl.h>
#include <dali/public-api/modeling/model-animation-map.h>
#include <dali/public-api/object/type-registry.h>

#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/update/resources/resource-manager.h>

namespace Dali
{

using Internal::ModelDataPtr;
using Integration::ModelResourceType;
using Integration::PlatformAbstraction;

namespace Internal
{

namespace
{

// Signals

const char* const SIGNAL_MODEL_LOADING_FINISHED = "model-loading-finished";
const char* const SIGNAL_MODEL_SAVING_FINISHED =  "model-saving-finished";

TypeRegistration mType( typeid( Dali::Model ), typeid( Dali::BaseHandle ), NULL );

SignalConnectorType signalConnector1( mType, SIGNAL_MODEL_LOADING_FINISHED, &Model::DoConnectSignal );
SignalConnectorType signalConnector2( mType, SIGNAL_MODEL_SAVING_FINISHED,  &Model::DoConnectSignal );

} // unnamed namespace

using Dali::Vector4;

ModelPtr Model::New(const std::string& name)
{
  ModelPtr model(new Model(name));

  model->RegisterObject();

  return model;
}

Model::Model(const std::string& name)
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gModel);

  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  mResourceClient = &tls.GetResourceClient();

  ModelFactory& modelFactory = tls.GetModelFactory();

  mTicket = modelFactory.Load(name);
  mTicket->AddObserver(*this);
}

Model::~Model()
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gModel);
  mTicket->RemoveObserver(*this);

  UnregisterObject();
}

bool Model::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  Model* model = dynamic_cast<Model*>(object);

  if( 0 == strcmp( signalName.c_str(), SIGNAL_MODEL_LOADING_FINISHED ) )
  {
    model->LoadingFinishedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_MODEL_SAVING_FINISHED ) )
  {
    model->SavingFinishedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void Model::ResourceLoadingFailed(const ResourceTicket& ticket)
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gModel);

  mLoadingFinished.Emit( Dali::Model( this ) );
}

void Model::ResourceLoadingSucceeded(const ResourceTicket& ticket)
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gModel);

  // Generate resource tickets for meshes
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  ResourceManager& resourceManager = tls.GetResourceManager();
  ModelDataPtr modelData = resourceManager.GetModelData( GetResourceId() );
  modelData->Unpack( *mResourceClient );

  mLoadingFinished.Emit( Dali::Model( this ) );
}

void Model::ResourceUploaded(const ResourceTicket& ticket)
{
  // do nothing - not a GL resource
}

void Model::ResourceSavingFailed(const ResourceTicket& ticket)
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gModel);

  mSavingFinished.Emit( Dali::Model( this ), false );
}

void Model::ResourceSavingSucceeded(const ResourceTicket& ticket)
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gModel);

  mSavingFinished.Emit( Dali::Model( this ), true );
}

ResourceId Model::GetResourceId() const
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gModel);
  DALI_ASSERT_DEBUG(mTicket);

  return mTicket->GetId();
}

/********************************************************************************
 * Write the model data to standard output
 */
void Model::Write()
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  ResourceManager& resourceManager = tls.GetResourceManager();
  ModelDataPtr modelData = resourceManager.GetModelData(GetResourceId());

  if(modelData != 0)
  {
    ModelLogger modelLogger(*(modelData.Get()));
    modelLogger.mWriteVertices = false; // TODO - add a public traits class for model logger
    modelLogger.Write();
  }
  else
  {
#ifndef EMSCRIPTEN
    fprintf(stderr, "%s: Model not loaded yet\n", __PRETTY_FUNCTION__);
#else
    // printf to stderr doesnt always show in browser console window
    printf("%s: Model not loaded yet\n", __PRETTY_FUNCTION__);
#endif
  }
}

/********************************************************************************
 * Write the model data to a url
 */
void Model::Save(const std::string& url)
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  ResourceManager& resourceManager = tls.GetResourceManager();
  if( resourceManager.GetModelData(GetResourceId()) )
  {
    mResourceClient->SaveResource(mTicket, url);
  }
}

/********************************************************************************
 *
 */
unsigned int Model::NumberOfAnimations() const
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gModel);

  unsigned int numAnimations = 0;
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  ResourceManager& resourceManager = tls.GetResourceManager();
  ModelDataPtr modelData = resourceManager.GetModelData(GetResourceId());

  if (modelData)
  {
    numAnimations = modelData->NumberOfAnimationMaps();
  }

  return numAnimations;
}

bool Model::FindAnimation(const std::string& animationName, unsigned int & animationIndex)
{
  bool found = false;

  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  ResourceManager& resourceManager = tls.GetResourceManager();
  ModelDataPtr modelData = resourceManager.GetModelData(GetResourceId());

  if (modelData)
  {
    found = modelData->FindAnimation(animationName, animationIndex);
  }
  return found;
}

/********************************************************************************
 * Return the model data, if it's finished loading.
 */
ModelDataPtr Model::GetModelData() const
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gModel);

  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  ResourceManager& resourceManager = tls.GetResourceManager();
  ModelDataPtr modelData = resourceManager.GetModelData(GetResourceId());
  return modelData;
}

} // namespace Internal
} // namespace Dali
