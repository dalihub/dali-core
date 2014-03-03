//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/public-api/scripting/scripting.h>

// INTERNAL INCLUDES
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/image-attributes.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/internal/event/images/frame-buffer-image-impl.h>
#include <dali/internal/event/images/bitmap-image-impl.h>
#include <dali/internal/event/effects/shader-effect-impl.h>

namespace Dali
{

namespace Scripting
{

bool CompareEnums(const std::string& a, const std::string& b)
{
  std::string::const_iterator ia = a.begin();
  std::string::const_iterator ib = b.begin();

  while( (ia != a.end()) && (ib != b.end()) )
  {
    char ca = *ia;
    char cb = *ib;

    if(ca == '-' || ca == '_')
    {
      ++ia;
      continue;
    }

    if(cb == '-' || cb == '_')
    {
      ++ib;
      continue;
    }

    if( 'A' <= ca && ca <= 'Z')
    {
      ca = ca + ('a' - 'A');
    }

    if( 'A' <= cb && cb <= 'Z')
    {
      cb = cb + ('a' - 'A');
    }

    if( ca != cb )
    {
      return false;
    }

    ++ia;
    ++ib;
  }

  if( (ia == a.end() && ib == b.end() ) )
  {
    return true;
  }
  else
  {
    return false;
  }

}


Image NewImage( const Property::Value& map )
{
  Image ret;

  std::string filename;
  Image::LoadPolicy loadPolicy       = Dali::Internal::ImageLoadPolicyDefault;
  Image::ReleasePolicy releasePolicy = Dali::Internal::ImageReleasePolicyDefault;
  ImageAttributes attributes         = ImageAttributes::New();

  if( Property::MAP == map.GetType() )
  {
    std::string field = "filename";
    if( map.HasKey(field) )
    {
      DALI_ASSERT_ALWAYS(map.GetValue(field).GetType() == Property::STRING && "Image filename property is not a string" );
      filename = map.GetValue(field).Get<std::string>();
    }

    field = "load-policy";
    if( map.HasKey(field) )
    {
      DALI_ASSERT_ALWAYS(map.GetValue(field).GetType() == Property::STRING && "Image load-policy property is not a string" );
      std::string v(map.GetValue(field).Get<std::string>());
      if(v == "IMMEDIATE")
      {
        loadPolicy = Dali::Image::Immediate;
      }
      else if(v == "ON_DEMAND")
      {
        loadPolicy = Dali::Image::OnDemand;
      }
      else
      {
        DALI_ASSERT_ALWAYS(!"Unknown image load-policy");
      }
    }

    field = "release-policy";
    if( map.HasKey(field) )
    {
      DALI_ASSERT_ALWAYS(map.GetValue(field).GetType() == Property::STRING && "Image release-policy property is not a string" );
      std::string v(map.GetValue(field).Get<std::string>());
      if(v == "UNUSED")
      {
        releasePolicy = Dali::Image::Unused;
      }
      else if(v == "NEVER")
      {
        releasePolicy = Dali::Image::Never;
      }
      else
      {
        DALI_ASSERT_ALWAYS(!"Unknown image release-policy");
      }
    }

    if( map.HasKey("width") && map.HasKey("height") )
    {
      Property::Value &value = map.GetValue("width");
      unsigned int w = 0, h = 0;
      // handle floats and integer the same for json script
      if( value.GetType() == Property::FLOAT)
      {
        w = static_cast<unsigned int>( value.Get<float>() );
      }
      else
      {
        DALI_ASSERT_ALWAYS(value.GetType() == Property::INTEGER && "Image width property is not a number" );
        w = value.Get<int>();
      }

      value = map.GetValue("height");
      if( value.GetType() == Property::FLOAT)
      {
        h = static_cast<unsigned int>( value.Get<float>() );
      }
      else
      {
        DALI_ASSERT_ALWAYS(value.GetType() == Property::INTEGER && "Image width property is not a number" );
        h = value.Get<int>();
      }

      attributes.SetSize( w, h );
    }

    field = "pixel-format";
    if( map.HasKey(field) )
    {
      DALI_ASSERT_ALWAYS(map.GetValue(field).GetType() == Property::STRING && "Image release-policy property is not a string" );

      std::string s(map.GetValue(field).Get<std::string>());

      if(s == "A8")
      {
        attributes.SetPixelFormat(Pixel::A8);
      }
      else if(s == "L8")
      {
        attributes.SetPixelFormat(Pixel::L8);
      }
      else if(s == "LA88")
      {
        attributes.SetPixelFormat(Pixel::LA88);
      }
      else if(s == "RGB565")
      {
        attributes.SetPixelFormat(Pixel::RGB565);
      }
      else if(s == "BGR565")
      {
        attributes.SetPixelFormat(Pixel::BGR565);
      }
      else if(s == "RGBA4444")
      {
        attributes.SetPixelFormat(Pixel::RGBA4444);
      }
      else if(s == "BGRA4444")
      {
        attributes.SetPixelFormat(Pixel::BGRA4444);
      }
      else if(s == "RGBA5551")
      {
        attributes.SetPixelFormat(Pixel::RGBA5551);
      }
      else if(s == "BGRA5551")
      {
        attributes.SetPixelFormat(Pixel::BGRA5551);
      }
      else if(s == "RGB888")
      {
        attributes.SetPixelFormat(Pixel::RGB888);
      }
      else if(s == "RGB8888")
      {
        attributes.SetPixelFormat(Pixel::RGB8888);
      }
      else if(s == "BGR8888")
      {
        attributes.SetPixelFormat(Pixel::BGR8888);
      }
      else if(s == "RGBA8888")
      {
        attributes.SetPixelFormat(Pixel::RGBA8888);
      }
      else if(s == "BGRA8888")
      {
        attributes.SetPixelFormat(Pixel::BGRA8888);
      }
      else
      {
        DALI_ASSERT_ALWAYS(!"Unknown image pixel format");
      }
    }

    field = "scaling-mode";
    if( map.HasKey(field) )
    {
      DALI_ASSERT_ALWAYS(map.GetValue(field).GetType() == Property::STRING && "Image release-policy property is not a string" );

      std::string s(map.GetValue(field).Get<std::string>());

      if(s == "SHRINK_TO_FIT")
      {
        attributes.SetScalingMode(ImageAttributes::ShrinkToFit);
      }
      else if(s == "SCALE_TO_FILL")
      {
        attributes.SetScalingMode(ImageAttributes::ScaleToFill);
      }
      else if(s == "FIT_WIDTH")
      {
        attributes.SetScalingMode(ImageAttributes::FitWidth);
      }
      else if(s == "FIT_HEIGHT")
      {
        attributes.SetScalingMode(ImageAttributes::FitHeight);
      }
      else
      {
        DALI_ASSERT_ALWAYS(!"Unknown image scaling mode");
      }
    }

    field = "crop";
    if( map.HasKey(field) )
    {
      DALI_ASSERT_ALWAYS(map.GetValue(field).GetType() == Property::VECTOR4 && "Image release-policy property is not a string" );
      Vector4 v(map.GetValue(field).Get<Vector4>());
      attributes.SetCrop( Rect<float>(v.x, v.y, v.z, v.w) );
    }

    if( map.HasKey("type") )
    {
      DALI_ASSERT_ALWAYS( map.GetValue("type").GetType() == Property::STRING );
      std::string s(map.GetValue("type").Get<std::string>());
      if("FrameBufferImage" == s)
      {
        ret = new Internal::FrameBufferImage(attributes.GetWidth(),
                                             attributes.GetHeight(),
                                             attributes.GetPixelFormat(),
                                             releasePolicy );
      }
      else if("BitMapImage" == s)
      {
        ret = new Internal::BitmapImage(attributes.GetWidth(),
                                        attributes.GetHeight(),
                                        attributes.GetPixelFormat(),
                                        loadPolicy,
                                        releasePolicy);
      }
      else if("Image" == s)
      {
        ret = Image::New(filename, attributes, loadPolicy, releasePolicy);
      }
    }
    else
    {
      ret = Image::New(filename, attributes, loadPolicy, releasePolicy);
    }
  }

  return ret;

} // Image NewImage( Property::Value map )


ShaderEffect NewShaderEffect( const Property::Value& map )
{
  Internal::ShaderEffectPtr ret;

  if( map.GetType() == Property::MAP )
  {
    ret = Internal::ShaderEffect::New(Dali::ShaderEffect::HINT_NONE); // hint can be reset if in map

    if( map.HasKey("program") )
    {
      Property::Index index = ret->GetPropertyIndex("program");
      DALI_ASSERT_DEBUG( map.GetValue("program").GetType() == Property::MAP );
      ret->SetProperty(index, map.GetValue("program"));
    }

    for(int i = 0; i < map.GetSize(); ++i)
    {
      const std::string& key = map.GetKey(i);
      if(key != "program")
      {
        Property::Index index = ret->GetPropertyIndex( key );

        if( Property::INVALID_INDEX != index )
        {
          ret->SetProperty(index, map.GetItem(i));
        }
        else
        {
          // if its not a property then register it as a uniform (making a custom property)

          if(map.GetItem(i).GetType() == Property::INTEGER)
          {
            // valid uniforms are floats, vec3's etc so we recast if the user accidentally
            // set as integer. Note the map could have come from json script.
            Property::Value asFloat( static_cast<float>( map.GetItem(i).Get<int>() ) );
            ret->SetUniform( key, asFloat, Dali::ShaderEffect::COORDINATE_TYPE_DEFAULT );
          }
          else
          {
            ret->SetUniform( key, map.GetItem(i), Dali::ShaderEffect::COORDINATE_TYPE_DEFAULT );
          }
        }
      }
    }
  }

  return Dali::ShaderEffect(ret.Get());
}

} // namespace scripting

} // namespace Dali




