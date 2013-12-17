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
#include <dali/internal/event/modeling/model-factory.h>

// INTERNAL INCLUDES
#include <dali/internal/event/resources/resource-client.h>

using namespace Dali::Integration;

namespace Dali
{

namespace Internal
{

ModelFactory::ModelFactory( ResourceClient& resourceClient )
: mResourceClient( resourceClient )
{
}

ModelFactory::~ModelFactory()
{
}

ResourceTicketPtr ModelFactory::Load( const std::string& filename )
{
  ResourceTicketPtr ticket;
  ModelResourceType modelResourceType; // construct first as no copy ctor (needed to bind ref to object)
  ResourceTypePath typePath(modelResourceType, filename);

  // Search for a matching resource
  ResourceTypePathIdIter iter = mResourceTypePathIdMap.end();
  if ( !mResourceTypePathIdMap.empty() )
  {
    iter = mResourceTypePathIdMap.find( typePath );
  }

  if ( mResourceTypePathIdMap.end() != iter )
  {
    // The resource was previously requested
    unsigned int resourceId = iter->second;

    // The resource may still be alive; share the ticket
    ticket = mResourceClient.RequestResourceTicket( resourceId );

    // Clean-up the map of resource IDs, if the ticket has been discarded
    if ( !ticket )
    {
      mResourceTypePathIdMap.erase( iter );
    }
  }

  // Request a new model resource, if necessary
  if ( !ticket )
  {
    ModelResourceType modelResourceType; // construct first as no copy ctor (needed to bind ref to object)
    ticket = mResourceClient.RequestResource(modelResourceType, filename);

    mResourceTypePathIdMap.insert( ResourceTypePathIdPair( typePath, ticket->GetId() ) );
  }

  return ticket;
}

} // namespace Internal

} // namespace Dali
