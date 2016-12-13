#include <dali/public-api/dali-core.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/update/manager/geometry-batcher.h>

#include <dali-test-suite-utils.h>

namespace
{

class MockActor : public Dali::Internal::Actor
{
public:
  inline const Dali::Internal::SceneGraph::Node* GetNode()
  {
    return mNode;
  }

  inline const bool IsNodeBatchParent()
  {
    return mNode->mIsBatchParent;
  }
};

Geometry CreateBatchQuadGeometryVector2( Vector4 texCoords )
{
  const float halfWidth = 0.5f;
  const float halfHeight = 0.5f;
  struct QuadVertex {
    QuadVertex( const Vector2& vertexPosition, const Vector2& vertexTexCoords )
      : position( vertexPosition ),
        texCoords( vertexTexCoords )
    {}
    Vector2 position;
    Vector2 texCoords;
  };

  // special case, when texture takes whole space
  if( texCoords == Vector4::ZERO )
  {
    texCoords = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
  }

  QuadVertex quadVertexData[6] =
  {
    QuadVertex( Vector2(-halfWidth,   -halfHeight ),   Vector2(texCoords.x, texCoords.y) ),
    QuadVertex( Vector2( halfWidth,   -halfHeight ),   Vector2(texCoords.z, texCoords.y) ),
    QuadVertex( Vector2(-halfWidth,    halfHeight ),   Vector2(texCoords.x, texCoords.w) ),
    QuadVertex( Vector2( halfWidth,   -halfHeight ),   Vector2(texCoords.z, texCoords.y) ),
    QuadVertex( Vector2(-halfWidth,    halfHeight ),   Vector2(texCoords.x, texCoords.w) ),
    QuadVertex( Vector2( halfWidth,    halfHeight ),   Vector2(texCoords.z, texCoords.w) ),
  };

  Property::Map vertexFormat;
  vertexFormat[ "aPosition" ] = Property::VECTOR2;
  vertexFormat[ "aTexCoord" ] = Property::VECTOR2;
  PropertyBuffer vertexBuffer = PropertyBuffer::New( vertexFormat );
  vertexBuffer.SetData( quadVertexData, 6 );

  // create geometry as normal, single quad
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( vertexBuffer );
  geometry.SetType( Geometry::TRIANGLES );

  return geometry;
}

Geometry CreateBatchQuadGeometryVector3( Vector4 texCoords )
{
  const float halfWidth = 0.5f;
  const float halfHeight = 0.5f;
  struct QuadVertex {
    QuadVertex( const Vector3& vertexPosition, const Vector2& vertexTexCoords )
      : position( vertexPosition ),
        texCoords( vertexTexCoords )
    {}
    Vector3 position;
    Vector2 texCoords;
  };

  // special case, when texture takes whole space
  if( texCoords == Vector4::ZERO )
  {
    texCoords = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
  }

  QuadVertex quadVertexData[6] =
  {
    QuadVertex( Vector3(-halfWidth,   -halfHeight, 0.0f ),   Vector2(texCoords.x, texCoords.y) ),
    QuadVertex( Vector3( halfWidth,   -halfHeight, 0.0f ),   Vector2(texCoords.z, texCoords.y) ),
    QuadVertex( Vector3(-halfWidth,    halfHeight, 0.0f ),   Vector2(texCoords.x, texCoords.w) ),
    QuadVertex( Vector3( halfWidth,   -halfHeight, 0.0f ),   Vector2(texCoords.z, texCoords.y) ),
    QuadVertex( Vector3(-halfWidth,    halfHeight, 0.0f ),   Vector2(texCoords.x, texCoords.w) ),
    QuadVertex( Vector3( halfWidth,    halfHeight, 0.0f ),   Vector2(texCoords.z, texCoords.w) ),
  };

  Property::Map vertexFormat;
  vertexFormat[ "aPosition" ] = Property::VECTOR3;
  vertexFormat[ "aTexCoord" ] = Property::VECTOR2;
  PropertyBuffer vertexBuffer = PropertyBuffer::New( vertexFormat );
  vertexBuffer.SetData( quadVertexData, 6 );

  // create geometry as normal, single quad
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( vertexBuffer );
  geometry.SetType( Geometry::TRIANGLES );
  return geometry;
}

Geometry CreateBatchQuadGeometryVector4( Vector4 texCoords )
{
  const float halfWidth = 0.5f;
  const float halfHeight = 0.5f;
  struct QuadVertex {
    QuadVertex( const Vector4& vertexPosition, const Vector2& vertexTexCoords )
      : position( vertexPosition ),
        texCoords( vertexTexCoords )
    {}
    Vector4 position;
    Vector2 texCoords;
  };

  // special case, when texture takes whole space
  if( texCoords == Vector4::ZERO )
  {
    texCoords = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
  }

  QuadVertex quadVertexData[6] =
  {
    QuadVertex( Vector4(-halfWidth,   -halfHeight, 0.0f, 1.0f ),   Vector2(texCoords.x, texCoords.y) ),
    QuadVertex( Vector4( halfWidth,   -halfHeight, 0.0f, 1.0f ),   Vector2(texCoords.z, texCoords.y) ),
    QuadVertex( Vector4(-halfWidth,    halfHeight, 0.0f, 1.0f ),   Vector2(texCoords.x, texCoords.w) ),
    QuadVertex( Vector4( halfWidth,   -halfHeight, 0.0f, 1.0f ),   Vector2(texCoords.z, texCoords.y) ),
    QuadVertex( Vector4(-halfWidth,    halfHeight, 0.0f, 1.0f ),   Vector2(texCoords.x, texCoords.w) ),
    QuadVertex( Vector4( halfWidth,    halfHeight, 0.0f, 1.0f ),   Vector2(texCoords.z, texCoords.w) ),
  };

  Property::Map vertexFormat;
  vertexFormat[ "aPosition" ] = Property::VECTOR4;
  vertexFormat[ "aTexCoord" ] = Property::VECTOR2;
  PropertyBuffer vertexBuffer = PropertyBuffer::New( vertexFormat );
  vertexBuffer.SetData( quadVertexData, 6 );

  // create geometry as normal, single quad
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( vertexBuffer );
  geometry.SetType( Geometry::TRIANGLES );
  return geometry;
}


void CreateShadersAndTextureSets( Shader* shaders, size_t shaderCount, TextureSet* textureSets, size_t textureSetCount )
{
  for( size_t i = 0; i < shaderCount; ++i )
  {
    shaders[i] = Shader::New( "", "" );
  }

  for( size_t i = 0; i < textureSetCount; ++i )
  {
    textureSets[i] = TextureSet::New();
  }
}

Vector4 WHOLE_IMAGE( 0.0f, 0.0f, 1.0f, 1.0f );

Actor CreateActor( Actor& parent, Shader& shader, TextureSet& textureSet, Vector3 position, Vector4 texCoords, Geometry(*geomfunc)(Vector4) = CreateBatchQuadGeometryVector2 )
{
  Geometry geometry = geomfunc( texCoords );
  Renderer renderer = Renderer::New( geometry, shader );

  renderer.SetTextures( textureSet );
  renderer.SetProperty( Dali::DevelRenderer::Property::BATCHING_ENABLED, true );

  Actor actor = Actor::New();
  actor.SetPosition( position );
  parent.Add( actor );
  actor.AddRenderer( renderer );
  return actor;
}

Actor CreateBatchParent( Vector3 pos )
{
  Actor actor = Actor::New();
  actor.SetProperty( DevelActor::Property::BATCH_PARENT, true );
  actor.SetPosition( pos );
  Stage::GetCurrent().Add( actor );
  return actor;
}

} // namespace

int UtcDaliGeometryBatcherBatchLevel0(void)
{
  TestApplication app;
  TestGlAbstraction& glAbstraction = app.GetGlAbstraction();
  glAbstraction.EnableDrawCallTrace( true );
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();

  Shader shaders[1];
  TextureSet textureSets[1];

  CreateShadersAndTextureSets( shaders, 1, textureSets, 1 );
  Actor batchParent = CreateBatchParent( Vector3::ZERO );
  batchParent.SetSize( Stage::GetCurrent().GetSize() );

  Actor children[] = {
    CreateActor( batchParent, shaders[0], textureSets[0], Vector3( 0.0f, 0.0f, 0.0f ), WHOLE_IMAGE ),
    CreateActor( batchParent, shaders[0], textureSets[0], Vector3( 10.0f, 0.0f, 0.0f ), WHOLE_IMAGE ),
    CreateActor( batchParent, shaders[0], textureSets[0], Vector3( 20.0f, 0.0f, 0.0f ), WHOLE_IMAGE ),
    CreateActor( batchParent, shaders[0], textureSets[0], Vector3( 30.0f, 0.0f, 0.0f ), WHOLE_IMAGE )
  };

  app.SendNotification();
  app.Render( 16 );
  app.SendNotification();
  app.Render( 16 );

  // should be 1 draw call
  {
    int result = drawTrace.CountMethod( "DrawElements");
    DALI_TEST_CHECK( result == 1 );
  }

  // remove actor
  batchParent.Remove( children[0] );
  children[0].Reset();

  // update
  app.SendNotification();
  app.Render( 16 );

  // test geometry for that batch, 1 batch, 3 children, 18 elements in the buffer
  //Dali::Internal::Actor& actor = GetImplementation( children[1] );
  MockActor* mockActor = static_cast<MockActor*>( &GetImplementation( children[1] ) );
  Dali::Internal::SceneGraph::GeometryBatcher* geometryBatcher = mockActor->GetNode()->mGeometryBatcher;
  DALI_TEST_CHECK( geometryBatcher ); // must not be NULL

  Dali::Internal::Render::Geometry* geometry = geometryBatcher->GetGeometry( 0 );
  int elementCount = geometry->GetPropertyBuffer(0)->GetElementCount();
  DALI_TEST_CHECK( elementCount == 18 );

  // delete batch parent
  Stage::GetCurrent().Remove( batchParent );
  batchParent.Reset();

  // update
  app.SendNotification();
  app.Render( 16 );

  END_TEST;
}

int UtcDaliGeometryBatcherBatchMultipleTextureSet(void)
{
  TestApplication app;
  TestGlAbstraction& glAbstraction = app.GetGlAbstraction();
  glAbstraction.EnableDrawCallTrace( true );
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();

  Shader shaders[1];
  TextureSet textureSets[3];
  CreateShadersAndTextureSets( shaders, 1, textureSets, 3 );

  Actor batchParent = CreateBatchParent( Vector3::ZERO );
  batchParent.SetSize( Stage::GetCurrent().GetSize() );

  Actor children[] = {
    CreateActor( batchParent, shaders[0], textureSets[0], Vector3( 0.0f, 0.0f, 0.0f ), WHOLE_IMAGE ),
    CreateActor( batchParent, shaders[0], textureSets[0], Vector3( 10.0f, 0.0f, 0.0f ), WHOLE_IMAGE ),
    CreateActor( batchParent, shaders[0], textureSets[0], Vector3( 20.0f, 0.0f, 0.0f ), WHOLE_IMAGE ),
    CreateActor( batchParent, shaders[0], textureSets[0], Vector3( 30.0f, 0.0f, 0.0f ), WHOLE_IMAGE ),
    CreateActor( batchParent, shaders[0], textureSets[1], Vector3( 0.0f, 0.0f, 0.0f ), WHOLE_IMAGE ),
    CreateActor( batchParent, shaders[0], textureSets[2], Vector3( 10.0f, 0.0f, 0.0f ), WHOLE_IMAGE ),
    CreateActor( batchParent, shaders[0], textureSets[2], Vector3( 20.0f, 0.0f, 0.0f ), WHOLE_IMAGE ),
    CreateActor( batchParent, shaders[0], textureSets[1], Vector3( 30.0f, 0.0f, 0.0f ), WHOLE_IMAGE ),
    CreateActor( batchParent, shaders[0], textureSets[1], Vector3( 0.0f, 0.0f, 0.0f ), WHOLE_IMAGE ),
    CreateActor( batchParent, shaders[0], textureSets[1], Vector3( 10.0f, 0.0f, 0.0f ), WHOLE_IMAGE ),
    CreateActor( batchParent, shaders[0], textureSets[2], Vector3( 20.0f, 0.0f, 0.0f ), WHOLE_IMAGE ),
    CreateActor( batchParent, shaders[0], textureSets[2], Vector3( 30.0f, 0.0f, 0.0f ), WHOLE_IMAGE )
  };

  // must update twice
  app.SendNotification();
  app.Render( 16 );
  app.SendNotification();
  app.Render( 16 );

  // should be 3 draw calls here
  {
    int result = drawTrace.CountMethod( "DrawElements");
    DALI_TEST_CHECK( result == 3 );
  }

  // test assigned indices
  {
    bool indicesTest( true );
    for( size_t i = 0; i < 12; ++i )
    {
      MockActor* mockActor = static_cast<MockActor*>( &GetImplementation( children[1] ) );
      if( mockActor->GetNode()->mBatchIndex == BATCH_NULL_HANDLE )
      {
        indicesTest = false;
      }
    }
    DALI_TEST_CHECK( indicesTest );
  }

  END_TEST;
}

int UtcDaliGeometryBatcherSettingBatchParent(void)
{
  TestApplication app;

  Shader shaders[1];
  TextureSet textureSets[1];
  CreateShadersAndTextureSets( shaders, 1, textureSets, 1 );

  Actor batchParent = CreateBatchParent( Vector3::ZERO );
  batchParent.SetSize( Stage::GetCurrent().GetSize() );
  app.SendNotification();
  app.Render( 16 );

  MockActor* mockActor = static_cast<MockActor*>( &GetImplementation( batchParent ) );
  DALI_TEST_CHECK( mockActor->IsNodeBatchParent() );

  END_TEST;
}

int UtcDaliGeometryBatcherBatchMultipleParents(void)
{
  TestApplication app;
  TestGlAbstraction& glAbstraction = app.GetGlAbstraction();
  glAbstraction.EnableDrawCallTrace( true );
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();

  Shader shaders[2];
  TextureSet textureSets[2];
  CreateShadersAndTextureSets( shaders, 2, textureSets, 2 );

  Actor batchParent0 = CreateBatchParent( Vector3::ZERO ); // Vector2
  batchParent0.SetSize( Stage::GetCurrent().GetSize() );
  Actor batchParent1 = CreateBatchParent( Vector3::ZERO ); // Vector3
  batchParent1.SetSize( Stage::GetCurrent().GetSize() );
  Actor batchParent2 = CreateBatchParent( Vector3::ZERO ); // Vector4
  batchParent2.SetSize( Stage::GetCurrent().GetSize() );

  CreateActor( batchParent0, shaders[0], textureSets[0], Vector3( 0.0f, 0.0f, 0.0f ), WHOLE_IMAGE );
  CreateActor( batchParent0, shaders[0], textureSets[0], Vector3( 10.0f, 0.0f, 0.0f ), WHOLE_IMAGE );
  CreateActor( batchParent0, shaders[0], textureSets[0], Vector3( 20.0f, 0.0f, 0.0f ), WHOLE_IMAGE );
  CreateActor( batchParent0, shaders[0], textureSets[0], Vector3( 30.0f, 0.0f, 0.0f ), WHOLE_IMAGE );
  CreateActor( batchParent0, shaders[0], textureSets[0], Vector3( 0.0f, 0.0f, 0.0f ), WHOLE_IMAGE );
  CreateActor( batchParent1, shaders[1], textureSets[1], Vector3( 0.0f, 0.0f, 0.0f ), WHOLE_IMAGE,  CreateBatchQuadGeometryVector3 );
  CreateActor( batchParent1, shaders[1], textureSets[1], Vector3( 10.0f, 0.0f, 0.0f ), WHOLE_IMAGE, CreateBatchQuadGeometryVector3 );
  CreateActor( batchParent1, shaders[1], textureSets[1], Vector3( 20.0f, 0.0f, 0.0f ), WHOLE_IMAGE, CreateBatchQuadGeometryVector3 );
  CreateActor( batchParent2, shaders[0], textureSets[1], Vector3( 30.0f, 0.0f, 0.0f ), WHOLE_IMAGE, CreateBatchQuadGeometryVector4 );
  CreateActor( batchParent2, shaders[0], textureSets[1], Vector3( 0.0f, 0.0f, 0.0f ), WHOLE_IMAGE,  CreateBatchQuadGeometryVector4 );
  CreateActor( batchParent2, shaders[0], textureSets[1], Vector3( 30.0f, 0.0f, 0.0f ), WHOLE_IMAGE, CreateBatchQuadGeometryVector4 );
  CreateActor( batchParent2, shaders[0], textureSets[1], Vector3( 0.0f, 0.0f, 0.0f ), WHOLE_IMAGE,  CreateBatchQuadGeometryVector4 );

  // must update twice
  app.SendNotification();
  app.Render( 16 );
  app.SendNotification();
  app.Render( 16 );

  // should be 3 draw calls here
  {
    int result = drawTrace.CountMethod( "DrawElements");
    DALI_TEST_EQUALS( result, 3, TEST_LOCATION );
  }

  // delete batch parent
  Stage::GetCurrent().Remove( batchParent1 );
  batchParent1.Reset();
  drawTrace.Reset();
  app.SendNotification();
  app.Render( 16 );
  // should be 2 draw calls here
  {
    int result = drawTrace.CountMethod( "DrawElements");
    DALI_TEST_EQUALS( result, 2, TEST_LOCATION );
  }

  END_TEST;
}
