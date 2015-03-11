#ifndef DALI_INTERNAL_CONNECTABLE_H
#define DALI_INTERNAL_CONNECTABLE_H

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

// INTERNAL INCLUDES

namespace Dali
{
namespace Internal
{

/**
 * @brief Interface for objects that can be connected to the stage.
 */
class Connectable
{
public:

  /**
   * @brief Destructor
   */
  virtual ~Connectable() {}

  /**
   * @brief Check if the object is on stage
   */
  virtual bool OnStage() const = 0;

  /**
   * @brief Notify this objects that it has been connected to the stage.
   */
  virtual void Connect() = 0;

  /**
   * @brief Notify this objects that it has been disconnected to the stage.
   */
  virtual void Disconnect() = 0;

};

} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_CONNECTABLE_H
