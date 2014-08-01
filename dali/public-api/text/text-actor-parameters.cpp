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

// FILE HEADER
#include <dali/public-api/text/text-actor-parameters.h>

// INTERNAL INCLUDES
#include <dali/public-api/text/text-style.h>

namespace Dali
{

namespace
{

static const TextStyle                                   DEFAULT_STYLE;
static const TextActorParameters::AutomaticFontDetection DEFAULT_AUTOMATIC_FONT_DETECTION = TextActorParameters::FONT_DETECTION_ON;

} // namespace

// Default font parameters. Adaptor will choose default system parameters.
TextActorParameters DEFAULT_TEXT_ACTOR_PARAMETERS;

struct TextActorParameters::Impl
{
  Impl()
  : mStyle(),
    mFontDetection( DEFAULT_AUTOMATIC_FONT_DETECTION )
  {
  }

  Impl( const TextStyle& style, AutomaticFontDetection fontDetection )
  : mStyle( style ),
    mFontDetection( fontDetection )
  {}

  TextStyle              mStyle;
  AutomaticFontDetection mFontDetection;
};

TextActorParameters::TextActorParameters()
: mImpl( NULL )
{
}

TextActorParameters::TextActorParameters( const TextStyle& style, AutomaticFontDetection fontDetection )
: mImpl( new Impl( style, fontDetection ) )
{
}

TextActorParameters::~TextActorParameters()
{
  delete mImpl;
}

TextActorParameters::TextActorParameters( const TextActorParameters& parameters )
: mImpl( NULL )
{
  *this = parameters;
}

TextActorParameters& TextActorParameters::operator=( const TextActorParameters& parameters )
{
  if( &parameters != this )
  {
    if( NULL == parameters.mImpl )
    {
      delete mImpl;
      mImpl = NULL;
    }
    else if( NULL == mImpl )
    {
      mImpl = new Impl( parameters.mImpl->mStyle,
                        parameters.mImpl->mFontDetection );
    }
    else
    {
      mImpl->mStyle = parameters.mImpl->mStyle;
      mImpl->mFontDetection = parameters.mImpl->mFontDetection;
    }
  }

  return *this;
}

const TextStyle& TextActorParameters::GetTextStyle() const
{
  return ( NULL == mImpl ) ? DEFAULT_STYLE : mImpl->mStyle;
}

bool TextActorParameters::IsAutomaticFontDetectionEnabled() const
{
  return ( NULL == mImpl ) ? ( DEFAULT_AUTOMATIC_FONT_DETECTION == TextActorParameters::FONT_DETECTION_ON ) : ( mImpl->mFontDetection == TextActorParameters::FONT_DETECTION_ON );
}

} // namespace Dali
