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
#include <algorithm>
#include <sstream>
#include <boost/functional/hash.hpp>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/event/effects/shader-effect-impl.h>
#include <dali/internal/event/effects/shader-declarations.h>

// the generated shader strings
#include "dali-shaders.h"


// Use pre-compiler constants in order to utilize string concatenation
#define SHADER_DEF_USE_BONES    "#define USE_BONES\n"
#define SHADER_DEF_USE_LIGHTING "#define USE_LIGHTING\n"
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

  shaderHash = HashShaderSource(vertexSource, fragmentSource);
  std::stringstream stringHash;
  stringHash << shaderHash;
  std::string filename = DALI_SHADERBIN_DIR;
  filename += stringHash.str();
  filename += ".dali-bin";

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
    // Load a shader (loaded in Update thread)
    ticket = mResourceClient.LoadShader(resourceType, filename);
    DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "ShaderFactory::Load Ticket ID:%d, path: \"%s\"\n", ticket->GetId(), filename.c_str());

    mResourceTypePathIdMap.insert( ResourceTypePathIdPair( typePath, ticket->GetId() ) );
  }

  return ticket;
}

void ShaderFactory::LoadDefaultShaders()
{
  mDefaultShader = ShaderEffect::New();

  mDefaultShader->SetProgram( GEOMETRY_TYPE_IMAGE, SHADER_DEFAULT, FlatColorTextureVertex, FlatColorTextureFragment );

  mDefaultShader->SetProgram( GEOMETRY_TYPE_TEXT, SHADER_DEFAULT, DistanceFieldFontVertex, DistanceFieldFontFragment );

  LoadTextSubtypeShaders(mDefaultShader); // TODO: Remove when applications no longer need these shaders

  // Untextured meshes
  mDefaultShader->SetProgram( GEOMETRY_TYPE_MESH, SHADER_DEFAULT,
                              "", // Vertex shader defs
                              SHADER_DEF_USE_LIGHTING, // fragment shader defs
                              MeshColorNoTextureVertex, MeshColorNoTextureFragment );

  mDefaultShader->SetProgram(GEOMETRY_TYPE_MESH, SHADER_EVENLY_LIT,
                             "", // Vertex shader defs
                             "", // fragment shader defs
                             MeshColorNoTextureVertex, MeshColorNoTextureFragment);

  mDefaultShader->SetProgram(GEOMETRY_TYPE_MESH, SHADER_RIGGED_AND_LIT,
                             SHADER_DEF_USE_BONES,    // vertex shader defs
                             SHADER_DEF_USE_LIGHTING, // fragment shader defs
                             MeshColorNoTextureVertex, MeshColorNoTextureFragment);

  mDefaultShader->SetProgram(GEOMETRY_TYPE_MESH, SHADER_RIGGED_AND_EVENLY_LIT,
                             SHADER_DEF_USE_BONES, // Vertex shader defs
                             "",                   // Fragment shader defs
                             MeshColorNoTextureVertex, MeshColorNoTextureFragment);

  mDefaultShader->SetProgram(GEOMETRY_TYPE_MESH, SHADER_RIGGED_AND_VERTEX_COLOR,
                             (SHADER_DEF_USE_BONES SHADER_DEF_USE_COLOR), // Vertex shader defs
                             SHADER_DEF_USE_COLOR,                        // Fragment shader defs
                             MeshColorNoTextureVertex, MeshColorNoTextureFragment);

  mDefaultShader->SetProgram(GEOMETRY_TYPE_MESH, SHADER_VERTEX_COLOR,
                             SHADER_DEF_USE_COLOR,  // Vertex shader defs
                             SHADER_DEF_USE_COLOR,  // Fragment shader defs
                             MeshColorNoTextureVertex, MeshColorNoTextureFragment);

  // Textured meshes
  mDefaultShader->SetProgram( GEOMETRY_TYPE_TEXTURED_MESH, SHADER_DEFAULT,
                              "",                      // Vertex shader defs
                              SHADER_DEF_USE_LIGHTING, // fragment shader defs
                              MeshVertex, MeshFragment );


  mDefaultShader->SetProgram(GEOMETRY_TYPE_TEXTURED_MESH, SHADER_EVENLY_LIT,
                             "",                      // Vertex shader defs
                             "",                      // Fragment shader defs
                             MeshVertex, MeshFragment);

  mDefaultShader->SetProgram(GEOMETRY_TYPE_TEXTURED_MESH, SHADER_RIGGED_AND_LIT,
                             SHADER_DEF_USE_BONES,    // Vertex shader defs
                             SHADER_DEF_USE_LIGHTING, // Fragment shader defs
                             MeshVertex, MeshFragment);

  mDefaultShader->SetProgram(GEOMETRY_TYPE_TEXTURED_MESH, SHADER_RIGGED_AND_EVENLY_LIT,
                             SHADER_DEF_USE_BONES, // Vertex shader defs
                             "",                   // Fragment shader defs
                             MeshVertex, MeshFragment);
}

void ShaderFactory::LoadTextSubtypeShaders(ShaderEffectPtr shaderEffect)
{
  shaderEffect->SetProgram(GEOMETRY_TYPE_TEXT, SHADER_GRADIENT,
                             SHADER_DEF_USE_GRADIENT,
                             SHADER_DEF_USE_GRADIENT,
                             DistanceFieldFontVertex, DistanceFieldFontFragment);

  shaderEffect->SetProgram(GEOMETRY_TYPE_TEXT, SHADER_GRADIENT_GLOW, DistanceFieldFontGlowVertex, DistanceFieldFontGlowFragment);

  shaderEffect->SetProgram(GEOMETRY_TYPE_TEXT, SHADER_GRADIENT_SHADOW, DistanceFieldFontShadowVertex, DistanceFieldFontShadowFragment);

  shaderEffect->SetProgram(GEOMETRY_TYPE_TEXT, SHADER_GRADIENT_OUTLINE, DistanceFieldFontOutlineVertex, DistanceFieldFontOutlineFragment);

  shaderEffect->SetProgram(GEOMETRY_TYPE_TEXT, SHADER_GRADIENT_OUTLINE_GLOW, DistanceFieldFontOutlineGlowVertex, DistanceFieldFontOutlineGlowFragment);
}


size_t ShaderFactory::HashShaderSource(const std::string& vertexSource, const std::string& fragmentSource) const
{
  std::string source = vertexSource + fragmentSource;

  // remove all white spaces, tabs and new lines
  source.erase(std::remove(source.begin(), source.end(), ' '), source.end());
  source.erase(std::remove(source.begin(), source.end(), '\n'), source.end());
  source.erase(std::remove(source.begin(), source.end(), '\t'), source.end());

  return boost::hash_value( source );
}

} // namespace Internal

} // namespace Dali
