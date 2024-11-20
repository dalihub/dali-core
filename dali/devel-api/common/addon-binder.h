#ifndef DALI_ADDON_BINDER_H
#define DALI_ADDON_BINDER_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
 */

#include <dali/integration-api/addon-manager.h>

namespace Dali
{
namespace AddOn
{
/**
 * Class automates binding an AddOn interface.
 *
 * This interface is meant to be used as a base
 * for classes that use function binding macros.
 *
 * Sample use:
 * \code{.cpp}
 * #include <dali/devel-api/common/addon-binder.h>
 * struct AddOnImageLoader : public Dali::DevelAddOn::AddOnBinder
 * {
 *   // Constructor requires the AddOn name and optional version (0 - any version accepted)
 *   AddOnImageLoader( const char* addonname ) : Dali::DevelAddOn::AddOnBinder( addonname, 0 ) {}
 *
 *   ~AddOnImageLoader() = default;
 *
 *   // Binding block
 *   // Using ADDON_BIND_FUNCTION() macro requires function name (resolved by the AddOn) and function
 *   // signature. It will generate member function with correct binding.
 *
 *   // LoadBitmap
 *   ADDON_BIND_FUNCTION( LoadBitmap, bool(const Dali::ImageLoader::Input&, Dali::Devel::PixelBuffer&) );
 *
 *   // LoadHeader
 *   ADDON_BIND_FUNCTION( LoadHeader, bool(const Dali::ImageLoader::Input&, unsigned int&, unsigned int&) );
 *
 *   // GetFormatExtension
 *   ADDON_BIND_FUNCTION( GetFormatExtension, const char*() );
 *
 *   // GetFormatMagicNumber
 *   ADDON_BIND_FUNCTION( GetFormatMagicNumber, uint16_t() );
 *
 *   // GetBitmapProfile
 *   ADDON_BIND_FUNCTION( GetBitmapProfile, Bitmap::Profile() );
 *  };
 *  \endcode
 */

class AddOnBinder
{
public:
  /**
   * @brief Constructor. Opens an AddOn and creates interface
   * @param[in] addonName Name of AddOn
   * @param[in] version Version of AddOn
   */
  explicit AddOnBinder(const char* addonName, uint32_t version = 0u)
  {
    mAddOnManager = Dali::Integration::AddOnManager::Get();
    if(mAddOnManager)
    {
      mAddOnHandle = mAddOnManager->GetAddOn(addonName);
      if(mAddOnHandle)
      {
        mAddOnManager->GetAddOnInfo(addonName, mAddOnInfo);
      }
    }
  }

  /**
   * @brief Constructor. Opens an AddOn and creates interface
   * @param[in] addonName Name of AddOn
   * @param[in] libraryName Name of the library to load addon from.
   * @param[in] version Version of AddOn
   */
  explicit AddOnBinder(const char* addonName, const char* libraryName, uint32_t version = 0u)
  {
    mAddOnManager = Dali::Integration::AddOnManager::Get();
    if(mAddOnManager)
    {
      mAddOnHandle = mAddOnManager->LoadAddOn(addonName, libraryName);
      if(mAddOnHandle)
      {
        mAddOnManager->GetAddOnInfo(addonName, mAddOnInfo);
      }
    }
  }

  /**
   * @brief Destructor
   */
  virtual ~AddOnBinder() = default;

  /**
   * @brief Looks up and converts c-style void* function into pointer of type T
   * @param[in] funcName name of the function
   * @return Returns a new pointer
   */
  template<class T>
  T* ConvertFunction(const std::string& funcName)
  {
    if(mAddOnHandle)
    {
      auto ptr = mAddOnManager->GetGlobalProc(mAddOnHandle, funcName.c_str());
      return *reinterpret_cast<T**>(&ptr);
    }
    return nullptr;
  }

  /**
   * @brief Returns a handle to the AddOn library
   * @return Handle object
   */
  Dali::AddOnLibrary GetHandle()
  {
    return mAddOnHandle;
  }

  /**
   * @brief Returns pointer to the global AddOn function.
   * @param[in] name Name of the function
   * @return Valid pointer or nullptr
   */
  void* GetGlobalProc(const char* name)
  {
    return mAddOnManager ? mAddOnManager->GetGlobalProc(mAddOnHandle, name) : nullptr;
  }

  /**
   * @brief Returns pointer to the instance AddOn function.
   * @param[in] name Name of the function
   * @return Valid pointer or nullptr
   */
  void* GetInstanceProc(const char* name)
  {
    return mAddOnManager ? mAddOnManager->GetInstanceProc(mAddOnHandle, name) : nullptr;
  }

  /**
   * @brief Tests whether the interface is valid
   * @return True if valid, false otherwise
   */
  virtual bool IsValid()
  {
    return GetHandle() != nullptr;
  }

  /**
   * @brief Returns AddOn info structure
   * @return AddOn info structure.
   */
  const AddOnInfo& GetAddOnInfo() const
  {
    return mAddOnInfo;
  }

protected:
  static DALI_CORE_API Dali::Integration::AddOnManager* mAddOnManager; ///< Pointer to the AddOn manager

  Dali::AddOnLibrary mAddOnHandle{nullptr}; ///< Handle to the AddOn library
  Dali::AddOnInfo    mAddOnInfo{};          ///< Stored AddOnInfo structure
};

/**
 * Macro binds function as a member function of the class, for example, the call:
 *
 * ADDON_BIND_FUNCTION( SomeAddOnFunction, void(int, char*) );
 *
 * will create a std::function object named SomeAddOnFunction and bound to the AddOn library.
 */
#define ADDON_BIND_FUNCTION(FUNCNAME, FUNCTYPE) \
  std::function<FUNCTYPE> FUNCNAME{ConvertFunction<FUNCTYPE>(std::string(#FUNCNAME))};

} // namespace AddOn
} // namespace Dali

#endif // DALI_ADDON_BINDER_H
