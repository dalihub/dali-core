#ifndef __DALI_INTERNAL_GL_CALL_DEBUG_H__
#define __DALI_INTERNAL_GL_CALL_DEBUG_H__

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

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Integration
{
class GlAbstraction;
}

namespace Internal
{

/**
 * Helper to check GL errors
 * @param glAbstraction to use for error check
 * @param operation to be logged
 */
void CheckGlError( Integration::GlAbstraction& glAbstraction, const char* operation );

// wrap gl calls with CHECK_GL e.g. "CHECK_GL( mGlAbstraction, mGlAbstraction.UseProgram(mProgramId) );"
// will LOG any glErrors eg "glError (0x0501) GL_INVALID_VALUE - glBindTexture(textureId)"
// only enable if specifically enabled as it can slow down GL a lot!
#ifdef DALI_GL_ERROR_CHECK
#define CHECK_GL(c,a)  (a); CheckGlError(c,#a)
#else
#define CHECK_GL(c,a)  (a)
#endif

#ifdef DEBUG_ENABLED
/// Switch debug level to Concise to disable, General to enable. Note, enabling snapshot logging will do this on the fly.
extern Debug::Filter* gGlLogFilter;
#endif // DEBUG_ENABLED

// Don't put guards around here (LOG_INFO has it's own guards)
#define LOG_GL(format, args...) \
  DALI_LOG_INFO(gGlLogFilter, Debug::General, format, ## args)

}

}

#endif // __DALI_INTERNAL_GL_CALL_DEBUG_H__

