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
#include <dali/internal/event/resources/image-ticket.h>
#include <dali/public-api/images/image-attributes.h>

using namespace std;

namespace Dali
{

namespace Internal
{

ImageTicket::ImageTicket( ResourceTicketLifetimeObserver& observer, unsigned int id, ResourceTypePath& typePath )
: ResourceTicket( observer, id, typePath )
{
}

int ImageTicket::GetWidth() const
{
  return mAttributes.GetWidth();
}

int ImageTicket::GetHeight() const
{
  return mAttributes.GetHeight();
}

} // namespace Internal

} // namespace Dali

