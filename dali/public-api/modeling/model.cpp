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
#include <dali/public-api/modeling/model.h>

// INTERNAL INCLUDES
#include <dali/internal/event/modeling/model-impl.h>
#include <dali/internal/event/actors/mesh-actor-impl.h>
#include <dali/public-api/animation/animation.h>

namespace Dali
{

const char* const Model::SIGNAL_MODEL_LOADING_FINISHED = "model-loading-finished";
const char* const Model::SIGNAL_MODEL_SAVING_FINISHED = "model-saving-finished";

Model Model::New(const std::string& url)
{
  Internal::ModelPtr internal = Internal::Model::New(url);
  return Model(internal.Get());
}

Model Model::DownCast( BaseHandle handle )
{
  return Model( dynamic_cast<Dali::Internal::Model*>(handle.GetObjectPtr()) );
}

Model::Model()
{
}

Model::Model(Internal::Model* internal)
: BaseHandle(internal)
{
}

Model::~Model()
{
}

LoadingState Model::GetLoadingState()
{
  return GetImplementation(*this).GetLoadingState();
}

Model::ModelSignalV2& Model::LoadingFinishedSignal()
{
  return GetImplementation(*this).LoadingFinishedSignal();
}

Model::ModelSaveSignalV2& Model::SavingFinishedSignal()
{
  return GetImplementation(*this).SavingFinishedSignal();
}

size_t Model::NumberOfAnimations() const
{
  return GetImplementation(*this).NumberOfAnimations();
}

bool Model::FindAnimation(const std::string& animationName, unsigned int & animationIndex)
{
  return GetImplementation(*this).FindAnimation(animationName, animationIndex);
}

void Model::Write()
{
  GetImplementation(*this).Write();
}

void Model::Save(const std::string& url)
{
  GetImplementation(*this).Save(url);
}

} // namespace Dali
