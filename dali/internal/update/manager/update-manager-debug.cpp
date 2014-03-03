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
#include <dali/internal/update/manager/update-manager-debug.h>

// EXTERNAL INCLUDES
#include <sstream>
#include <iomanip>
#include <ios>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/degree.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

#if defined(DEBUG_ENABLED)
static Debug::Filter* gNodeLogFilter = Debug::Filter::New(Debug::Verbose, false, "LOG_UPDATE_MANAGER");
#endif

/**
 * Debug helper function.
 */
void PrintNodes( const Node& node, BufferIndex updateBufferIndex, int level )
{
#if defined(DEBUG_ENABLED)
  const Vector3& position = node.GetPosition(updateBufferIndex);
  const Vector3& scale = node.GetScale(updateBufferIndex);
  const Vector3& fullPos = node.GetWorldPosition(updateBufferIndex);
  const Quaternion& rotation = node.GetRotation(updateBufferIndex);
  Vector3 axis;
  float angle;
  rotation.ToAxisAngle(axis, angle);
  angle = angle * 180.0f / Math::PI;

  std::string nodeName= DALI_LOG_GET_OBJECT_STRING((&node));

  {
    std::ostringstream oss;
    std::ios_base::fmtflags mask = oss.flags();
    mask &= ~std::ios_base::scientific;
    mask |=  std::ios_base::fixed;
    oss << std::setprecision(2) << std::setiosflags(mask)
        << std::setw(level*2) << std::setfill(' ') << "";
    oss << "Node "  << nodeName << " " << &node
        << "  Pos (" << position.x << ", " << position.y << ", " << position.z << ")"
        << "  FullPos (" << fullPos.x << ", " << fullPos.y << ", " << fullPos.z << ")"
        << "  Rot (" << angle << "deg <" << axis.x << ", " << axis.y << ", " << axis.z << ">)"
        << "  Scale (" << scale.x << ", " << scale.y << ", " << scale.z << ")"
        << std::endl;

    DALI_LOG_INFO(gNodeLogFilter, Debug::Verbose, "%s", oss.str().c_str());
  }

  {
    std::ostringstream oss;
    std::ios_base::fmtflags mask = oss.flags();
    mask &= ~std::ios_base::scientific;
    mask |=  std::ios_base::fixed;
    oss << std::setprecision(2) << std::setiosflags(mask)
        << std::setw(level*2) << std::setfill(' ') << "";

    std::string trafoMatrix = Debug::MatrixToString(node.GetWorldMatrix(updateBufferIndex), 2, level*2);
    DALI_LOG_INFO(gNodeLogFilter, Debug::Verbose, "%s", trafoMatrix.c_str());
  }

  ++level;

  for ( NodeConstIter iter = node.GetChildren().Begin(); iter != node.GetChildren().End(); ++iter )
  {
    PrintNodes(**iter, updateBufferIndex, level);
  }
#endif // DEBUG_ENABLED
}

void PrintNodeTree( const Node& node, BufferIndex bufferIndex, std::string indentation )
{
#if defined(DEBUG_ENABLED)

  std::cout << "Node " << &node
            << " \"" << node.mDebugString << "\""
            << " Origin: "       << node.GetParentOrigin()
            << " Anchor: "       << node.GetAnchorPoint()
            << " Size: "         << node.GetSize(bufferIndex)
            << " Pos: "          << node.GetPosition(bufferIndex)
            << " Rot: "          << node.GetRotation(bufferIndex)
            << " Scale: "        << node.GetScale(bufferIndex)
            << " Color: "        << node.GetColor(bufferIndex)
            << " Visible: "      << node.IsVisible(bufferIndex)
            << " World Pos: "    << node.GetWorldPosition(bufferIndex)
            << " World Rot: "    << node.GetWorldRotation(bufferIndex)
            << " World Scale: "  << node.GetWorldScale(bufferIndex)
            << " World Color: "  << node.GetWorldColor(bufferIndex)
            << " World Matrix: " << node.GetWorldMatrix(bufferIndex)
            << std::endl;

  for ( NodeConstIter iter = node.GetChildren().Begin(); iter != node.GetChildren().End(); ++iter)
  {
    std::cout << indentation << "|" << std::endl
              << indentation << "---->";

    std::string nextIndent = indentation;
    if ( (iter + 1) != node.GetChildren().End() )
    {
      nextIndent += "|    ";
    }
    else
    {
      nextIndent += "     ";
    }

    PrintNodeTree(**iter, bufferIndex, nextIndent);
  }

#endif // DEBUG_ENABLED
}

} // SceneGraph

} // Internal

} // Dali
