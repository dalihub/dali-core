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
#include <dali/internal/event/actors/light-actor-impl.h>

// INTERNAL HEADER
#include <dali/public-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/property-index-ranges.h>

namespace Dali
{

const Property::Index LightActor::LIGHT_TYPE              = DEFAULT_ACTOR_PROPERTY_MAX_COUNT;
const Property::Index LightActor::ENABLE                  = DEFAULT_ACTOR_PROPERTY_MAX_COUNT + 1;
const Property::Index LightActor::FALL_OFF                = DEFAULT_ACTOR_PROPERTY_MAX_COUNT + 2;
const Property::Index LightActor::SPOT_ANGLE              = DEFAULT_ACTOR_PROPERTY_MAX_COUNT + 3;
const Property::Index LightActor::AMBIENT_COLOR           = DEFAULT_ACTOR_PROPERTY_MAX_COUNT + 4;
const Property::Index LightActor::DIFFUSE_COLOR           = DEFAULT_ACTOR_PROPERTY_MAX_COUNT + 5;
const Property::Index LightActor::SPECULAR_COLOR          = DEFAULT_ACTOR_PROPERTY_MAX_COUNT + 6;
const Property::Index LightActor::DIRECTION               = DEFAULT_ACTOR_PROPERTY_MAX_COUNT + 7;

namespace Internal
{
bool LightActor::mFirstInstance = true ;
Actor::DefaultPropertyLookup* LightActor::mDefaultLightActorPropertyLookup = NULL;

namespace
{

BaseHandle Create()
{
  return Dali::LightActor::New();
}

TypeRegistration mType( typeid(Dali::LightActor), typeid(Dali::Actor), Create );

const std::string DEFAULT_LIGHT_ACTOR_PROPERTY_NAMES[] =
{
  "light-type",
  "enable",
  "fall-off",
  "spot-angle",
  "ambient-color",
  "diffuse-color",
  "specular-color",
  "direction"
};
const int DEFAULT_LIGHT_ACTOR_PROPERTY_COUNT = sizeof( DEFAULT_LIGHT_ACTOR_PROPERTY_NAMES ) / sizeof( std::string );

const Property::Type DEFAULT_LIGHT_ACTOR_PROPERTY_TYPES[DEFAULT_LIGHT_ACTOR_PROPERTY_COUNT] =
{
  Property::STRING,  // "light-type",
  Property::VECTOR2, // "enable",
  Property::VECTOR2, // "fall-off",
  Property::VECTOR2, // "spot-angle",
  Property::VECTOR3, // "ambient-color",
  Property::VECTOR3, // "diffuse-color",
  Property::VECTOR3, // "specular-color",
  Property::VECTOR3, // "direction",
};


LightType LightTypeEnum(const std::string &stringValue)
{
  LightType ret(AMBIENT);

  if(stringValue == "AMBIENT")
  {
    ret =  AMBIENT;
  }
  else if(stringValue == "DIRECTIONAL")
  {
    ret =  DIRECTIONAL;
  }
  else if(stringValue == "SPOT")
  {
    ret =  SPOT;
  }
  else if(stringValue == "POINT")
  {
    ret =  POINT;
  }
  else
  {
    DALI_LOG_WARNING("Unknown Light Type:%s\n", stringValue.c_str());
  }

  return ret;
}

std::string LightTypeString(const LightType type)
{
  std::string ret;

  if(type == AMBIENT)
  {
    ret = "AMBIENT" ;
  }
  else if(type == DIRECTIONAL)
  {
    ret = "DIRECTIONAL" ;
  }
  else if(type == SPOT)
  {
    ret = "SPOT" ;
  }
  else if(type == POINT)
  {
    ret = "POINT" ;
  }
  else
  {
    DALI_LOG_WARNING("Unknown Light Type:%d\n", type);
    ret = "AMBIENT";
  }

  return ret;
}


}


LightActorPtr LightActor::New()
{
  LightActorPtr actor(new LightActor());

  // Second-phase construction

  actor->Initialize();

  // Create the attachment
  actor->mLightAttachment = LightAttachment::New( *actor->mNode );
  actor->Attach(*actor->mLightAttachment);
  actor->mLightAttachment->SetName(actor->GetName());

  return actor;
}


void LightActor::OnInitialize()
{
  if(LightActor::mFirstInstance)
  {
    mDefaultLightActorPropertyLookup = new DefaultPropertyLookup();
    const int start = DEFAULT_ACTOR_PROPERTY_MAX_COUNT;
    for ( int i = 0; i < DEFAULT_LIGHT_ACTOR_PROPERTY_COUNT; ++i )
    {
      (*mDefaultLightActorPropertyLookup)[DEFAULT_LIGHT_ACTOR_PROPERTY_NAMES[i]] = i + start;
    }
    LightActor::mFirstInstance = false ;
  }
}

LightActor::LightActor()
: Actor( Actor::BASIC ),
  mIsActive(false)
{
}

LightActor::~LightActor()
{
}

void LightActor::SetLight(Dali::Light light)
{
  Internal::LightPtr lightPtr(&GetImplementation(light));
  mLightAttachment->SetLight(lightPtr);
  mLightAttachment->SetActive(true);
}

Dali::Light LightActor::GetLight() const
{
  Internal::LightPtr lightPtr(mLightAttachment->GetLight());
  return Dali::Light(lightPtr.Get());
}

void LightActor::SetActive(bool active)
{
  mLightAttachment->SetActive(active);
  mIsActive = active;
}

bool LightActor::GetActive()
{
  return mIsActive;
}

unsigned int LightActor::GetDefaultPropertyCount() const
{
  return Actor::GetDefaultPropertyCount() + DEFAULT_LIGHT_ACTOR_PROPERTY_COUNT;
}

bool LightActor::IsDefaultPropertyWritable( Property::Index index ) const
{
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    return Actor::IsDefaultPropertyWritable(index) ;
  }
  else
  {
    return true ;
  }
}

bool LightActor::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    return Actor::IsDefaultPropertyAnimatable(index) ;
  }
  else
  {
    return false ;
  }
}

Property::Type LightActor::GetDefaultPropertyType( Property::Index index ) const
{
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    return Actor::GetDefaultPropertyType(index) ;
  }
  else
  {
    // ProxyObject guarantees that index is within range
    return DEFAULT_LIGHT_ACTOR_PROPERTY_TYPES[index - DEFAULT_ACTOR_PROPERTY_MAX_COUNT];
  }
}

const std::string& LightActor::GetDefaultPropertyName( Property::Index index ) const
{
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    return Actor::GetDefaultPropertyName(index) ;
  }
  else
  {
    // ProxyObject guarantees that index is within range
    return DEFAULT_LIGHT_ACTOR_PROPERTY_NAMES[index - DEFAULT_ACTOR_PROPERTY_MAX_COUNT];
  }
}

Property::Index LightActor::GetDefaultPropertyIndex(const std::string& name) const
{
  Property::Index index = Property::INVALID_INDEX;

  DALI_ASSERT_DEBUG( NULL != mDefaultLightActorPropertyLookup );

  // Look for name in current class' default properties
  DefaultPropertyLookup::const_iterator result = mDefaultLightActorPropertyLookup->find( name );
  if ( mDefaultLightActorPropertyLookup->end() != result )
  {
    index = result->second;
  }
  else
  {
    // If not found, check in base class
    index = Actor::GetDefaultPropertyIndex( name );
  }

  return index;
}

void LightActor::SetDefaultProperty( Property::Index index, const Property::Value& propertyValue )
{
  // ProxyObject guarantees the property is writable and index is in range

  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    Actor::SetDefaultProperty(index, propertyValue) ;
  }
  else
  {
    switch(index)
    {
      case Dali::LightActor::LIGHT_TYPE:
      {
        mLightAttachment->SetType(LightTypeEnum(propertyValue.Get<std::string>()));
        break;
      }
      case Dali::LightActor::ENABLE:
      {
        SetActive(propertyValue.Get<bool>());
        break;
      }
      case Dali::LightActor::FALL_OFF:
      {
        mLightAttachment->SetFallOff(propertyValue.Get<Vector2>());
        break;
      }
      case Dali::LightActor::SPOT_ANGLE:
      {
        mLightAttachment->SetSpotAngle(propertyValue.Get<Vector2>());
        break;
      }
      case Dali::LightActor::AMBIENT_COLOR:
      {
        mLightAttachment->SetAmbientColor(propertyValue.Get<Vector3>());
        break;
      }
      case Dali::LightActor::DIFFUSE_COLOR:
      {
        mLightAttachment->SetDiffuseColor(propertyValue.Get<Vector3>());
        break;
      }
      case Dali::LightActor::SPECULAR_COLOR:
      {
        mLightAttachment->SetSpecularColor(propertyValue.Get<Vector3>());
        break;
      }
      case Dali::LightActor::DIRECTION:
      {
        mLightAttachment->SetDirection(propertyValue.Get<Vector3>());
        break;
      }
      default:
      {
        DALI_LOG_WARNING("Unknown property (%d)\n", index);
        break;
      }
    } // switch(index)

  } // else
}

Property::Value LightActor::GetDefaultProperty( Property::Index index ) const
{
  Property::Value ret ;
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    ret = Actor::GetDefaultProperty(index) ;
  }
  else
  {
    switch(index)
    {
      case Dali::LightActor::LIGHT_TYPE:
      {
        ret = LightTypeString(mLightAttachment->GetType());
        break;
      }
      case Dali::LightActor::ENABLE:
      {
        ret = mIsActive;
        break;
      }
      case Dali::LightActor::FALL_OFF:
      {
        ret = mLightAttachment->GetFallOff();
        break;
      }
      case Dali::LightActor::SPOT_ANGLE:
      {
        ret = mLightAttachment->GetSpotAngle();
        break;
      }
      case Dali::LightActor::AMBIENT_COLOR:
      {
        ret = mLightAttachment->GetAmbientColor();
        break;
      }
      case Dali::LightActor::DIFFUSE_COLOR:
      {
        ret = mLightAttachment->GetDiffuseColor();
        break;
      }
      case Dali::LightActor::SPECULAR_COLOR:
      {
        ret = mLightAttachment->GetSpecularColor();
        break;
      }
      case Dali::LightActor::DIRECTION:
      {
        ret = mLightAttachment->GetDirection();
        break;
      }
      default:
      {
        DALI_LOG_WARNING("Unknown property (%d)\n", index);
        break;
      }
    } // switch(index)
  }

  return ret ;
}


} // namespace Internal

} // namespace Dali
