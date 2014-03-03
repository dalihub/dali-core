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
#include <dali/public-api/actors/blending.h>

namespace Dali
{

const BlendingFactor::Type   DEFAULT_BLENDING_SRC_FACTOR_RGB(    BlendingFactor::SRC_ALPHA );
const BlendingFactor::Type   DEFAULT_BLENDING_DEST_FACTOR_RGB(   BlendingFactor::ONE_MINUS_SRC_ALPHA );
const BlendingFactor::Type   DEFAULT_BLENDING_SRC_FACTOR_ALPHA(  BlendingFactor::ONE );
const BlendingFactor::Type   DEFAULT_BLENDING_DEST_FACTOR_ALPHA( BlendingFactor::ONE_MINUS_SRC_ALPHA );

const BlendingEquation::Type DEFAULT_BLENDING_EQUATION_RGB(   BlendingEquation::ADD );
const BlendingEquation::Type DEFAULT_BLENDING_EQUATION_ALPHA( BlendingEquation::ADD );

} // namespace Dali
