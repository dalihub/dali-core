#ifndef DALI_INTERNAL_GESTURE_DEVICE_PROFILE_TABLE_H
#define DALI_INTERNAL_GESTURE_DEVICE_PROFILE_TABLE_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <vector>

// INTERNAL INCLUDES
#include <dali/internal/event/events/gesture-input-source.h>
#include <dali/public-api/events/gesture-device-selector.h>

namespace DALI_NAMESPACE
{
namespace Internal
{
/**
 * @brief Per-device gesture profiles keyed by GestureDeviceSelector.
 *
 * Holds at most one profile per selector and resolves the profile for an input source in the
 * order Name > Class+Subclass > Class. A miss returns nullptr so the caller applies its default.
 * Entries are kept in three small vectors, one per match type; the number of entries an
 * application registers is expected to be in the single digits.
 *
 * @tparam OptionsT The profile type. Must be copyable.
 */
template<typename OptionsT>
class GestureDeviceProfileTable
{
public:
  /**
   * @brief Registers a profile for the selector, replacing any existing profile for the same selector.
   * @param[in] selector The devices the profile applies to
   * @param[in] options  The profile. Copied.
   */
  void Set(const GestureDeviceSelector& selector, const OptionsT& options)
  {
    Entries& entries = Bucket(selector.GetMatchType());
    for(Entry& entry : entries)
    {
      if(entry.selector == selector)
      {
        entry.options = options;
        return;
      }
    }
    entries.push_back(Entry{selector, options});
  }

  /**
   * @brief Finds the profile registered for exactly this selector.
   * @param[in] selector The selector
   * @return The profile, or nullptr if none is registered for the selector
   */
  const OptionsT* Find(const GestureDeviceSelector& selector) const
  {
    const Entries& entries = Bucket(selector.GetMatchType());
    for(const Entry& entry : entries)
    {
      if(entry.selector == selector)
      {
        return &entry.options;
      }
    }
    return nullptr;
  }

  /**
   * @brief Removes the profile registered for exactly this selector.
   * @param[in] selector The selector
   * @return true if a profile was removed
   */
  bool Clear(const GestureDeviceSelector& selector)
  {
    Entries& entries = Bucket(selector.GetMatchType());
    for(auto iter = entries.begin(); iter != entries.end(); ++iter)
    {
      if(iter->selector == selector)
      {
        entries.erase(iter);
        return true;
      }
    }
    return false;
  }

  /**
   * @brief Removes every profile.
   */
  void ClearAll()
  {
    mByName.clear();
    mByClassAndSubclass.clear();
    mByClass.clear();
  }

  /**
   * @brief Checks whether no profile is registered.
   * @return true if empty
   */
  bool Empty() const
  {
    return mByName.empty() && mByClassAndSubclass.empty() && mByClass.empty();
  }

  /**
   * @brief Calls the functor for every registered profile.
   * @param[in] functor Callable taking const OptionsT&
   */
  template<typename Functor>
  void ForEach(Functor&& functor) const
  {
    for(const Entry& entry : mByName)
    {
      functor(entry.options);
    }
    for(const Entry& entry : mByClassAndSubclass)
    {
      functor(entry.options);
    }
    for(const Entry& entry : mByClass)
    {
      functor(entry.options);
    }
  }

  /**
   * @brief Resolves the profile for an input source: Name > Class+Subclass > Class.
   * @param[in] source The input source that started the gesture
   * @return The matching profile, or nullptr if no selector matches
   */
  const OptionsT* Resolve(const GestureInputSource& source) const
  {
    if(const OptionsT* options = ResolveIn(mByName, source))
    {
      return options;
    }
    if(const OptionsT* options = ResolveIn(mByClassAndSubclass, source))
    {
      return options;
    }
    return ResolveIn(mByClass, source);
  }

private:
  struct Entry
  {
    GestureDeviceSelector selector;
    OptionsT              options;
  };
  using Entries = std::vector<Entry>;

  Entries& Bucket(GestureDeviceSelector::MatchType matchType)
  {
    switch(matchType)
    {
      case GestureDeviceSelector::MatchType::DEVICE_NAME:
      {
        return mByName;
      }
      case GestureDeviceSelector::MatchType::DEVICE_CLASS_AND_SUBCLASS:
      {
        return mByClassAndSubclass;
      }
      case GestureDeviceSelector::MatchType::DEVICE_CLASS:
      default:
      {
        return mByClass;
      }
    }
  }

  const Entries& Bucket(GestureDeviceSelector::MatchType matchType) const
  {
    return const_cast<GestureDeviceProfileTable*>(this)->Bucket(matchType);
  }

  static const OptionsT* ResolveIn(const Entries& entries, const GestureInputSource& source)
  {
    for(const Entry& entry : entries)
    {
      if(source.Matches(entry.selector))
      {
        return &entry.options;
      }
    }
    return nullptr;
  }

  Entries mByName;             ///< Profiles keyed by device name.
  Entries mByClassAndSubclass; ///< Profiles keyed by class and subclass.
  Entries mByClass;            ///< Profiles keyed by class.
};

} // namespace Internal

} // namespace DALI_NAMESPACE

#endif // DALI_INTERNAL_GESTURE_DEVICE_PROFILE_TABLE_H
