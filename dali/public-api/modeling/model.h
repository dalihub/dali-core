#ifndef __DALI_MODEL_H__
#define __DALI_MODEL_H__

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
#include <boost/function.hpp>
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/common/loading-state.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{

class MeshActor;
class Animation;

namespace Internal DALI_INTERNAL
{
class Model;
}

/**
 * @brief A handle to 3D model data loaded as a resource.
 *
 * Use ModelActorFactory::BuildActorTree() to create actors from this model.
 * Use ModelActorFactory::BuildAnimation() to create animations on such actors.
 */
class DALI_IMPORT_API Model : public BaseHandle
{
public:

  typedef Signal<void (Model)> ModelSignalType; ///< Signal type
  typedef Signal<void (Model,bool)> ModelSaveSignalType; ///< Signal type for saving models

  //Signal Names
  static const char* const SIGNAL_MODEL_LOADING_FINISHED; ///< name "model-loading-finished"
  static const char* const SIGNAL_MODEL_SAVING_FINISHED;  ///< name "model-saving-finished"

public:
  /**
   * @brief Create an uninitialized Model.
   *
   * This can be initialised with Model::New().  Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  Model();

  /**
   * @brief Asynchronously load a model.
   *
   * Connect to SignalLoadingFinished() to determine when the model
   * has finished loading.
   *
   * @param [in] url The url of the model data.
   * @return A handle to a newly allocated Dali resource.
   */
  static Model New(const std::string& url);

  /**
   * @brief Downcast an Object handle to Model handle.
   *
   * If handle points to a Model object the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle to An object
   * @return handle to an Model object or an uninitialized handle
   */
  static Model DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Model();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  Model(const Model& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  Model& operator=(const Model& rhs);

  /**
   * @brief Query whether the model data has loaded.
   *
   * The asynchronous loading begins when the Model object is created.
   * After the Model object is discarded, the model data will be released from memory.
   * @return The loading state, either Loading, Success or Failed.
   */
  LoadingState GetLoadingState();

  /**
   * @brief Emitted when the model data loads successfully or when the loading fails.
   *
   * @return A signal object to Connect() with.
   */
  ModelSignalType& LoadingFinishedSignal();

  /**
   * @brief Emitted when the model data save request completes.
   *
   * @return A signal object to Connect() with.
   */
  ModelSaveSignalType& SavingFinishedSignal();

  /**
   * @brief Get number of animations in the model.
   *
   * @pre The model has been loaded.
   * @return The number of animations encoded in the model, or zero if the model
   * hasn't finished loading.
   */
  size_t NumberOfAnimations() const;

  /**
   * @brief Get the index of a named animation map in the model.
   *
   * @param[in] animationName The name of the animation to find
   * @param[out] animationIndex The index of the named animation if found
   * @return true if the animation was foud, false if not found.
   */
  bool FindAnimation(const std::string& animationName, unsigned int& animationIndex);

  /**
   * @brief Write the model data to the standard output in textual format.
   *
   * Note - will assert if any part of the model is on the scene graph (i.e. if
   * an actor has been created from this model)
   */
  void Write();

  /**
   * @brief Save a Dali representation of the mode data.
   *
   * Used for faster loading on subsequent uses.
   * @param[in] url The resource url for the data
   */
  void Save(const std::string& url);

public: // Not intended for application developers

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in] model A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Model(Internal::Model* model);
};

} // namespace Dali

#endif // __DALI_MODEL_H__
