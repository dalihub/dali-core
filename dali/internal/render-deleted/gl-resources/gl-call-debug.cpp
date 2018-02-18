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

// HEADER
#include <dali/internal/render/gl-resources/gl-call-debug.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/gl-abstraction.h>
#include <dali/integration-api/gl-defines.h>

namespace
{
/**
 * GL error strings
 */
struct errorStrings
{
  const Dali::GLenum errorCode;
  const char* errorString;
};
errorStrings errors[] =
{
   { GL_NO_ERROR,           "GL_NO_ERROR" },
   { GL_INVALID_ENUM,       "GL_INVALID_ENUM" },
   { GL_INVALID_VALUE,      "GL_INVALID_VALUE" },
   { GL_INVALID_OPERATION,  "GL_INVALID_OPERATION" },
   { GL_OUT_OF_MEMORY,      "GL_OUT_OF_MEMORY" }
};

const char* ErrorToString( Dali::GLenum errorCode )
{
  for( unsigned int i = 0; i < sizeof(errors) / sizeof(errors[0]); ++i)
  {
    if (errorCode == errors[i].errorCode)
    {
      return errors[i].errorString;
    }
  }
  return "Unknown Open GLES error";
}

}

namespace Dali
{

namespace Internal
{

#ifdef DEBUG_ENABLED
/// Switch debug level to Concise to disable, General to enable. Note, enabling snapshot logging will do this on the fly.
Debug::Filter* gGlLogFilter = Debug::Filter::New(Debug::Concise, false, "LOG_CONTEXT");
#endif // DEBUG_ENABLED

void CheckGlError( Integration::GlAbstraction& glAbstraction, const char* operation )
{
  bool foundError = false;
  while( GLint error = glAbstraction.GetError() )
  {
    DALI_LOG_ERROR( "glError (0x%x) %s - after %s\n",  error, ErrorToString( error ), operation );
    foundError = true;
  }
  DALI_ASSERT_ALWAYS( !foundError && "GL ERROR" ); // if errors are being checked we should assert
}

}

}
