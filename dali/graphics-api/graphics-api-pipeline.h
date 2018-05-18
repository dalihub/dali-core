#ifndef DALI_GRAPHICS_API_PIPELINE_H
#define DALI_GRAPHICS_API_PIPELINE_H

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

namespace Dali
{
namespace Graphics
{
namespace API
{

/**
 *
 */
class Pipeline
{
public:
  // not copyable
  Pipeline(const Pipeline&) = delete;
  Pipeline& operator=(const Pipeline&) = delete;

  virtual ~Pipeline() = default;

protected:

  // derived types should not be moved directly to prevent slicing
  Pipeline(Pipeline&&) = default;
  Pipeline& operator=(Pipeline&&) = default;

  /**
   * Objects of this type should not directly.
   */
  Pipeline() = default;
};

}
}
}


#endif // DALI_GRAPHICS_API_PIPELINE_H
