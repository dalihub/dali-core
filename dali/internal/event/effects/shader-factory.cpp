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
#include <dali/internal/event/effects/shader-factory.h>

// EXTERNAL INCLUDES
#include <sstream>

// INTERNAL INCLUDES
#include <dali/public-api/dali-core-version.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/devel-api/common/hash.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/event/effects/shader-effect-impl.h>
#include <dali/internal/event/effects/shader-declarations.h>

// compile time generated shader strings
#include "dali-shaders.h"

namespace
{
const char* VERSION_SEPARATOR = "-";
const char* SHADER_SUFFIX = ".dali-bin";
}

// Use pre-compiler constants in order to utilize string concatenation
#define SHADER_DEF_USE_BONES    "#define USE_BONES\n"
#define SHADER_DEF_USE_COLOR    "#define USE_COLOR\n"
#define SHADER_DEF_USE_GRADIENT "#define USE_GRADIENT\n"

using namespace Dali::Integration;

namespace Dali
{

namespace Internal
{

ShaderFactory::ShaderFactory(ResourceClient& resourceClient)
: mResourceClient(resourceClient)
{
}

ShaderFactory::~ShaderFactory()
{
}

ResourceTicketPtr ShaderFactory::Load(const std::string& vertexSource, const std::string& fragmentSource, size_t& shaderHash)
{
  ResourceTicketPtr ticket;

  shaderHash = CalculateHash(vertexSource, fragmentSource);
  std::stringstream stringHash;
  stringHash << CORE_MAJOR_VERSION << VERSION_SEPARATOR << CORE_MINOR_VERSION << VERSION_SEPARATOR << CORE_MICRO_VERSION << VERSION_SEPARATOR;
  stringHash << shaderHash;
  std::string filename;
  filename.append( stringHash.str() );
  filename.append( SHADER_SUFFIX );

  ShaderResourceType resourceType(shaderHash, vertexSource, fragmentSource);
  ResourceTypePath typePath(resourceType, filename);

  // Search for a matching resource
  ResourceTypePathIdIter iter = mResourceTypePathIdMap.end();
  if ( !mResourceTypePathIdMap.empty() )
  {
    iter = mResourceTypePathIdMap.find( typePath );
  }

  if ( mResourceTypePathIdMap.end() != iter )
  {
    // The resource was previously requested
    unsigned int resourceId = iter->second;

    // The ticket may still be alive, request another copy
    ticket = mResourceClient.RequestResourceTicket( resourceId );

    // Clean-up the map of resource IDs, if the ticket has been discarded
    if ( !ticket )
    {
      mResourceTypePathIdMap.erase( iter );
    }
    else
    {
      DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "ShaderFactory::Load filename= %s already requested to Load\n", filename.c_str());
    }
  }

  if ( !ticket )
  {
    // Load the shader (loaded synchronously in Update thread so its ready by the time the set shader message is processed)
    ticket = mResourceClient.LoadShader(resourceType, filename);
    DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "ShaderFactory::Load Ticket ID:%d, path: \"%s\"\n", ticket->GetId(), filename.c_str());

    mResourceTypePathIdMap.insert( ResourceTypePathIdPair( typePath, ticket->GetId() ) );
  }

  return ticket;
}

void ShaderFactory::LoadDefaultShaders()
{
  mDefaultShader = ShaderEffect::New();

  mDefaultShader->SendProgramMessage( GEOMETRY_TYPE_IMAGE, ImageVertex, ImageFragment, false );
}

} // namespace Internal

} // namespace Dali
