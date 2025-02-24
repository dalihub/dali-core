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
 */

// CLASS HEADER
#include <dali/internal/render/renderers/pipeline-cache.h>

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-types.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/render/renderers/render-vertex-buffer.h>
#include <dali/internal/render/shaders/program.h>

namespace Dali::Internal::Render
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_PIPELINE_CACHE");
#endif

constexpr uint32_t CACHE_CLEAN_FRAME_COUNT = 600; // 60fps * 10sec

// Helper to get the vertex input format
Dali::Graphics::VertexInputFormat GetPropertyVertexFormat(Property::Type propertyType)
{
  Dali::Graphics::VertexInputFormat type{};

  switch(propertyType)
  {
    case Property::BOOLEAN:
    {
      type = Dali::Graphics::VertexInputFormat::UNDEFINED; // type = GL_BYTE; @todo new type for this?
      break;
    }
    case Property::INTEGER:
    {
      type = Dali::Graphics::VertexInputFormat::INTEGER; // (short)
      break;
    }
    case Property::FLOAT:
    {
      type = Dali::Graphics::VertexInputFormat::FLOAT;
      break;
    }
    case Property::VECTOR2:
    {
      type = Dali::Graphics::VertexInputFormat::FVECTOR2;
      break;
    }
    case Property::VECTOR3:
    {
      type = Dali::Graphics::VertexInputFormat::FVECTOR3;
      break;
    }
    case Property::VECTOR4:
    {
      type = Dali::Graphics::VertexInputFormat::FVECTOR4;
      break;
    }
    default:
    {
      type = Dali::Graphics::VertexInputFormat::UNDEFINED;
    }
  }

  return type;
}

constexpr Graphics::CullMode ConvertCullFace(Dali::FaceCullingMode::Type mode)
{
  switch(mode)
  {
    case Dali::FaceCullingMode::NONE:
    {
      return Graphics::CullMode::NONE;
    }
    case Dali::FaceCullingMode::FRONT:
    {
      return Graphics::CullMode::FRONT;
    }
    case Dali::FaceCullingMode::BACK:
    {
      return Graphics::CullMode::BACK;
    }
    case Dali::FaceCullingMode::FRONT_AND_BACK:
    {
      return Graphics::CullMode::FRONT_AND_BACK;
    }
    default:
    {
      return Graphics::CullMode::NONE;
    }
  }
}

constexpr Graphics::BlendFactor ConvertBlendFactor(BlendFactor::Type blendFactor)
{
  switch(blendFactor)
  {
    case BlendFactor::ZERO:
      return Graphics::BlendFactor::ZERO;
    case BlendFactor::ONE:
      return Graphics::BlendFactor::ONE;
    case BlendFactor::SRC_COLOR:
      return Graphics::BlendFactor::SRC_COLOR;
    case BlendFactor::ONE_MINUS_SRC_COLOR:
      return Graphics::BlendFactor::ONE_MINUS_SRC_COLOR;
    case BlendFactor::SRC_ALPHA:
      return Graphics::BlendFactor::SRC_ALPHA;
    case BlendFactor::ONE_MINUS_SRC_ALPHA:
      return Graphics::BlendFactor::ONE_MINUS_SRC_ALPHA;
    case BlendFactor::DST_ALPHA:
      return Graphics::BlendFactor::DST_ALPHA;
    case BlendFactor::ONE_MINUS_DST_ALPHA:
      return Graphics::BlendFactor::ONE_MINUS_DST_ALPHA;
    case BlendFactor::DST_COLOR:
      return Graphics::BlendFactor::DST_COLOR;
    case BlendFactor::ONE_MINUS_DST_COLOR:
      return Graphics::BlendFactor::ONE_MINUS_DST_COLOR;
    case BlendFactor::SRC_ALPHA_SATURATE:
      return Graphics::BlendFactor::SRC_ALPHA_SATURATE;
    case BlendFactor::CONSTANT_COLOR:
      return Graphics::BlendFactor::CONSTANT_COLOR;
    case BlendFactor::ONE_MINUS_CONSTANT_COLOR:
      return Graphics::BlendFactor::ONE_MINUS_CONSTANT_COLOR;
    case BlendFactor::CONSTANT_ALPHA:
      return Graphics::BlendFactor::CONSTANT_ALPHA;
    case BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
      return Graphics::BlendFactor::ONE_MINUS_CONSTANT_ALPHA;
    default:
      return Graphics::BlendFactor();
  }
}

constexpr Graphics::BlendOp ConvertBlendEquation(DevelBlendEquation::Type blendEquation)
{
  switch(blendEquation)
  {
    case DevelBlendEquation::ADD:
      return Graphics::BlendOp::ADD;
    case DevelBlendEquation::SUBTRACT:
      return Graphics::BlendOp::SUBTRACT;
    case DevelBlendEquation::REVERSE_SUBTRACT:
      return Graphics::BlendOp::REVERSE_SUBTRACT;
    case DevelBlendEquation::COLOR:
      return Graphics::BlendOp::COLOR;
    case DevelBlendEquation::COLOR_BURN:
      return Graphics::BlendOp::COLOR_BURN;
    case DevelBlendEquation::COLOR_DODGE:
      return Graphics::BlendOp::COLOR_DODGE;
    case DevelBlendEquation::DARKEN:
      return Graphics::BlendOp::DARKEN;
    case DevelBlendEquation::DIFFERENCE:
      return Graphics::BlendOp::DIFFERENCE;
    case DevelBlendEquation::EXCLUSION:
      return Graphics::BlendOp::EXCLUSION;
    case DevelBlendEquation::HARD_LIGHT:
      return Graphics::BlendOp::HARD_LIGHT;
    case DevelBlendEquation::HUE:
      return Graphics::BlendOp::HUE;
    case DevelBlendEquation::LIGHTEN:
      return Graphics::BlendOp::LIGHTEN;
    case DevelBlendEquation::LUMINOSITY:
      return Graphics::BlendOp::LUMINOSITY;
    case DevelBlendEquation::MAX:
      return Graphics::BlendOp::MAX;
    case DevelBlendEquation::MIN:
      return Graphics::BlendOp::MIN;
    case DevelBlendEquation::MULTIPLY:
      return Graphics::BlendOp::MULTIPLY;
    case DevelBlendEquation::OVERLAY:
      return Graphics::BlendOp::OVERLAY;
    case DevelBlendEquation::SATURATION:
      return Graphics::BlendOp::SATURATION;
    case DevelBlendEquation::SCREEN:
      return Graphics::BlendOp::SCREEN;
    case DevelBlendEquation::SOFT_LIGHT:
      return Graphics::BlendOp::SOFT_LIGHT;
  }
  return Graphics::BlendOp{};
}
} // namespace

PipelineCacheL0Ptr PipelineCache::GetPipelineCacheL0(Program* program, Render::Geometry* geometry)
{
  auto it = std::find_if(level0nodes.begin(), level0nodes.end(), [program, geometry](PipelineCacheL0& item) {
    return ((item.program == program && item.geometry == geometry));
  });

  // Add new node to cache
  if(it == level0nodes.end())
  {
    uint32_t bindingIndex{0u};
    auto&    reflection = graphicsController->GetProgramReflection(program->GetGraphicsProgram());

    Graphics::VertexInputState vertexInputState{};
    uint32_t                   base = 0;

    bool attrNotFound = false;
    for(auto&& vertexBuffer : geometry->GetVertexBuffers())
    {
      const VertexBuffer::Format& vertexFormat = *vertexBuffer->GetFormat();

      uint32_t                  divisor         = vertexBuffer->GetDivisor();
      Graphics::VertexInputRate vertexInputRate = (divisor == 0
                                                     ? Graphics::VertexInputRate::PER_VERTEX
                                                     : Graphics::VertexInputRate::PER_INSTANCE);

      vertexInputState.bufferBindings.emplace_back(vertexFormat.size, // stride
                                                   vertexInputRate);
      //@todo Add the actual rate to the graphics struct

      const uint32_t attributeCount          = vertexBuffer->GetAttributeCount();
      uint32_t       lastBoundAttributeIndex = 0;
      for(uint32_t i = 0; i < attributeCount; ++i)
      {
        auto    attributeName = vertexBuffer->GetAttributeName(i);
        int32_t pLocation     = reflection.GetVertexAttributeLocation(std::string(attributeName.GetStringView()));
        if(-1 != pLocation)
        {
          auto location = static_cast<uint32_t>(pLocation);
          vertexInputState.attributes.emplace_back(location,
                                                   bindingIndex,
                                                   vertexFormat.components[i].offset,
                                                   GetPropertyVertexFormat(vertexFormat.components[i].type));
          ++lastBoundAttributeIndex;
        }
        else
        {
          attrNotFound = true;
          DALI_LOG_WARNING("Attribute not found in the shader: %s\n", attributeName.GetCString());
          // Don't bind unused attributes.
        }
      }
      base += lastBoundAttributeIndex;
      ++bindingIndex;
    }
    PipelineCacheL0 level0;
    level0.program    = program;
    level0.geometry   = geometry;
    level0.inputState = vertexInputState;

    // Observer geometry lifecycle
    geometry->AddLifecycleObserver(*this);

    it = level0nodes.insert(level0nodes.end(), std::move(level0));

    if(attrNotFound)
    {
      DALI_LOG_INFO(gLogFilter, Debug::General,
                    "!!!!!!!  Attributes not found. !!!!!!!!\n"
                    "Shader src: VERT:\n%s\nFRAGMENT:\n%s\n",
                    program->GetShaderData()->GetVertexShader(),
                    program->GetShaderData()->GetFragmentShader());
    }
  }

  return it;
}

PipelineCacheL1Ptr PipelineCacheL0::GetPipelineCacheL1(Render::Renderer* renderer, bool usingReflection)
{
  // hash must be collision free
  uint32_t hash = 0;
  auto     topo = (uint32_t(geometry->GetTopology()) & 0xffu);
  auto     cull = (uint32_t(renderer->GetFaceCullMode()) & 0xffu);

  static const Graphics::PolygonMode polyTable[] = {
    Graphics::PolygonMode::POINT,
    Graphics::PolygonMode::LINE,
    Graphics::PolygonMode::LINE,
    Graphics::PolygonMode::LINE,
    Graphics::PolygonMode::FILL,
    Graphics::PolygonMode::FILL,
    Graphics::PolygonMode::FILL};

  auto poly = polyTable[topo];

  static const FaceCullingMode::Type adjFaceCullingMode[4] =
    {
      FaceCullingMode::NONE,
      FaceCullingMode::BACK,
      FaceCullingMode::FRONT,
      FaceCullingMode::FRONT_AND_BACK,
    };

  static const FaceCullingMode::Type normalFaceCullingMode[4] =
    {
      FaceCullingMode::NONE,
      FaceCullingMode::FRONT,
      FaceCullingMode::BACK,
      FaceCullingMode::FRONT_AND_BACK,
    };

  static const FaceCullingMode::Type* cullModeTable[2] = {
    normalFaceCullingMode,
    adjFaceCullingMode};

  // Retrieve cull mode
  auto cullModeTableIndex = uint32_t(usingReflection) & 1u;
  cull                    = cullModeTable[cullModeTableIndex][renderer->GetFaceCullMode()];

  hash = (topo & 0xffu) | ((cull << 8u) & 0xff00u) | ((uint32_t(poly) << 16u) & 0xff0000u);

  // If L1 not found by hash, create rasterization state describing pipeline and store it
  auto it = std::find_if(level1nodes.begin(), level1nodes.end(), [hash](PipelineCacheL1& item) { return item.hashCode == hash; });

  if(it == level1nodes.end())
  {
    PipelineCacheL1 item;
    item.hashCode       = hash;
    item.rs.cullMode    = ConvertCullFace(FaceCullingMode::Type(cull));
    item.rs.frontFace   = Graphics::FrontFace::COUNTER_CLOCKWISE;
    item.rs.polygonMode = poly; // not in use
    item.ia.topology    = geometry->GetTopology();

    it = level1nodes.insert(level1nodes.end(), std::move(item));
  }

  return it;
}

void PipelineCacheL0::ClearUnusedCache()
{
  for(auto iter = level1nodes.begin(); iter != level1nodes.end();)
  {
    if(iter->ClearUnusedCache())
    {
      iter = level1nodes.erase(iter);
    }
    else
    {
      iter++;
    }
  }
}

PipelineCacheL2Ptr PipelineCacheL1::GetPipelineCacheL2(bool blend, bool premul, BlendingOptions& blendingOptions)
{
  // early out
  if(!blend)
  {
    if(DALI_UNLIKELY(noBlends.empty()))
    {
      noBlends.emplace_back(PipelineCacheL2{});
    }

    auto& noBlend = *noBlends.begin();

    if(noBlend.pipeline == nullptr)
    {
      // reset all before returning if pipeline has never been created for that case
      noBlend.hash = 0;
      memset(&noBlend.colorBlendState, 0, sizeof(Graphics::ColorBlendState));
    }
    return noBlends.begin();
  }

  auto bitmask = uint32_t(blendingOptions.GetBitmask());

  // Find by bitmask (L2 entries must be sorted by bitmask)
  auto it = std::find_if(level2nodes.begin(), level2nodes.end(), [bitmask](PipelineCacheL2& item) { return item.hash == bitmask; });

  // TODO: find better way of blend constants lookup
  PipelineCacheL2Ptr retval = level2nodes.end();
  if(it != level2nodes.end())
  {
    bool hasBlendColor = blendingOptions.GetBlendColor();
    while(hasBlendColor && it != level2nodes.end() && (*it).hash == bitmask)
    {
      Vector4 v(it->colorBlendState.blendConstants);
      if(v == *blendingOptions.GetBlendColor())
      {
        retval = it;
      }
      ++it;
    }
    if(!hasBlendColor)
    {
      retval = it;
    }
  }

  if(retval == level2nodes.end())
  {
    // create new entry and return it with null pipeline
    PipelineCacheL2 l2{};
    l2.pipeline           = nullptr;
    auto& colorBlendState = l2.colorBlendState;
    colorBlendState.SetBlendEnable(true);
    Graphics::BlendOp rgbOp   = ConvertBlendEquation(blendingOptions.GetBlendEquationRgb());
    Graphics::BlendOp alphaOp = ConvertBlendEquation(blendingOptions.GetBlendEquationAlpha());
    if(blendingOptions.IsAdvancedBlendEquationApplied() && premul)
    {
      if(rgbOp != alphaOp)
      {
        DALI_LOG_ERROR("Advanced Blend Equation MUST be applied by using BlendEquation.\n");
        alphaOp = rgbOp;
      }
    }

    colorBlendState
      .SetSrcColorBlendFactor(ConvertBlendFactor(blendingOptions.GetBlendSrcFactorRgb()))
      .SetSrcAlphaBlendFactor(ConvertBlendFactor(blendingOptions.GetBlendSrcFactorAlpha()))
      .SetDstColorBlendFactor(ConvertBlendFactor(blendingOptions.GetBlendDestFactorRgb()))
      .SetDstAlphaBlendFactor(ConvertBlendFactor(blendingOptions.GetBlendDestFactorAlpha()))
      .SetColorBlendOp(rgbOp)
      .SetAlphaBlendOp(alphaOp);

    // Blend color is optional and rarely used
    auto* blendColor = const_cast<Vector4*>(blendingOptions.GetBlendColor());
    if(blendColor)
    {
      colorBlendState.SetBlendConstants(blendColor->AsFloat());
    }

    l2.hash = blendingOptions.GetBitmask();

    auto upperBound = std::upper_bound(level2nodes.begin(), level2nodes.end(), l2, [](const PipelineCacheL2& lhs, const PipelineCacheL2& rhs) { return lhs.hash < rhs.hash; });

    level2nodes.insert(upperBound, std::move(l2));

    // run same function to retrieve retval
    retval = GetPipelineCacheL2(blend, premul, blendingOptions);
  }

  return retval;
}

bool PipelineCacheL1::ClearUnusedCache()
{
  for(auto iter = level2nodes.begin(); iter != level2nodes.end();)
  {
    if(iter->referenceCount == 0)
    {
      iter = level2nodes.erase(iter);
    }
    else
    {
      iter++;
    }
  }

  if(!noBlends.empty() && noBlends.begin()->referenceCount > 0)
  {
    return false;
  }

  return level2nodes.empty();
}

void PipelineCacheQueryInfo::GenerateHash()
{
  // Lightweight hash value generation.
  hash = (reinterpret_cast<std::size_t>(program) >> Dali::Log<sizeof(decltype(*program))>::value) ^
         (reinterpret_cast<std::size_t>(geometry) >> Dali::Log<sizeof(decltype(*geometry))>::value) ^
         ((blendingEnabled ? 1u : 0u) << 0u) ^
         ((alphaPremultiplied ? 1u : 0u) << 1u) ^
         (static_cast<std::size_t>(geometry->GetTopology()) << 2u) ^
         (static_cast<std::size_t>(renderer->GetFaceCullMode()) << 5u) ^
         ((cameraUsingReflection ? 1u : 0u) << 8u) ^
         (blendingEnabled ? static_cast<std::size_t>(blendingOptions->GetBitmask()) : 0xDA11u);
}

bool PipelineCacheQueryInfo::Equal(const PipelineCacheQueryInfo& lhs, const PipelineCacheQueryInfo& rhs) noexcept
{
  // Naive equal check.
  const bool ret = (lhs.hash == rhs.hash) && // Check hash value first
                   (lhs.program == rhs.program) &&
                   (lhs.geometry == rhs.geometry) &&
                   (lhs.blendingEnabled == rhs.blendingEnabled) &&
                   (lhs.alphaPremultiplied == rhs.alphaPremultiplied) &&
                   (lhs.geometry->GetTopology() == rhs.geometry->GetTopology()) &&
                   (lhs.renderer->GetFaceCullMode() == rhs.renderer->GetFaceCullMode()) &&
                   (lhs.cameraUsingReflection == rhs.cameraUsingReflection) &&
                   (!lhs.blendingEnabled ||
                    (lhs.blendingOptions->GetBitmask() == rhs.blendingOptions->GetBitmask() &&
                     ((lhs.blendingOptions->GetBlendColor() == nullptr && rhs.blendingOptions->GetBlendColor() == nullptr) ||
                      (lhs.blendingOptions->GetBlendColor() &&
                       rhs.blendingOptions->GetBlendColor() &&
                       (*lhs.blendingOptions->GetBlendColor() == *rhs.blendingOptions->GetBlendColor())))));

  return ret;
}

PipelineCache::PipelineCache(Graphics::Controller& controller)
: graphicsController(&controller)
{
  // Clean up cache first
  CleanLatestUsedCache();
}

PipelineCache::~PipelineCache()
{
  // Stop observer lifecycle
  for(auto&& level0node : level0nodes)
  {
    level0node.geometry->RemoveLifecycleObserver(*this);
  }
}

PipelineResult PipelineCache::GetPipeline(const PipelineCacheQueryInfo& queryInfo, bool createNewIfNotFound)
{
  // Seperate branch whether query use blending or not.
  const int latestUsedCacheIndex = queryInfo.blendingEnabled ? 0 : 1;

  // If we can reuse latest bound pipeline, Fast return.
  if(ReuseLatestBoundPipeline(latestUsedCacheIndex, queryInfo))
  {
    mLatestResult[latestUsedCacheIndex].level2->referenceCount++;
    return mLatestResult[latestUsedCacheIndex];
  }

  auto level0 = GetPipelineCacheL0(queryInfo.program, queryInfo.geometry);
  auto level1 = level0->GetPipelineCacheL1(queryInfo.renderer, queryInfo.cameraUsingReflection);

  PipelineCachePtr level2 = level1->GetPipelineCacheL2(queryInfo.blendingEnabled, queryInfo.alphaPremultiplied, *queryInfo.blendingOptions);

  // Create new pipeline at level2 if requested
  if(level2->pipeline == nullptr && createNewIfNotFound)
  {
    Graphics::ProgramState programState{};
    programState.program = &queryInfo.program->GetGraphicsProgram();
    // Create the pipeline
    Graphics::PipelineCreateInfo createInfo;
    createInfo
      .SetInputAssemblyState(&level1->ia)
      .SetVertexInputState(&level0->inputState)
      .SetRasterizationState(&level1->rs)
      .SetColorBlendState(&level2->colorBlendState)
      .SetProgramState(&programState);

    // Store a pipeline per renderer per render (renderer can be owned by multiple nodes,
    // and re-drawn in multiple instructions).
    level2->pipeline = graphicsController->CreatePipeline(createInfo, nullptr);
  }

  PipelineResult result{};

  result.pipeline = level2->pipeline.get();
  result.level2   = level2;

  level2->referenceCount++;

  // Copy query and result
  mLatestQuery[latestUsedCacheIndex]  = queryInfo;
  mLatestResult[latestUsedCacheIndex] = result;

  return result;
}

bool PipelineCache::ReuseLatestBoundPipeline(const int latestUsedCacheIndex, const PipelineCacheQueryInfo& queryInfo) const
{
  return mLatestResult[latestUsedCacheIndex].pipeline != nullptr && PipelineCacheQueryInfo::Equal(queryInfo, mLatestQuery[latestUsedCacheIndex]);
}

void PipelineCache::PreRender()
{
  CleanLatestUsedCache();

  // We don't need to check this every frame
  if(++mFrameCount >= CACHE_CLEAN_FRAME_COUNT)
  {
    mFrameCount = 0u;
    ClearUnusedCache();
  }
}

void PipelineCache::ClearUnusedCache()
{
  for(auto iter = level0nodes.begin(); iter != level0nodes.end();)
  {
    iter->ClearUnusedCache();

    if(iter->level1nodes.empty())
    {
      // Stop observer lifecycle
      iter->geometry->RemoveLifecycleObserver(*this);
      iter = level0nodes.erase(iter);
    }
    else
    {
      iter++;
    }
  }
}

void PipelineCache::ResetPipeline(PipelineCachePtr pipelineCache)
{
  // TODO : Can we always assume that pipelineCache input is valid iterator?
  pipelineCache->referenceCount--;
}

void PipelineCache::GeometryDestroyed(const Geometry* geometry)
{
  // Remove cached items what cache hold now.
  for(auto iter = level0nodes.begin(); iter != level0nodes.end();)
  {
    if(iter->geometry == geometry)
    {
      iter = level0nodes.erase(iter);
    }
    else
    {
      iter++;
    }
  }
}

} // namespace Dali::Internal::Render
