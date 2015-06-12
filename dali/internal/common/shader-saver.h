#ifndef __DALI_INTERNAL_SHADER_DISPATCHER_H__
#define __DALI_INTERNAL_SHADER_DISPATCHER_H__

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
#include <dali/public-api/common/intrusive-ptr.h>

// EXTERNAL INCLUDES


namespace Dali
{

namespace Integration
{
class ShaderData;
typedef IntrusivePtr<ShaderData> ShaderDataPtr;
}

namespace Internal
{

/**
 * Abstract interface for passing a ShaderData object towards being saved.
 */
class ShaderSaver
{
public:

  /**
   * A function saving the binary from a ShaderDataPtr or passing it on to where it can be saved.
   * @param[in] shaderData A smart pointer to a ShaderData for which the program binary should be saved.
   */
  virtual void SaveBinary( Integration::ShaderDataPtr shaderData ) = 0;

protected:
  /**
   * Destructor. Protected as no derived class should ever be deleted
   * through a reference to this pure abstract interface.
   */
  virtual ~ShaderSaver(){}
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SHADER_DISPATCHER_H__
