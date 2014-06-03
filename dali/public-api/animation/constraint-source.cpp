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
#include <dali/public-api/animation/constraint-source.h>

namespace Dali
{

LocalSource::LocalSource( Property::Index index )
: propertyIndex( index )
{
}

ParentSource::ParentSource( Property::Index index )
: propertyIndex( index )
{
}

Source::Source( Handle& obj, Property::Index index )
: propertyIndex( index ),
  object( obj )
{
}

ConstraintSource::ConstraintSource( Source source )
: sourceType( OBJECT_PROPERTY ),
  propertyIndex( source.propertyIndex ),
  object( source.object )
{
}

ConstraintSource::ConstraintSource( LocalSource local )
: sourceType( LOCAL_PROPERTY ),
  propertyIndex( local.propertyIndex )
{
}

ConstraintSource::ConstraintSource( ParentSource parent )
: sourceType( PARENT_PROPERTY ),
  propertyIndex( parent.propertyIndex )
{
}

} // namespace Dali
