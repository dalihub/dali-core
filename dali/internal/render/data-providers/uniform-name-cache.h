#ifndef DALI_INTERNAL_UNIFORM_NAME_CACHE_H
#define DALI_INTERNAL_UNIFORM_NAME_CACHE_H

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
 */

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/devel-api/common/owner-container.h>

namespace Dali
{

namespace Internal
{

namespace Render
{

/**
 * This class maps uniform names to unique indices that can be used to cache the GL uniform
 * index values in programs and only do the costly string lookup once
 */
class UniformNameCache
{
public:

  /**
   * Constructor
   */
  UniformNameCache();

  /**
   * Destructor
   */
  ~UniformNameCache();

  /**
   * This method can be used to query a cache for the unique index for a sampler uniform
   * @param uniformName to get the index for
   * @return a unique index for this sampler uniform
   */
  int32_t GetSamplerUniformUniqueIndex( const std::string& uniformName );

private: // Data

  struct UniformEntry
  {
    UniformEntry( const std::string& name, std::size_t hash )
    : uniformName( name ),
      nameHash( hash )
    { }
    std::string uniformName;
    std::size_t nameHash;
  };

  OwnerContainer< UniformEntry* > mSamplerUniformCache;

};

} // namespace Render

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_UNIFORM_NAME_CACHE_H
