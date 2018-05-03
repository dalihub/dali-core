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

#include <iostream>

#include <dali/graphics/vulkan/spirv/vulkan-spirv.h>
#include <dali/graphics/vulkan/spirv/vulkan-spirv-opcode.h>

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
   * Stores descriptorset bindings and createinfo data used by reflection
   */
  struct DescriptorSetLayoutAndBindingInfo
  {
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    vk::DescriptorSetLayoutCreateInfo createInfo;
  };

  SPIRVOpCode& FindByResultId( uint32_t resultId ) const
  {
    return *(opResults[resultId] );
  }

  SPIRVOpCode* FindByResultId( SPIRVOpCode& opcode ) const
  {
    if(!opcode.hasResult)
    {
      return nullptr;
    }
    return opResults[opcode.localData.resultId];
  }

  SPIRVOpCode* FindByResultPtrId( uint32_t resultId ) const
  {
    if( resultId < opResults.size() )
    {
      return opResults[resultId];
    }
    return nullptr;
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
  }

  auto FindDecorationsForId( uint32_t id )
  {
    std::vector<SPIRVOpCode*> retval{};
    for( auto&& op : opCodes )
    {
      if( op.code == SpvOpDecorate && op.GetParameterU32( 0 ) == id )
      {
        retval.push_back( &op );
      }
    }
    return retval;
  }

  auto FindMemberDecorationsForId( uint32_t id, uint32_t memberIndex )
  {
    std::vector<SPIRVOpCode*> retval{};
    for( auto&& op : opCodes )
    {
      if( op.code == SpvOpMemberDecorate && op.GetParameterU32( 0 ) == id && op.GetParameterU32( 1 ) == memberIndex )
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

  struct SPIRVReflectionData
  {
    SPIRVReflectionData() = default;
    SPIRVReflectionData( const SPIRVReflectionData& ) = default;
    SPIRVReflectionData& operator=( const SPIRVReflectionData& ) = default;
    SPIRVReflectionData( SPIRVReflectionData&& ) = delete;
    SPIRVReflectionData& operator=( SPIRVReflectionData&& ) = delete;

    std::string                                     name{};
    SPIRVOpCode*                                    op{nullptr};
    SpvStorageClass                                 storage{SpvStorageClassMax};
    vk::DescriptorType                              descriptorType{}; // only valid for uniforms
    std::unordered_map<SpvDecoration, SPIRVOpCode*> decorations{};
    std::vector<SPIRVReflectionData>                members{}; // used by structs only
    uint32_t                                        structSize{0u};
  };

  template<class M, class K>
  bool MapContains( const M& map, const K& key ) const
  {
    return map.find( key ) != map.end();
  }


  template<class V>
  struct GetResult
  {
    GetResult( bool v, V& value)
    : success(v), result(&value)
    {
    }

    GetResult( bool v )
      : success(v), result(nullptr)
    {
    }

    operator V&()
    {
      return *result;
    }

    operator V*()
    {
      return result;
    }

    GetResult() : success( false ), result(nullptr){}
    bool  success;
    V*     result;
  };

  template<class K, class V>
  GetResult<V> GetMapItem( std::unordered_map<K,V>& map, const K& key )
  {
    auto iter = map.find( key );
    if( iter == map.end() )
    {
      return GetResult<V>( false );
    }

    return GetResult<V>( true, iter->second );
  }

  struct SPIRVTypeInfo
  {
    vk::Format  vkFormat;
    uint32_t    sizeInBytes;
    uint32_t    components;
    uint32_t    rows;
    uint32_t    componentSizeInBytes;
  };

  auto GetTypeInfo( const SPIRVOpCode& typeOpCode ) const
  {
    auto retval = SPIRVTypeInfo{};

    const vk::Format VEC[] =
                       {
                         vk::Format::eUndefined,
                         vk::Format::eR32Sfloat,
                         vk::Format::eR32G32Sfloat,
                         vk::Format::eR32G32B32Sfloat,
                         vk::Format::eR32G32B32A32Sfloat,
                       };

    // note: always assumed:
    // - not normalized
    // - float
    // - signed
    if( typeOpCode == SpvOpTypeMatrix )
    {
      retval.components = typeOpCode.GetParameterU32( 2 );
      retval.rows = retval.components;
      retval.componentSizeInBytes = sizeof(float);
      retval.sizeInBytes = (retval.components*retval.components)*retval.componentSizeInBytes;
      retval.vkFormat = VEC[retval.components];
    }
    else if(typeOpCode == SpvOpTypeVector)
    {
      retval.components = typeOpCode.GetParameterU32( 2 );
      retval.rows = 1;
      retval.componentSizeInBytes = sizeof(float);
      retval.sizeInBytes = retval.components*retval.componentSizeInBytes;
      retval.vkFormat = VEC[retval.components];
    }
    else if(typeOpCode == SpvOpTypeFloat)
    {
      retval.components = 1;
      retval.rows = 1;
      retval.componentSizeInBytes = sizeof(float);
      retval.sizeInBytes = sizeof(float);
      retval.vkFormat = vk::Format::eR32Sfloat;
    }
    else if(typeOpCode == SpvOpTypeInt)
    {
      retval.components = 1;
      retval.rows = 1;
      retval.componentSizeInBytes = sizeof(uint32_t);
      retval.sizeInBytes = sizeof(uint32_t);
      retval.vkFormat = vk::Format::eR32Sint;
    }
    else
    {
      retval.components = 0;
      retval.rows = 0;
      retval.componentSizeInBytes = 0;
      retval.sizeInBytes = 0;
      retval.vkFormat = vk::Format::eUndefined;
    }

    return retval;
  }



  auto BuildReflection()
  {
    // collect variables
    using MemberNameArray = std::vector<SPIRVOpCode*>;
    auto vars = std::vector<SPIRVOpCode*>{};
    auto opNames = std::unordered_map<uint32_t, SPIRVOpCode*>{};

    // member names, key: struct id, value: ordered vector of OpMemberName ops
    auto opMemberNames = std::unordered_map<uint32_t, MemberNameArray>{};
    for( auto&& op : opCodes )
    {
      auto id = op.GetParameterU32(0);
      if( op == SpvOpVariable )
      {
        vars.push_back( &op );
      }
      else if( op == SpvOpName )
      {
        opNames.emplace( id, &op );
      }
      else if( op == SpvOpMemberName )
      {
        GetResult<MemberNameArray> result{};
        MemberNameArray* memberNames{ nullptr };
        if( !(result = GetMapItem( opMemberNames, id )).success )
        {
          opMemberNames.emplace(id, MemberNameArray{} );
          memberNames = &opMemberNames[id];
        }
        else
        {
          memberNames = result.result;
        }

        if(memberNames->size() <= op.GetParameterU32(1))
          memberNames->resize( op.GetParameterU32(1)+1 );
        (*memberNames)[op.GetParameterU32(1)] = &op;
      }
    }

    // find uniforms and inputs
    auto decorationVariables = std::unordered_map<uint32_t, SPIRVReflectionData>{};
    auto uniformVariables = std::vector<SPIRVOpCode*>{};
    auto inputVariables   = std::vector<SPIRVOpCode*>{};
    auto outputVariables  = std::vector<SPIRVOpCode*>{};
    for( auto&& op : vars )
    {
      auto storage = op->GetParameter<SpvStorageClass>( 2 );
      bool varFound{false};
      if( storage == SpvStorageClassUniform || storage == SpvStorageClassUniformConstant )
      {
        uniformVariables.emplace_back( op );
        varFound = true;
      }
      else if( storage == SpvStorageClassInput )
      {
        inputVariables.emplace_back( op );
        varFound = true;
      }
      else if( storage == SpvStorageClassOutput )
      {
        outputVariables.emplace_back( op );
        varFound = true;
      }

      // find decorations if variable
      if( varFound )
      {
        auto id = op->localData.resultId;
        auto decorations = FindDecorationsForId( id );
        SPIRVReflectionData decorationInfo;
        decorationInfo.op = op;
        decorationInfo.storage = storage;

        // update descriptor type if viable
        decorationInfo.descriptorType = FindDescriptorTypeForVariable( *op );

        for( auto&& decoration : decorations )
        {
          auto decorationQualifier = decoration->GetParameter<SpvDecoration>( 1 );
          decorationInfo.decorations.emplace( decorationQualifier, decoration );
          std::cout << decorationQualifier << std::endl;
        }
        decorationVariables.emplace( id, decorationInfo );

        // store name if element is named
        GetResult<SPIRVOpCode*> name{};

        bool foundName = false;
        if( (name = GetMapItem( opNames, id )).success )
        {

          // variable may not be named ( global scope of the shader )
          if( !(*name.result)->GetParameterAsString( 1 ).empty() )
          {
            std::cout <<"Found name\n";
            decorationVariables[id].name = (*name.result)->GetParameterAsString( 1 );
            foundName = true;
          }
        }

        // continue if name hasn't been found, this means the variable is an uniform
        // in the global scope
        if( !foundName )
        {
          auto pointerId = op->GetParameterU32(0);
          auto pointer = FindByResultId( pointerId );
          auto pointerToType = FindByResultId( pointer.GetParameterU32(2) );

          // find name of the structure
          GetResult<SPIRVOpCode*> retval{};
          if( (retval = GetMapItem( opNames, pointerToType.localData.resultId )).success )
          {
            std::cout << "Found: " << (*retval.result)->GetParameterAsString(1) << std::endl;
            decorationVariables[id].name = (*retval.result)->GetParameterAsString(1);
          }

          // depending on the type, we may need to extract members, member types as well
          // as other relevant data
          if( pointerToType == SpvOpTypeStruct )
          {

            auto memberCount = pointerToType.localData.count-2;
            std::cout << "Found struct, look for member names and member decorations: "
                      "member count: " << memberCount << std::endl;

            // for each member resolve type and compute size of the structure
            auto memberNames = opMemberNames[ pointerToType.localData.resultId ];
            for( auto i = 0u; i < memberCount; ++i )
            {
              auto& memberName = memberNames[i];
              SPIRVReflectionData memberOpInfo;
              memberOpInfo.name = memberName->GetParameterAsString(2);
              auto memberResultId = pointerToType.GetParameterU32( i+1 );
              memberOpInfo.op = FindByResultPtrId( memberResultId );

              // look for decoration for each member ( needed in order to build data structures )
              auto memberDecorationOps = FindMemberDecorationsForId( pointerToType.localData.resultId, i );
              for( auto&& mop : memberDecorationOps )
              {
                memberOpInfo.decorations.emplace( mop->GetParameter<SpvDecoration>( 2 ), mop );
              }
              decorationVariables[id].members.emplace_back(memberOpInfo);
              std::cout << "memberName: " << memberName->GetParameterAsString(2);
              std::cout << std::endl;
            }

            uint32_t structSize = 0u;

            // for last member update size of the data structure ( for blocks only )
            if(memberCount)
            {
              if( memberCount > 0 )
              {
                auto& lastMember = decorationVariables[id].members.back();

                if( MapContains( lastMember.decorations, SpvDecorationOffset ) )
                {
                  structSize = lastMember.decorations[SpvDecorationOffset]->GetParameterU32(3);
                }
                auto typeInfo = GetTypeInfo( *lastMember.op );
                structSize += typeInfo.sizeInBytes;
              }
              decorationVariables[id].structSize = structSize;
            }
            std::cout << "struct size: " << structSize << std::endl;
          }
        }
      }
    }

    std::cout << "Found " << uniformVariables.size() << " variables\n";

    return decorationVariables;
  }

  auto LoadOpCodes( const std::vector<SPIRVWord>& _data )
  {
    auto retval = std::vector<SPIRVOpCode>{};

    // test if we have valid SPIRV header
    auto iter = data.begin();
    if( !CheckHeader() )
    {
      debug( "Not SPIRV!" );
      return retval;
    }

    debug( "SPIR-V detected" );
    std::advance( iter, 5u ); // skip header

    while( iter != data.end() )
    {
      auto opword    = *iter;
      auto wordCount = ( ( opword >> 16 ) & 0xFFFF );
      auto opCode    = ( (opword)&0xFFFF );

      auto& op = FindOpCode( opCode );

      if( op != OP_CODE_NULL )
      {
        uint32_t resultIndex{0};
        int      resultIndexOffset = 1;
        int32_t  resultType{0u};

        // make a copy
        retval.emplace_back( op );
        auto& opcode           = retval.back();
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
            resultType = static_cast<int32_t>( *( iter + 1 ) );
          }
          resultIndex                 = *( iter + resultIndexOffset );
          opcode.localData.resultId   = resultIndex;
          opcode.localData.resultType = resultType;
        }
      }

      // next instruction
      std::advance( iter, wordCount );
    }

    return retval;
  }

  auto CreateOpResults( std::vector<SPIRVOpCode>& _opcodes )
  {
    auto retval = std::vector<SPIRVOpCode*>{};
    for( auto i = 0u; i < _opcodes.size(); ++i )
    {
      const auto& op = _opcodes[i];
      if( op.hasResult )
      {
        if( retval.size() <= op.localData.resultId )
        {
          retval.resize( op.localData.resultId + 1 );
        }
        retval[op.localData.resultId] = &_opcodes[i];
      }
    }
    return retval;
  }

  vk::DescriptorType FindDescriptorTypeForVariable( SPIRVOpCode& opVariable )
  {
    vk::DescriptorType descriptorType{};

    auto storageClass = opVariable.GetParameter<SpvStorageClass>(2);

    // we need to detect storage by call into function
    if (storageClass == SpvStorageClassUniformConstant)
    {
      auto& resource = opVariable;
      if (TestStorageImageDescriptor(resource))
      {
        descriptorType = vk::DescriptorType::eStorageImage;
      }
      else if(TestSamplerDescriptor(resource))
      {
        descriptorType = vk::DescriptorType::eSampler;
      }
      else if(TestSampledImageDescriptor(resource))
      {
        descriptorType = vk::DescriptorType::eSampledImage;
      }
      else if(TestCombinedImageSamplerDescriptor(resource))
      {
        descriptorType = vk::DescriptorType::eCombinedImageSampler;
      }
      else if(TestUniformTexelBufferDescriptor(resource))
      {
        descriptorType = vk::DescriptorType::eUniformTexelBuffer;
      }
      else if(TestStorageTexelBufferDescriptor(resource))
      {
        descriptorType = vk::DescriptorType::eStorageTexelBuffer;
      }
      else
      {
        // @todo check the shader, something hasn't been recognized
        descriptorType = vk::DescriptorType{};
      }

      if(descriptorType != vk::DescriptorType{})
      {
        //uniformResources.push_back( &resource );
      }
    }
    else if(storageClass == SpvStorageClassUniform)
    {
      descriptorType = vk::DescriptorType::eUniformBuffer;
    }
    return descriptorType;
  }

  auto GenerateVulkanDescriptorSetLayouts()
  {
    // key: descriptor set, value: ds layout create info and binding
    auto vkDescriptorSetLayoutCreateInfos = std::unordered_map<uint32_t, DescriptorSetLayoutAndBindingInfo>{};

    for(auto&& symbol : reflectionData)
    {
      auto storage = symbol.second.storage;
      auto& symbolData = symbol.second;
      if( storage == SpvStorageClassUniform || storage == SpvStorageClassUniformConstant )
      {
        auto binding = MapContains( symbolData.decorations, SpvDecorationBinding ) ? symbolData.decorations[SpvDecorationBinding]->GetParameterU32(2) : 0u;
        auto descriptorSet = MapContains( symbolData.decorations, SpvDecorationDescriptorSet ) ? symbolData.decorations[SpvDecorationDescriptorSet]->GetParameterU32(2) : 0u;
        debug("found layout: binding: " << binding << " ds: " << descriptorSet << ", type: " << U32(symbolData.descriptorType) );

        auto& ds = (MapContains( vkDescriptorSetLayoutCreateInfos, descriptorSet ) ?
                    vkDescriptorSetLayoutCreateInfos[descriptorSet] :
                    (*vkDescriptorSetLayoutCreateInfos.emplace( descriptorSet, DescriptorSetLayoutAndBindingInfo{} ).first).second);


        ds.bindings.emplace_back( vk::DescriptorSetLayoutBinding{}.setBinding( binding )
                                                                  .setDescriptorCount( 1 )
                                                                  .setDescriptorType( symbolData.descriptorType )
                                                                  .setStageFlags( vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment ) );
      }
    }

    // sort bindings and complete create info structures
    for( auto&& ds : vkDescriptorSetLayoutCreateInfos )
    {
      std::sort(ds.second.bindings.begin(), ds.second.bindings.end(), []( auto& a, auto& b ){ return a.binding < b.binding; });
      ds.second.createInfo.setBindingCount( U32(ds.second.bindings.size()) );
      ds.second.createInfo.pBindings = ds.second.bindings.data();
    }

    return vkDescriptorSetLayoutCreateInfos;
  }

  bool Initialise()
  {
    // load opcodes
    opCodes = std::move(LoadOpCodes(data));

    // create results lookup array
    opResults = std::move(CreateOpResults(opCodes));

    // build reflection map
    reflectionData = std::move(BuildReflection());

    // build vulkan descriptor set layout infos
    descriptorSetLayoutCreateInfoMap = std::move(GenerateVulkanDescriptorSetLayouts());

    // generate additional reflection structures
    uint32_t blockIndex = 0u;
    for( auto&& symbol : reflectionData )
    {
      auto& symbolData = symbol.second;
      auto binding = MapContains( symbolData.decorations, SpvDecorationBinding ) ? symbolData.decorations[SpvDecorationBinding]->GetParameterU32(2) : 0u;
      auto descriptorSet = MapContains( symbolData.decorations, SpvDecorationDescriptorSet ) ? symbolData.decorations[SpvDecorationDescriptorSet]->GetParameterU32(2) : 0u;

      if( symbolData.storage == SpvStorageClassUniform )
      {
        auto block = SPIRVUniformBlock{};
        block.name = symbolData.name;
        block.size = symbolData.structSize;
        block.binding = binding;
        block.descriptorSet = descriptorSet;

        uint32_t location{ 0u };
        for( auto&& member : symbolData.members )
        {
          auto blockMember = SPIRVUniformBlockMember{};
          blockMember.name = member.name;
          blockMember.location = location++;
          blockMember.offset = MapContains( member.decorations, SpvDecorationOffset ) ? member.decorations[SpvDecorationOffset]->GetParameterU32(3) : 0u;
          blockMember.blockIndex = blockIndex;
          block.members.emplace_back( blockMember );
        }
        blockIndex++;
        uniformBlockReflection.emplace_back( block );

      }
      else if( symbolData.storage == SpvStorageClassUniformConstant )
      {
        auto opaque = SPIRVUniformOpaque{};
        opaque.name = symbolData.name;
        opaque.binding = binding;
        opaque.descriptorSet = descriptorSet;
        opaque.type =  symbolData.descriptorType;
        uniformOpaqueReflection.emplace_back( opaque );
      }
    }

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
  bool TestStorageImageDescriptor( SPIRVOpCode& opcode )
  {
    auto opPointer = &FindByResultId( opcode.GetParameterU32(0) );
    if( (opPointer && *opPointer == SpvOpTypePointer) )
    {
      auto& opTypeImage = GetReferencedOpCode( *opPointer, 2 );
      if( opTypeImage == SpvOpTypeImage && opTypeImage.GetParameterU32( 6 ) == 2 )
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
  bool TestSamplerDescriptor( SPIRVOpCode& opcode )
  {
    auto opPointer = &FindByResultId( opcode.GetParameterU32(0) );
    if( (opPointer && *opPointer == SpvOpTypePointer) )
    {
      auto& opTypeSampler = GetReferencedOpCode( *opPointer, 2 );
      if( opTypeSampler == SpvOpTypeSampler )
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
  bool TestSampledImageDescriptor( SPIRVOpCode& opcode)
  {
    auto opPointer = &FindByResultId( opcode.GetParameterU32(0) );
    if( (opPointer && *opPointer == SpvOpTypePointer) )
    {
      auto& opTypeImage = GetReferencedOpCode( *opPointer, 2 );
      if( opTypeImage == SpvOpTypeImage && opTypeImage.GetParameterU32( 6 ) == 1 )
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
  bool TestCombinedImageSamplerDescriptor( SPIRVOpCode& opcode )
  {
    auto opPointer = &FindByResultId( opcode.GetParameterU32(0) );
    if( (opPointer && *opPointer == SpvOpTypePointer) )
    {
      auto& opCode = GetReferencedOpCode( *opPointer, 2 );
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
  bool TestUniformTexelBufferDescriptor( SPIRVOpCode& opcode  )
  {
    auto opPointer = &FindByResultId( opcode.GetParameterU32(0) );
    if( (opPointer && *opPointer == SpvOpTypePointer) )
    {
      auto& opCode = GetReferencedOpCode( *opPointer, 2 );
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
  bool TestStorageTexelBufferDescriptor( SPIRVOpCode& opcode )
  {
    auto opPointer = &FindByResultId( opcode.GetParameterU32(0) );
    if( (opPointer && *opPointer == SpvOpTypePointer) )
    {
      auto& opCode = GetReferencedOpCode( *opPointer, 2 );
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
  bool TestUniformBufferDescriptor( SPIRVOpCode& opcode )
  {
    auto opPointer = &FindByResultId( opcode.GetParameterU32(0) );
    if( (opPointer && *opPointer == SpvOpTypePointer) && opPointer->GetParameter<SpvStorageClass>( 1 ) == SpvStorageClassUniform )
    {
      auto& opTypeStruct = GetReferencedOpCode( *opPointer, 2 );
      if( opTypeStruct == SpvOpTypeStruct )
      {
        return CheckDecorationForOpId( opTypeStruct, SpvDecorationBlock );
      }
    }
    return false;
  }

  bool TestStorageBufferDescriptor( SPIRVOpCode& opcode )
  {
    auto opPointer = FindByResultId( opcode );
    if( (opPointer && *opPointer == SpvOpTypePointer) && opPointer->GetParameter<SpvStorageClass>( 1 ) == SpvStorageClassUniform )
    {
      auto& opTypeStruct = GetReferencedOpCode( *opPointer, 2 );
      if( opTypeStruct == SpvOpTypeStruct )
      {
        return CheckDecorationForOpId( opTypeStruct, SpvDecorationBufferBlock );
      }
    }
    return false;
  }

  std::vector<vk::DescriptorSetLayoutCreateInfo> GenerateDescriptorSetLayoutCreateInfo() const
  {
    auto retval = std::vector<vk::DescriptorSetLayoutCreateInfo>{};
    for( auto& layout : descriptorSetLayoutCreateInfoMap )
    {
      retval.emplace_back( layout.second.createInfo );
    }

    return retval;
  }

  bool GetVertexInputAttributes( std::vector<SPIRVVertexInputAttribute>& out, bool canOverlap = false )
  {
    for( auto&& i : reflectionData )
    {
      if( i.second.storage == SpvStorageClassInput )
      {
        auto attr =  SPIRVVertexInputAttribute{};
        attr.name = i.second.name;
        attr.location = MapContains( i.second.decorations, SpvDecorationLocation ) ?
                        i.second.decorations[SpvDecorationLocation]->GetParameterU32(2) : 0u;
        attr.format = GetTypeInfo(
          GetReferencedOpCode( GetReferencedOpCode( *i.second.op, 0 ), 2)
        ).vkFormat;
        out.emplace_back( attr );
      }
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
  std::vector<SPIRVOpCode>  opCodes;   // contains all opcodes
  std::vector<SPIRVOpCode*> opResults; // links to the resulting opcode or nullptr if opcode doesn't return

  std::unordered_map<uint32_t, SPIRVReflectionData> reflectionData;
  std::unordered_map<uint32_t, DescriptorSetLayoutAndBindingInfo>      descriptorSetLayoutCreateInfoMap;

  std::vector<SPIRVWord>    data;


  std::vector<SPIRVUniformBlock>                                       uniformBlockReflection;
  std::vector<SPIRVUniformOpaque>                                      uniformOpaqueReflection;
  Header                                                               header;

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
  auto                   wordSize = sizeInBytes / sizeof( SPIRVWord );
  spirvCode.resize( wordSize );
  std::copy( data, data + wordSize, spirvCode.begin() );
  return Parse( spirvCode, stages );
}

} // namespace SpirV

} // namespace Vulkan

} // namespace Graphics

} // namespace Dali