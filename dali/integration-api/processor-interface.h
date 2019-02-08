#ifndef DALI_INTEGRATION_PROCESSOR_INTERFACE_H
#define DALI_INTEGRATION_PROCESSOR_INTERFACE_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace Integration
{

/**
 * Interface to enable classes to be processed after the event loop. Classes are processed
 * in the order they are registered.
 */
class DALI_CORE_API Processor
{
public:
  /**
   * @brief Run the processor
   */
  virtual void Process() = 0;

protected:

  /**
   * Virtual protected destructor
   */
  virtual ~Processor() { }
};

} // Dali

} // Integration

#endif // DALI_INTEGRATION_PROCESSOR_INTERFACE_H