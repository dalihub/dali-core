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

// CLASS HEADER
#include <dali/devel-api/update/update-proxy.h>

// INTERNAL INCLUDES
#include <dali/internal/update/manager/update-proxy-impl.h>

namespace Dali
{

Vector3 UpdateProxy::GetPosition( unsigned int id ) const
{
  return mImpl.GetPosition( id );
}

void UpdateProxy::SetPosition( unsigned int id, const Vector3& position )
{
  mImpl.SetPosition( id, position );
}

void UpdateProxy::BakePosition( unsigned int id, const Vector3& position )
{
  mImpl.BakePosition( id, position );
}

const Vector3& UpdateProxy::GetSize( unsigned int id ) const
{
  return mImpl.GetSize( id );
}

void UpdateProxy::SetSize( unsigned int id, const Vector3& size )
{
  mImpl.SetSize( id, size );
}

void UpdateProxy::BakeSize( unsigned int id, const Vector3& size )
{
  mImpl.BakeSize( id, size );
}

void UpdateProxy::GetPositionAndSize( unsigned int id, Vector3& position, Vector3& size ) const
{
  mImpl.GetPositionAndSize( id, position, size );
}

Vector3 UpdateProxy::GetScale( unsigned int id ) const
{
  return mImpl.GetScale( id );
}

void UpdateProxy::SetScale( unsigned int id, const Vector3& scale )
{
  mImpl.SetScale( id, scale );
}

void UpdateProxy::BakeScale( unsigned int id, const Vector3& scale )
{
  mImpl.BakeScale( id, scale );
}

Vector4 UpdateProxy::GetColor( unsigned int id ) const
{
  return mImpl.GetColor( id );
}

void UpdateProxy::SetColor( unsigned int id, const Vector4& color ) const
{
  mImpl.SetColor( id, color );
}

void UpdateProxy::BakeColor( unsigned int id, const Vector4& color ) const
{
  mImpl.BakeColor( id, color );
}

UpdateProxy::UpdateProxy( Internal::UpdateProxy& impl )
: mImpl( impl )
{
}

UpdateProxy::~UpdateProxy()
{
}

} // namespace Dali
