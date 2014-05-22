#ifndef __DALI_INTERNAL_TEXTURE_OBSERVER_H__
#define __DALI_INTERNAL_TEXTURE_OBSERVER_H__

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

namespace Dali
{
namespace Internal
{

/**
 * The TextureObserver observes texture lifetime
 */
class TextureObserver
{
public:
  /**
   * Called when a texture is discarded. This occurs in the render thread.
   * @param[in] textureId The resource id of the discarded texture
   */
  virtual void TextureDiscarded( unsigned int textureId ) = 0;

protected:
  /**
   * Virtual destructor
   */
  virtual ~TextureObserver(){}
};

}//Internal
}//Dali

#endif // __DALI_INTERNAL_TEXTURE_OBSERVER_H__
