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

const Vector3& UpdateProxy::GetSize( unsigned int id ) const
{
  return mImpl.GetSize( id );
}

void UpdateProxy::SetSize( unsigned int id, const Vector3& size )
{
  mImpl.SetSize( id, size );
}

void UpdateProxy::GetPositionAndSize( unsigned int id, Vector3& position, Vector3& size ) const
{
  mImpl.GetPositionAndSize( id, position, size );
}

Vector4 UpdateProxy::GetWorldColor( unsigned int id ) const
{
  return mImpl.GetWorldColor( id );
}

void UpdateProxy::SetWorldColor( unsigned int id, const Vector4& color ) const
{
  mImpl.SetWorldColor( id, color );
}

void UpdateProxy::GetWorldMatrixAndSize( unsigned int id, Matrix& worldMatrix, Vector3& size ) const
{
  mImpl.GetWorldMatrixAndSize( id, worldMatrix, size );
}

const Matrix& UpdateProxy::GetWorldMatrix( unsigned int id ) const
{
  return mImpl.GetWorldMatrix( id );
}

void UpdateProxy::SetWorldMatrix( unsigned int id, const Matrix& worldMatrix )
{
  mImpl.SetWorldMatrix( id, worldMatrix );
}

UpdateProxy::UpdateProxy( Internal::UpdateProxy& impl )
: mImpl( impl )
{
}

UpdateProxy::~UpdateProxy()
{
}

} // namespace Dali
