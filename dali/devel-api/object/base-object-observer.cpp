/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/object/base-object-observer.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/base-object-impl.h>

namespace Dali
{
class BaseObjectObserver::Impl : public BaseObject::Impl::Observer
{
public:
  explicit Impl(const BaseHandle& handle)
  {
    if(handle)
    {
      mObject = static_cast<Dali::BaseObject*>(handle.GetObjectPtr());
    }
  }

  ~Impl() override
  {
    Stop();
  }

  void Start(BaseObjectObserver* observerBase)
  {
    if(mObject && !mObserverBase && observerBase)
    {
      mObserverBase = observerBase;
      BaseObject::Impl::Get(*mObject).AddObserver(*this);
    }
  }

  void Stop()
  {
    if(mObject && mObserverBase)
    {
      BaseObject::Impl::Get(*mObject).RemoveObserver(*this);
      mObserverBase = nullptr;
    }
  }

  /**
   * From BaseObject::Impl::Observer
   */
  void ObjectDestroyed(BaseObject&) override
  {
    mObject = nullptr;
    mObserverBase->ObjectDestroyed();
  }

private:
  Dali::BaseObject*   mObject{nullptr};
  BaseObjectObserver* mObserverBase{nullptr};
};

BaseObjectObserver::BaseObjectObserver(const BaseHandle& handle)
: mImpl{std::make_unique<BaseObjectObserver::Impl>(handle)}
{
}

BaseObjectObserver::~BaseObjectObserver() = default;

void BaseObjectObserver::StartObservingDestruction()
{
  mImpl->Start(this);
}

void BaseObjectObserver::StopObservingDestruction()
{
  mImpl->Stop();
}

} // namespace Dali
