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
#include <dali/internal/render/shaders/custom-uniform.h>

// INTERNAL INCLUDES
#include <dali/internal/render/shaders/program.h>

namespace Dali
{

namespace Internal
{

CustomUniform::CustomUniform()
: mCacheIndex( 0 )
{
}

CustomUniform::~CustomUniform()
{
}

void CustomUniform::Initialize( Program& program, const char* name )
{
  mCacheIndex = program.RegisterUniform( name );
}

void CustomUniform::Reset()
{
  mCacheIndex = 0;
}

const int CustomUniform::GetUniformLocation( Program& program, const char* name )
{
  if( 0 == mCacheIndex )
  {
    Initialize( program, name );
  }

  return program.GetUniformLocation( mCacheIndex );
}

} // namespace Internal

} // namespace Dali
