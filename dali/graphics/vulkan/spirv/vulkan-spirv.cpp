/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include <dali/graphics/vulkan/spirv/vulkan-spirv.h>
#include <dali/graphics/vulkan/spirv/vulkan-spirv-opcode.h>
#include <iostream>

#define debug( x ) std::cout << x << std::endl;

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
namespace SpirV
{

SPIRVShader::Impl& SPIRVShader::GetImplementation() const
{
  return *mImpl;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wframe-larger-than="
struct SPIRVShader::Impl
{
  /**
   * 32bit word needed to identify SPIRV code
   */
  static constexpr uint32_t MAGIC_NUMBER{0x07230203u};

  /**
   * SPIRV header binary structure
   */
  struct Header
  {
    uint32_t magicNumber;
    uint32_t versionNumber; // 0 | major | minor | 0
    uint32_t generatorMagicNumber;
    uint32_t bound;
    uint32_t reserved;
  };

  /**
   * Stores OpTypePointer opcodes
   */
  struct Pointer
  {
    SPIRVOpCode*    opCode;
    SPIRVOpCode*    pointerType;
    SpvStorageClass storageClass;
  };

  std::vector<Pointer> pointers{}; //@todo move to the local scope

  /**
   * Stores OpName opcodes which refer to pointers
   */
  struct PointerName
  {
    SPIRVOpCode* opName;
    SPIRVOpCode* opRefId;
    bool         isMemberName{false};
  };

  /**
   * ResourceDescriptor contains details of resource binding
   */
  struct ResourceDescriptor
  {
    Pointer*     pointer;

    uint32_t descriptorSet{0u};
    uint32_t binding{0u};
    uint32_t location{0u};
    uint32_t alignment{0u};

    vk::DescriptorType descriptorType{};
    std::string        name{};
  };

  /**
   * Structure describes each uniform block member
   */
  struct BlockMemberDescriptor
  {
    uint32_t blockId { 0u };
    uint32_t location { 0u };
    uint32_t offset { 0u };
    uint32_t size { 0u };
    std::string name {};
  };

  std::unordered_map<uint32_t, std::vector<BlockMemberDescriptor>> blockStructure;

  /**
   * Contains array of resources per storage type
   */
  struct StorageContainer
  {
    using PtrNameArray = std::vector<ResourceDescriptor>;
    SpvStorageClass storageClass{SpvStorageClassMax};
    PtrNameArray    resources{};
  };

  SPIRVOpCode& FindByResultId( uint32_t resultId ) const
  {
    return *( opResults[resultId] );
  }

  /**
   * Constructor
   * @param pData
   * @param size
   * @param stages
   */
  Impl( void* pData, std::size_t size, vk::ShaderStageFlags stages )
  {
    data.resize( size );
    auto begin = reinterpret_cast<uint32_t*>( pData );
    auto end   = begin + size;
    std::copy( begin, end, data.begin() );
    shaderStages = stages;
  }

  /**
   * Constructor
   * @tparam T
   * @param buffer
   * @param stages
   */
  template<typename T>
  explicit Impl( std::vector<T> buffer, vk::ShaderStageFlags stages )
  {
    data.resize( ( buffer.size() * sizeof( buffer[0] ) ) / sizeof( uint32_t ) );
    auto begin = reinterpret_cast<uint32_t*>( &*buffer.begin() );
    auto end   = reinterpret_cast<uint32_t*>( &*buffer.end() );
    std::copy( begin, end, data.begin() );
    shaderStages = stages;
  }

  // Searches the references within OpVariable, OpConstant and OpFunction
  const SPIRVOpCode& FindInstanceByType( uint32_t resultId ) const
  {
    // OpVariable
    for( auto&& op : opResults )
    {
      if( op )
      {
        switch( op->code )
        {
          // Variable, Constant
          case SpvOpVariable:
          case SpvOpConstant:
          {
            if( op->GetParameterU32( 0 ) == resultId )
            {
              return *op;
            }
            break;
          }
          default:
          {
            break;
          }
        }
      }
    }
    return OP_CODE_NULL;
  }

  auto FindDecorationsForId( uint32_t id ) const
  {
    std::vector<const SPIRVOpCode*> retval{};

    for( auto&& op : opCodes )
    {
      if( op.code == SpvOpDecorate && op.GetParameterU32( 0 ) == id )
      {
        retval.push_back( &op );
      }
    }
    return retval;
  }

  SPIRVOpCode& GetReferencedOpCode( const SPIRVOpCode& opCode, uint32_t refIndex ) const
  {
    return FindByResultId( opCode.GetParameterU32( refIndex ) );
  }



  auto GetDecorationsOpId( const SPIRVOpCode& resultOp )
  {
    std::vector<SPIRVOpCode*> retval;
    if( resultOp.hasResult )
    {
      for( auto&& op : opCodes )
      {
        if( op == SpvOpDecorate && op.GetParameterU32( 0 ) == resultOp.localData.resultId )
        {
          retval.push_back( &op );
        }
      }
    }
    return retval;
  }

  bool CheckDecorationForOpId( const SPIRVOpCode& resultOp, SpvDecoration expectedDecoration )
  {
    if( resultOp.hasResult )
    {
      for( auto&& op : opCodes )
      {
        if( op == SpvOpDecorate && op.GetParameterU32( 0 ) == resultOp.localData.resultId &&
            op.GetParameter<SpvDecoration>( 1 ) == expectedDecoration )
        {
          return true;
        }
      }
    }
    return false;
  }

  bool Initialise()
  {
    // test if we have valid SPIRV header
    auto iter = data.begin();
    if( !CheckHeader() )
    {
      debug( "Not SPIRV!" );
      return false;
    }

    debug( "SPIR-V detected" );
    std::advance( iter, 5u ); // skip header

    while( iter != data.end() )
    {
      auto opword    = *iter;
      auto wordCount = ( ( opword >> 16 ) & 0xFFFF );
      auto opCode    = ( (opword)&0xFFFF );

      //debug( "wordCount: " << wordCount << " opcode: " << opCode );

      auto& op = FindOpCode( opCode );

      if( op != OP_CODE_NULL )
      {
        uint32_t resultIndex{0};
        int      resultIndexOffset = 1;
        int32_t  resultType{0u};

        // make a copy
        opCodes.emplace_back( op );
        auto& opcode           = opCodes.back();
        opcode.localData.start = &*iter;
        opcode.localData.count = wordCount;

        // update result type and index for non-void opcodes
        if( op.hasResultType )
        {
          resultIndexOffset++;
        }
        if( op.hasResult )
        {
          if( op.hasResultType )
          {
            resultType = static_cast<int32_t>(*( iter + 1 ) );
          }
          resultIndex                 = *( iter + resultIndexOffset );
          opcode.localData.resultId   = resultIndex;
          opcode.localData.resultType = resultType;
          if( opResults.size() <= opcode.localData.resultId )
          {
            opResults.resize( opcode.localData.resultId + 1 );
          }
          opResults[opcode.localData.resultId] = &opcode;
        }
      }

      // next instruction
      std::advance( iter, wordCount );
    }

    for( auto&& opcode : opCodes )
    {
      // OpTypePointer? Extract storage class and type it points to ( will be probably named )
      // - pointers usually are not named
      // - objects they point at usually are named
      // - pointers carry storage class qualifier
      if( opcode == SpvOpTypePointer )
      {
        auto storageClass = opcode.GetParameter<SpvStorageClass>( 1 ); // get storage class
        auto refId        = opcode.GetParameterU32( 2 );               // get type ref id
        pointers.emplace_back( Pointer() = {&opcode, &FindByResultId( refId ), storageClass} );
      }
      else if( opcode == SpvOpVariable )
      {
        auto storageClass = opcode.GetParameter<SpvStorageClass>( 1 ); // get storage class
        auto refId        = opcode.GetParameterU32( 0 );               // get type ref id
        pointers.emplace_back( Pointer() = {&opcode, &FindByResultId( refId ), storageClass} );
      }
      else if( opcode == SpvOpName )
      {
        auto refId = opcode.GetParameterU32( 0 ); // get type ref id
        names.emplace_back( PointerName{} = {&opcode, &FindByResultId( refId ), false} );
      }
      else if( opcode == SpvOpMemberName )
      {
        auto refId = opcode.GetParameterU32( 0 ); // get type ref id
        names.emplace_back( PointerName{} = {&opcode, &FindByResultId( refId ), true} );
      }
      else if( opcode == SpvOpDecorate ) //@todo: member decorate
      {
        decorate.push_back( &opcode );
      }
    }

    std::vector<StorageContainer> storageContainer{};

    // Per each named resource build the resource map
    // @todo simplify that!
    for( auto&& name : names )
    {
      std::string strName( name.opName->GetParameterAsString( name.isMemberName ? 2 : 1 ) );

      if( !strName.empty() && name.isMemberName )
      {

      }
      else if( !strName.empty() && !name.isMemberName )
      {
        SpvStorageClass storageClass{SpvStorageClassMax};
        Pointer*        spvPointer{nullptr};

        // resource descriptor
        uint32_t resourceDescriptorSet{0u};
        uint32_t resourceLocation{0u};
        uint32_t resourceBinding{0u};
        uint32_t resourceAlignment{0u};

        // Will contain storage
        if( name.opRefId->code == SpvOpVariable )
        {
          for( auto&& ptr : pointers )
          {
            if( ptr.opCode->localData.resultId == name.opRefId->GetParameterU32( 0u ) )
            {
              storageClass = ptr.storageClass;
              spvPointer   = &ptr;

              // look for all the decoration for the variable
              for( auto&& decor : decorate )
              {
                if( decor->GetParameterU32( 0 ) == name.opRefId->localData.resultId ||
                    decor->GetParameterU32( 0 ) == ptr.pointerType->localData.resultId )
                {
                  auto decorationQualifier = decor->GetParameter<SpvDecoration>( 1 );
                  auto decorationParameter = decor->GetParameterU32( 2 );

                  if( decorationQualifier == SpvDecorationLocation )
                  {
                    resourceLocation = decorationParameter;
                  }
                  else if( decorationQualifier == SpvDecorationBinding )
                  {
                    resourceBinding = decorationParameter;
                  }
                  else if( decorationQualifier == SpvDecorationDescriptorSet )
                  {
                    resourceDescriptorSet = decorationParameter;
                  }
                  else if( decorationQualifier == SpvDecorationAlignment )
                  {
                    resourceAlignment = decorationParameter;
                  }
                }
              }

              //@todo: support input handling ( when vertex buffers work )
              if( storageClass == SpvStorageClassInput )
              {
                auto format = vk::Format{ vk::Format::eUndefined };
                auto location = U32(-1);
                if( *ptr.pointerType == SpvOpTypeFloat )
                {
                  format = vk::Format::eR32Sfloat;
                }
                else if( *ptr.pointerType == SpvOpTypeInt )
                {
                  format = vk::Format::eR32Sint;
                }
                else if( *ptr.pointerType == SpvOpTypeVector )
                {
                  // always 32-bit float signed
                  const vk::Format SUPPORTED_VECTOR_FORMATS[] =
                                     {
                                       vk::Format::eR32Sfloat,
                                       vk::Format::eR32G32Sfloat,
                                       vk::Format::eR32G32B32Sfloat,
                                       vk::Format::eR32G32B32A32Sfloat,
                                     };

                  format = SUPPORTED_VECTOR_FORMATS[ptr.pointerType->GetParameterU32(2)];
                }

                // format is valid, find location
                if( format != vk::Format::eUndefined )
                {
                  auto decorations = FindDecorationsForId( name.opName->GetParameterU32(0) );
                  for( auto&& decor : decorations )
                  {
                    if( decor->GetParameter<SpvDecoration>(1) == SpvDecorationLocation )
                    {
                      location = decor->GetParameterU32(2);
                      break;
                    }
                  }
                  if( location != U32(-1) )
                  {
                    if( location >= inputAttributes.size() )
                    {
                      inputAttributes.resize( location+1 );
                    }
                    inputAttributes[location] = { location, name.opName->GetParameterAsString(1), format };
                  }
                }
              }
              break;
            }
          }
        }
        // if pointing at type, we need to find what refers to it among pointers and variables
        else if( name.opRefId->isType )
        {
          for( auto&& ptr : pointers )
          {
            auto ptrResultId = ptr.pointerType->localData.resultId;
            if( ptrResultId == name.opRefId->GetParameterU32( 0u ) )
            {
              auto memberDesc = BuildMemberDescriptorList( ptrResultId );

              if( memberDesc.size() )
              {
                // push onto list of blocks
                name.opName->GetParameterAsString( 0 );
                blockStructureMap[ name.opName ] = memberDesc;
              }

              const auto& instance = FindInstanceByType( ptrResultId );
              if( instance != OP_CODE_NULL )
              {
                auto decors = FindDecorationsForId( instance.localData.resultId );

                for( auto&& opDecorate : decors )
                {
                  auto decorationQualifier = opDecorate->GetParameter<SpvDecoration>( 1 );
                  auto decorationParameter = opDecorate->GetParameterU32( 2 );
                  if( decorationQualifier == SpvDecorationLocation )
                  {
                    resourceLocation = decorationParameter;
                  }
                  else if( decorationQualifier == SpvDecorationBinding )
                  {
                    resourceBinding = decorationParameter;
                  }
                  else if( decorationQualifier == SpvDecorationDescriptorSet )
                  {
                    resourceDescriptorSet = decorationParameter;
                  }
                  else if( decorationQualifier == SpvDecorationAlignment )
                  {
                    resourceAlignment = decorationParameter;
                  }
                }
              }
              storageClass = ptr.storageClass;
              spvPointer   = &ptr;
              break;
            }
          }
        }

        // add resource to the storage container
        if( storageClass != SpvStorageClassMax )
        {
          auto index = static_cast<uint32_t>( storageClass );
          if( storageContainer.size() <= index )
          {
            storageContainer.resize( index + 1 );
          }

          auto& container        = storageContainer[index];
          container.storageClass = storageClass;
          container.resources.push_back( {spvPointer} );

          auto& resource         = container.resources.back();
          resource.location      = resourceLocation;
          resource.binding       = resourceBinding;
          resource.descriptorSet = resourceDescriptorSet;
          resource.alignment     = resourceAlignment;
          resource.name          = strName;
        }
      }
    }

    // Identify descriptor set types based on the usage within
    // the shader ( ref. vulkan spec 1.0.68 )
    uniformResources.clear();
    for( auto&& storage : storageContainer )
    {
      if( storage.storageClass == SpvStorageClassUniformConstant )
      {
        for( auto&& resource : storage.resources )
        {
          if( TestStorageImageDescriptor( resource ) )
          {
            resource.descriptorType = vk::DescriptorType::eStorageImage;
          }
          else if( TestSamplerDescriptor( resource ) )
          {
            resource.descriptorType = vk::DescriptorType::eSampler;
          }
          else if( TestSampledImageDescriptor( resource ) )
          {
            resource.descriptorType = vk::DescriptorType::eSampledImage;
          }
          else if( TestCombinedImageSamplerDescriptor( resource ) )
          {
            resource.descriptorType = vk::DescriptorType::eCombinedImageSampler;
          }
          else if( TestUniformTexelBufferDescriptor( resource ) )
          {
            resource.descriptorType = vk::DescriptorType::eUniformTexelBuffer;
          }
          else if( TestStorageTexelBufferDescriptor( resource ) )
          {
            resource.descriptorType = vk::DescriptorType::eStorageTexelBuffer;
          }
          else
          {
            // @todo check the shader, something hasn't been recognized
            resource.descriptorType = vk::DescriptorType{};
          }
          if( resource.descriptorType != vk::DescriptorType{} )
          {
            uniformResources.push_back( &resource );
          }
        }
      }
      else if( storage.storageClass == SpvStorageClassUniform )
      {
        /**
         * We need to test against 4 possible descriptor types:
         * UniformBuffer
         * StorageBuffer
         * DynamicUniformBuffer
         * DynamicStorageBuffer
         *
         * This information must be read directly from decoration
         */
        for( auto&& resource : storage.resources )
        {
          if( TestUniformBufferDescriptor( resource ) )
          {
            resource.descriptorType = vk::DescriptorType::eUniformBuffer;
          }
          else if( TestStorageBufferDescriptor( resource ) )
          {
            resource.descriptorType = vk::DescriptorType::eStorageBuffer;
          }
          else
          {
            resource.descriptorType = vk::DescriptorType{};
          }
          if( resource.descriptorType != vk::DescriptorType{} )
          {
            uniformResources.push_back( &resource );
          }
        }
      }
    }


    // sort uniform resources by descriptor sets and bindings
    std::sort( uniformResources.begin(), uniformResources.end(), ( []( const auto& lhs, const auto& rhs ) {
                 if( lhs->descriptorSet < rhs->descriptorSet )
                   return true;
                 else if( lhs->binding < rhs->binding )
                   return true;
                 else if( lhs->location < rhs->location )
                   return true;
                 return false;
               } ) );

    // TODO: store more details?
    for( auto&& res : uniformResources )
    {
      auto tmp = SPIRVUniformOpaque{};
      tmp.name = res->name;
      tmp.binding = res->binding;
      tmp.descriptorSet = res->descriptorSet;
      tmp.type = res->descriptorType;
      uniformOpaqueReflection.emplace_back( tmp );
    }

    GenerateDescriptorSetLayoutCreateInfo( uniformResources );

    // Generate uniform block reflection
    uniformBlockReflection.clear();
    auto blockIndex  = 0u;
    for( auto&& uboDesc : blockStructureMap )
    {
      auto block = uboDesc.first;
      auto blockName = block->GetParameterAsString(1);
      uniformBlockReflection.emplace_back();
      auto& ubo = uniformBlockReflection.back();
      ubo.name = blockName;

      // find block resource details
      for( auto&& resource : uniformResources )
      {
        if( resource->name == blockName )
        {
          ubo.binding = resource->binding;
          ubo.descriptorSet = resource->descriptorSet;
          auto& ptrType = *resource->pointer->pointerType;

          // calculate size of struct
          // todo: support deep search
          ubo.size = 0;
          if( ptrType == SpvOpTypeStruct )
          {
            for( auto i = 1u; i < ptrType.localData.count-1; ++i)
            {
              auto memberId = ptrType.GetParameterU32( i );
              auto& memberType = FindByResultId( memberId );

              if(memberType == SpvOpTypeMatrix )
              {
                ubo.size += memberType.GetParameterU32( 2 ) * memberType.GetParameterU32( 2 ) * U32(sizeof(float));
              }
              else if(memberType == SpvOpTypeVector )
              {
                ubo.size += memberType.GetParameterU32( 2 ) * U32(sizeof(float));
              }
              else if(memberType.isTrivial)
              {
                ubo.size += 4;
              }
              else if(memberType == SpvOpTypeArray || memberType == SpvOpTypeStruct )
              {
                // todo: support recursive search through types!
              }
            }
          }
        }
      }

      // write members
      debug(uboDesc.first->GetParameterAsString(1) << ": ");
      for( auto&& desc : uboDesc.second )
      {
        ubo.members.emplace_back();
        auto& uboMember = ubo.members.back();
        uboMember.name = desc.name;
        uboMember.location = desc.location;
        uboMember.offset = desc.offset;
        uboMember.blockIndex = blockIndex;
      }

      blockIndex++;
    }

    debug( "done" );
    return true;
  }

  /**
   * Recognizes descriptor type VkDescriptorTypeStorageImage
   * GLSL:
   *      layout (set=m, binding=n, r32f) uniform image2D myStorageImage;
   *
   * SPIR-V:
   *      %7 = OpTypeImage %6 2D 0 0 0 2 R32f
   *      %8 = OpTypePointer UniformConstant %7
   *      %9 = OpVariable %8 UniformConstant
   * @param resource
   * @return
   */
  bool TestStorageImageDescriptor( const ResourceDescriptor& resource )
  {
    if( *resource.pointer->opCode == SpvOpTypePointer )
    {
      auto& opCode = GetReferencedOpCode( *resource.pointer->opCode, 2 );
      if( opCode == SpvOpTypeImage && opCode.GetParameterU32( 6 ) == 2 )
      {
        return true;
      }
    }
    return false;
  }

  /**
   * Recognizes descriptor type VkDescriptorTypeSampler
   * GLSL:
          layout (set=m, binding=n) uniform sampler mySampler;
   *
   * SPIR-V:
          %3 = OpTypeFunction %2
          %6 = OpTypeSampler
          %7 = OpTypePointer UniformConstant %6
          %8 = OpVariable %7 UniformConstant

   * @param resource
   * @return
   */
  bool TestSamplerDescriptor( const ResourceDescriptor& resource )
  {
    if( *resource.pointer->opCode == SpvOpTypePointer )
    {
      auto& opCode = GetReferencedOpCode( *resource.pointer->opCode, 2 );
      if( opCode == SpvOpTypeSampler )
      {
        return true;
      }
    }
    return false;
  }

  /**
   * Recognizes descriptor type VkDescriptorTypeSampledImage
   * GLSL:
   *      layout (set=m, binding=n) uniform texture2D mySampledImage;
   * SPIR_V:
          %6 = OpTypeFloat 32
          %7 = OpTypeImage %6 2D 0 0 0 1 Unknown
          %8 = OpTypePointer UniformConstant %7
          %9 = OpVariable %8 UniformConstant
   *
   * @param resource
   * @return
   */
  bool TestSampledImageDescriptor( const ResourceDescriptor& resource )
  {
    if( *resource.pointer->opCode == SpvOpTypePointer )
    {
      auto& opCode = GetReferencedOpCode( *resource.pointer->opCode, 2 );
      if( opCode == SpvOpTypeImage && opCode.GetParameterU32( 6 ) == 1 )
      {
        return true;
      }
    }
    return false;
  }

  /**
   * Recognizes descriptor type VkDescriptorTypeCombinedImageSampler
   * GLSL:
   *      layout (set=m, binding=n) uniform sampler2D myCombinedImageSampler;
   * SPIR-V:
          %7 = OpTypeImage %6 2D 0 0 0 1 Unknown
          %8 = OpTypeSampledImage %7
          %9 = OpTypePointer UniformConstant %8
         %10 = OpVariable %9 UniformConstant
   * @param resource
   * @return
   */
  bool TestCombinedImageSamplerDescriptor( const ResourceDescriptor& resource )
  {
    if( *resource.pointer->opCode == SpvOpTypePointer )
    {
      auto& opCode = GetReferencedOpCode( *resource.pointer->opCode, 2 );
      if( opCode == SpvOpTypeSampledImage )
      {
        return true;
      }
    }
    return false;
  }

  /**
   * Recognizes descriptor type VkDescriptorTypeUniformTexelBuffer
   * GLSL:
   *      layout (set=m, binding=n) uniform samplerBuffer myUniformTexelBuffer;
   * SPIR-V:
            %6 = OpTypeFloat 32
            %7 = OpTypeImage %6 Buffer 0 0 0 1 Unknown
            %8 = OpTypePointer UniformConstant %7
            %9 = OpVariable %8 UniformConstant
   * @param resource
   * @return
   */
  bool TestUniformTexelBufferDescriptor( const ResourceDescriptor& resource )
  {
    if( *resource.pointer->opCode == SpvOpTypePointer )
    {
      auto& opCode = GetReferencedOpCode( *resource.pointer->opCode, 2 );
      if( opCode == SpvOpTypeImage && opCode.GetParameter<SpvDim>( 2 ) == SpvDimBuffer &&
          opCode.GetParameterU32( 6 ) == 1 )
      {
        return true;
      }
    }
    return false;
  }

  /**
   * Recognizes descriptor type VkDescriptorTypeStorageTexelBuffer
   * GLSL:
   *      layout (set=m, binding=n, r32f) uniform imageBuffer myStorageTexelBuffer;
   * SPIR-V:
          %7 = OpTypeImage %6 Buffer 0 0 0 2 R32f
          %8 = OpTypePointer UniformConstant %7
          %9 = OpVariable %8 UniformConstant
   * @param resource
   * @return
   */
  bool TestStorageTexelBufferDescriptor( const ResourceDescriptor& resource )
  {
    if( *resource.pointer->opCode == SpvOpTypePointer )
    {
      auto& opCode = GetReferencedOpCode( *resource.pointer->opCode, 2 );
      if( opCode == SpvOpTypeImage && opCode.GetParameter<SpvDim>( 2 ) == SpvDimBuffer &&
          opCode.GetParameterU32( 6 ) == 2 )
      {
        return true;
      }
    }
    return false;
  }

  /**
   * Recognizes descriptor type VkDescriptorTypeUniformBuffer
   * GLSL:
          layout (set=m, binding=n) uniform myUniformBuffer
          {
              vec4 myElement[32];
          };
   * SPIR-V:
         %11 = OpTypeStruct %10
         %12 = OpTypePointer Uniform %11
         %13 = OpVariable %12 Uniform
   * @todo pull data out of OpDecorate ( Block )
   * @param resource
   * @return
   */
  bool TestUniformBufferDescriptor( const ResourceDescriptor& resource )
  {
    auto& ptrOpCode( *resource.pointer->opCode );
    if( ptrOpCode == SpvOpTypePointer && ptrOpCode.GetParameter<SpvStorageClass>( 1 ) == SpvStorageClassUniform )
    {
      auto& opTypeStruct = GetReferencedOpCode( ptrOpCode, 2 );
      if( opTypeStruct == SpvOpTypeStruct )
      {
        return CheckDecorationForOpId( opTypeStruct, SpvDecorationBlock );
      }
    }
    return false;
  }

  bool TestStorageBufferDescriptor( const ResourceDescriptor& resource )
  {
    auto& ptrOpCode( *resource.pointer->opCode );
    if( ptrOpCode == SpvOpTypePointer && ptrOpCode.GetParameter<SpvStorageClass>( 1 ) == SpvStorageClassUniform )
    {
      auto& opTypeStruct = GetReferencedOpCode( ptrOpCode, 2 );
      if( opTypeStruct == SpvOpTypeStruct )
      {
        return CheckDecorationForOpId( opTypeStruct, SpvDecorationBufferBlock );
      }
    }
    return false;
  }

  /**
   * Collects all the member decorations for particular block and field
   * @param structId
   * @param fieldLocation
   * @return
   */
  std::vector<BlockMemberDescriptor> BuildMemberDescriptorList( uint32_t blockId )
  {
    // collect data:
    // - block id
    // - name
    // - location
    // - offset
    // Skip built-ins
    // there is no way to know size of single field
    // size of the block remains unknown ( depends on the last type )
    std::vector<BlockMemberDescriptor> retval;
    for( auto&& op : opCodes )
    {
      if( op == SpvOpMemberName && op.GetParameterU32( 0 ) == blockId )
      {
        auto location = op.GetParameterU32( 1 );
        if( retval.size() <= location )
        {
          retval.resize( location+1 );
        }
        auto& desc = retval[location];
        desc.location = location;
        desc.name = op.GetParameterAsString( 2 );
        desc.blockId = blockId;
      }
      else if( op == SpvOpMemberDecorate && op.GetParameterU32(0) == blockId )
      {
        auto location = op.GetParameterU32( 1 );
        if( retval.size() <= location )
        {
          retval.resize( location+1 );
        }
        auto& desc = retval[location];

        auto decoration = op.GetParameter<SpvDecoration>( 2 );
        if( decoration == SpvDecorationOffset )
        {
          desc.offset = op.GetParameterU32( 3 );
        }
        else if( decoration == SpvDecorationBuiltIn ) // skip builtins for now, return empty result
        {
          retval.clear();
          break;
        }
      }
    }

    return retval;
  }



  void GenerateDescriptorSetLayoutCreateInfo( const std::vector<ResourceDescriptor*>& _uniformResources )
  {
    auto currentSet     = -1u;
    auto currentBinding = -1u;

    for( auto&& resource : uniformResources )
    {
      // if descriptor set changed, increment
      if( resource->descriptorSet != currentSet )
      {
        // finalize current set
        if( currentSet != -1u )
        {
          auto& layoutData = layoutCreateInfoCache.back();
          layoutData.createInfo.setPBindings( layoutData.bindings.data() )
            .setBindingCount( U32( layoutData.bindings.size() ) );
        }
        currentSet = resource->descriptorSet;
        if( currentSet == -1u || layoutCreateInfoCache.size() <= currentSet )
        {
          layoutCreateInfoCache.resize(currentSet+1);
        }
        layoutCreateInfoCache.back() = {vk::DescriptorSetLayoutCreateInfo{}.setBindingCount( 0u ).setPBindings( nullptr ), {}};
        currentBinding = -1u;
      }

      auto& layoutData = layoutCreateInfoCache.back();

      if( resource->binding != currentBinding )
      {
        currentBinding = resource->binding;
        layoutData.bindings.emplace_back( vk::DescriptorSetLayoutBinding{} );
        auto& binding = layoutData.bindings.back();
        binding.setBinding( resource->binding );
        binding.setDescriptorType( resource->descriptorType );
        binding.setDescriptorCount( 1 ); //@todo: support arrays!
        binding.setStageFlags( shaderStages );
      }
      else
      {
        //@todo assert that bindings overlap within same descriptor set!!!
      }
    }
    // finalize current set
    if( currentSet != -1u )
    {
      auto& layoutData = layoutCreateInfoCache.back();
      layoutData.createInfo.setPBindings( layoutData.bindings.data() )
                .setBindingCount( U32( layoutData.bindings.size() ) );
    }
  }

  std::vector<vk::DescriptorSetLayoutCreateInfo> GenerateDescriptorSetLayoutCreateInfo() const
  {
    std::vector<vk::DescriptorSetLayoutCreateInfo> retval{};

    retval.resize( layoutCreateInfoCache.size() );
    auto i = 0u;
    for( auto&& layout : retval )
    {
      layout = layoutCreateInfoCache[i++].createInfo;
    }
    return retval;
  }

  bool GetVertexInputAttributes( std::vector<SPIRVVertexInputAttribute>& out, bool canOverlap = false ) const
  {
    if( out.size() < inputAttributes.size() )
    {
      out.resize( inputAttributes.size() );
    }
    for( auto i = 0u; i < inputAttributes.size(); ++i )
    {
      if( !canOverlap && out[i].format != vk::Format::eUndefined )
      {
        return false;
      }
      out[i] = inputAttributes[i];
    }
    return true;
  }

  /**
   * Tests if the header is valid for SPIR-V
   * @return
   */
  bool CheckHeader()
  {
    header = *reinterpret_cast<Header*>( data.data() );
    return MAGIC_NUMBER == header.magicNumber;
  }

public:

  std::vector<SPIRVOpCode>  opCodes; // contains all opcodes
  std::vector<SPIRVOpCode*> opResults; // links to the resulting opcode or nullptr if opcode doesn't return

  /**
   * The cache of generated create info structures, it's necessary to keep it
   * due to object lifetime.
   * The ownership is passed to the caller ( opaque object ). It must exist as long
   * as the device::CreateDescriptorSetLayout() returns!
   */
  struct LayoutAndBindings
  {
    vk::DescriptorSetLayoutCreateInfo           createInfo;
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
  };

  std::vector<LayoutAndBindings> layoutCreateInfoCache{};
  std::vector<SPIRVVertexInputAttribute> inputAttributes{};

  std::vector<SPIRVWord>    data;
  std::vector<PointerName>  names{};
  std::vector<SPIRVOpCode*> decorate{};

  std::unordered_map<SPIRVOpCode*, std::vector<BlockMemberDescriptor>> blockStructureMap;
  std::vector<SPIRVUniformBlock> uniformBlockReflection;
  std::vector<ResourceDescriptor*> uniformResources;
  std::vector<SPIRVUniformOpaque> uniformOpaqueReflection;
  Header   header;

  vk::ShaderStageFlags shaderStages{};



};
#pragma GCC diagnostic pop


/**************************************************************************************
 * SPIRVShader
 */

SPIRVShader::SPIRVShader() = default;

SPIRVShader::~SPIRVShader() = default;

SPIRVShader::SPIRVShader( SPIRVShader&& shader ) noexcept = default;

SPIRVShader::SPIRVShader( Impl& impl )
{
  mImpl.reset( &impl );
}

SPIRVShader::SPIRVShader( std::vector<SPIRVWord> code, vk::ShaderStageFlags stages )
{
  mImpl = std::make_unique<Impl>( code, stages );
}

std::vector<vk::DescriptorSetLayoutCreateInfo> SPIRVShader::GenerateDescriptorSetLayoutCreateInfo() const
{
  return mImpl->GenerateDescriptorSetLayoutCreateInfo();
}

uint32_t SPIRVShader::GetOpCodeCount() const
{
  return static_cast<uint32_t>( mImpl->opCodes.size() );
}

const SPIRVOpCode* SPIRVShader::GetOpCodeAt( uint32_t index ) const
{
  return &mImpl->opCodes[index];
}

const SPIRVOpCode* SPIRVShader::GetOpCodeForResultId( uint32_t resultId ) const
{
  return mImpl->opResults[resultId];
}

SPIRVWord SPIRVShader::GetOpCodeParameterWord( const SPIRVOpCode& opCode, uint32_t index ) const
{
  return GetOpCodeParameter<SPIRVWord>( opCode, index );
}

SpvOp SPIRVShader::GetOpCodeType( SPIRVOpCode& opCode )
{
  return SpvOpMax;
}

const uint32_t* SPIRVShader::GetOpCodeParameterPtr( const SPIRVOpCode& opCode, uint32_t index ) const
{
  return ( opCode.localData.start + index + 1 );
}

void SPIRVShader::GetVertexInputAttributes( std::vector<SPIRVVertexInputAttribute>& out ) const
{
  mImpl->GetVertexInputAttributes( out );
}

const std::vector<SPIRVUniformBlock>& SPIRVShader::GetUniformBlocks() const
{
  return mImpl->uniformBlockReflection;
}

const std::vector<SPIRVUniformOpaque>& SPIRVShader::GetOpaqueUniforms() const
{
  return mImpl->uniformOpaqueReflection;
}

bool SPIRVShader::FindUniformMemberByName( const std::string& uniformName, SPIRVUniformBlockMember& out ) const
{
  for( auto&& ubo : mImpl->uniformBlockReflection )
  {
    for( auto&& member : ubo.members )
    {
      if( member.name == uniformName )
      {
        out = member;
        return true;
      }
    }
  }
  return false;
}

/**************************************************************************************
 * SPIRVUtils
 */

/**
 * SPIRVUtils
 * @param data
 * @return
 */
std::unique_ptr<SPIRVShader> SPIRVUtils::Parse( std::vector<SPIRVWord> data, vk::ShaderStageFlags stages )
{
  auto shader = std::unique_ptr<SPIRVShader>( new SPIRVShader( data, stages ) );
  if( !shader->GetImplementation().Initialise() )
  {
    return nullptr;
  }
  return shader;
}

std::unique_ptr<SPIRVShader> SPIRVUtils::Parse( const SPIRVWord* data, size_t sizeInBytes, vk::ShaderStageFlags stages )
{
  std::vector<SPIRVWord> spirvCode{};
  auto wordSize = sizeInBytes / sizeof(SPIRVWord);
  spirvCode.resize( wordSize );
  std::copy( data, data+wordSize, spirvCode.begin());
  return Parse( spirvCode, stages );
}




} // namespace SpirV

} // namespace Vulkan

} // namespace Graphics

} // namespace Dali