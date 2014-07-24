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
#include <dali/internal/event/common/property-notification-manager.h>
#include <dali/internal/event/common/property-notification-impl.h>

namespace Dali
{

namespace Internal
{

PropertyNotificationManager* PropertyNotificationManager::New()
{
  return new PropertyNotificationManager;
}

PropertyNotificationManager::~PropertyNotificationManager()
{
}

void PropertyNotificationManager::PropertyNotificationCreated( PropertyNotification& propertyNotification )
{
  mPropertyNotifications.insert( &propertyNotification );
}

void PropertyNotificationManager::PropertyNotificationDestroyed( PropertyNotification& propertyNotification )
{
  std::set< PropertyNotification* >::iterator iter = std::find( mPropertyNotifications.begin(), mPropertyNotifications.end(), &propertyNotification );
  DALI_ASSERT_ALWAYS( iter != mPropertyNotifications.end() && "PropertyNotification not found" );

  mPropertyNotifications.erase( iter );
}

void PropertyNotificationManager::NotifyProperty( SceneGraph::PropertyNotification* propertyNotification, bool validity )
{
  std::set< PropertyNotification* >::iterator iter = mPropertyNotifications.begin();
  std::set< PropertyNotification* >::iterator endIter = mPropertyNotifications.end();

  // walk the collection of PropertyNotifications
  for( ; iter != endIter; ++iter )
  {
    // found one with the matching SceneGraph::PropertyNotification?
    if( (*iter)->CompareSceneObject( propertyNotification ) )
    {
      // allow application to access the value that triggered this emit incase of NotifyOnChanged mode
      (*iter)->SetNotifyResult(validity);
      // yes..emit signal
      (*iter)->EmitSignalNotify();
      break;
    }
  }
}

PropertyNotificationManager::PropertyNotificationManager()
{
}

} // namespace Internal

} // namespace Dali

