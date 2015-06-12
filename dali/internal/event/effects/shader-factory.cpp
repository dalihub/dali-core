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
#include <dali/integration-api/platform-abstraction.h>
#include <dali/internal/event/common/thread-local-storage.h>
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

namespace
{

/**
 * @brief Generates a filename for a shader binary based on the hash value passed in.
 * @param[in] shaderHash A hash over shader sources.
 * @param[out] filename A string to overwrite with the filename.
 */
void shaderBinaryFilename( size_t shaderHash, std::string& filename )
{
  std::stringstream binaryShaderFilenameBuilder( std::ios_base::out );
  binaryShaderFilenameBuilder << CORE_MAJOR_VERSION << VERSION_SEPARATOR << CORE_MINOR_VERSION << VERSION_SEPARATOR << CORE_MICRO_VERSION << VERSION_SEPARATOR
                              << shaderHash
                              << SHADER_SUFFIX;
  filename = binaryShaderFilenameBuilder.str();
}

}

ShaderFactory::ShaderFactory()
{
}

ShaderFactory::~ShaderFactory()
{
  // Let all the cached objects destroy themselves:
  for( int i = 0, cacheSize = mShaderBinaryCache.Size(); i < cacheSize; ++i )
  {
    if( mShaderBinaryCache[i] )
    {
      mShaderBinaryCache[i]->Unreference();
    }
  }
}

ShaderDataPtr ShaderFactory::Load( const std::string& vertexSource, const std::string& fragmentSource, size_t& shaderHash )
{
  // Work out the filename for the binary that the glsl source will be compiled and linked to:
  shaderHash = CalculateHash( vertexSource.c_str(), fragmentSource.c_str() );
  std::string binaryShaderFilename;
  shaderBinaryFilename( shaderHash, binaryShaderFilename );

  ShaderDataPtr shaderData;

  /// Check a cache of previously loaded shaders:
  for( int i = 0, cacheSize = mShaderBinaryCache.Size(); i < cacheSize; ++i )
  {
    if( mShaderBinaryCache[i]->GetHashValue() == shaderHash )
    {
      shaderData = mShaderBinaryCache[i];

      DALI_LOG_INFO( Debug::Filter::gShader, Debug::General, "Mem cache hit on path: \"%s\"\n", binaryShaderFilename.c_str() );
      break;
    }
  }

  // If memory cache failed check the file system for a binary or return a source-only ShaderData:
  if( shaderData.Get() == NULL )
  {
    // Allocate the structure that returns the loaded shader:
    shaderData = new ShaderData( vertexSource, fragmentSource );
    shaderData->SetHashValue( shaderHash );
    shaderData->GetBuffer().Clear();

    // Try to load the binary (this will fail if the shader source has never been compiled before):
    ThreadLocalStorage& tls = ThreadLocalStorage::Get();
    Integration::PlatformAbstraction& platformAbstraction = tls.GetPlatformAbstraction();
    const bool loaded = platformAbstraction.LoadShaderBinaryFile( binaryShaderFilename, shaderData->GetBuffer() );

    if( loaded )
    {
      MemoryCacheInsert( *shaderData );
    }

    DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, loaded ?
        "loaded on path: \"%s\"\n" :
        "failed to load on path: \"%s\"\n",
        binaryShaderFilename.c_str());
  }

  return shaderData;
}

void ShaderFactory::SaveBinary( Integration::ShaderDataPtr shaderData )
{
  // Save the binary to the file system:
  std::string binaryShaderFilename;
  shaderBinaryFilename( shaderData->GetHashValue(), binaryShaderFilename );

  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  Integration::PlatformAbstraction& platformAbstraction = tls.GetPlatformAbstraction();
  const bool saved = platformAbstraction.SaveShaderBinaryFile( binaryShaderFilename, &shaderData->GetBuffer()[0], shaderData->GetBufferSize() );

  // Save the binary into to memory cache:
  MemoryCacheInsert( *shaderData );

  DALI_LOG_INFO( Debug::Filter::gShader, Debug::General, saved ? "Saved to file: %s\n" : "Save to file failed: %s\n", binaryShaderFilename.c_str() );
  if( saved ) {} // Avoid unused variable warning in release builds
}

void ShaderFactory::LoadDefaultShaders()
{
  mDefaultShader = ShaderEffect::New();

  mDefaultShader->SendProgramMessage( ImageVertex, ImageFragment, false );
}

void ShaderFactory::MemoryCacheInsert( ShaderData& shaderData )
{
  DALI_ASSERT_DEBUG( shaderData.GetBufferSize() > 0 );

  // Save the binary into to memory cache:
  if( shaderData.GetBufferSize() > 0 )
  {
    mShaderBinaryCache.Reserve( mShaderBinaryCache.Size() + 1 ); // Make sure the push won't throw after we inc the ref count.
    shaderData.Reference();
    mShaderBinaryCache.PushBack( &shaderData );
    DALI_LOG_INFO( Debug::Filter::gShader, Debug::General, "CACHED BINARY FOR HASH: %u\n", shaderData.GetHashValue() );
  }
}

} // namespace Internal

} // namespace Dali
