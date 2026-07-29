#pragma once

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
 */

/**
 * @brief Toolchain-dependent compile-time constants shared by public-api types.
 *
 * These values must be derived from compiler-intrinsic macros (not DALi build
 * flags), because public-api headers are also compiled by client applications:
 * the value must be identical in the DALi libraries and in every consumer, or a
 * type that embeds the value (e.g. Dali::String) breaks its binary layout.
 */

/**
 * @brief Inline storage size, in bytes, for the std::string embedded in Dali::String.
 *
 * The MSVC Standard Library enables iterator debugging with the debug CRT (/MDd),
 * which adds a container-proxy pointer and grows sizeof(std::string) from 32 to
 * 40 bytes on 64-bit Windows. Win32 Debug remains within 32 bytes. The size is
 * keyed off compiler-intrinsic target macros and _DEBUG to stay consistent across
 * the library build and every client translation unit. All other toolchains
 * (Tizen/Ubuntu libstdc++, MinGW, MSVC release) use 32 bytes.
 */
#if defined(_MSC_VER) && defined(_DEBUG) && defined(_WIN64)
#define DALI_STRING_STORAGE_SIZE 48
#else
#define DALI_STRING_STORAGE_SIZE 32
#endif
