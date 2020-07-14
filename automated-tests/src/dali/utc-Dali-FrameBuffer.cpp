/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>
#include <dali/devel-api/rendering/frame-buffer-devel.h>

using namespace Dali;

#include <mesh-builder.h>

void framebuffer_set_startup(void)
{
  test_return_value = TET_UNDEF;
}

void framebuffer_set_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliFrameBufferNew01(void)
{
  TestApplication application;

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::NONE );

  DALI_TEST_CHECK( frameBuffer );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferColorAttachmentCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferDepthAttachment(), (GLenum)GL_FALSE, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferStencilAttachment(), (GLenum)GL_FALSE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameBufferNew02(void)
{
  TestApplication application;

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::DEPTH );

  DALI_TEST_CHECK( frameBuffer );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferColorAttachmentCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferDepthAttachment(), (GLenum)GL_TRUE, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferStencilAttachment(), (GLenum)GL_FALSE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameBufferNew03(void)
{
  TestApplication application;

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::STENCIL );

  DALI_TEST_CHECK( frameBuffer );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferColorAttachmentCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferDepthAttachment(), (GLenum)GL_FALSE, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferStencilAttachment(), (GLenum)GL_TRUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameBufferNew04(void)
{
  TestApplication application;

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::DEPTH_STENCIL );

  DALI_TEST_CHECK( frameBuffer );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferColorAttachmentCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferDepthAttachment(), (GLenum)GL_TRUE, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferStencilAttachment(), (GLenum)GL_TRUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameBufferNew05(void)
{
  TestApplication application;
  FrameBuffer frameBuffer;
  DALI_TEST_CHECK( !frameBuffer );
  END_TEST;
}

int UtcDaliFrameBufferNew06(void)
{
  TestApplication application;

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, static_cast<FrameBuffer::Attachment::Mask>( FrameBuffer::Attachment::DEPTH | FrameBuffer::Attachment::STENCIL ) );

  DALI_TEST_CHECK( frameBuffer );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferColorAttachmentCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferDepthAttachment(), (GLenum)GL_TRUE, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferStencilAttachment(), (GLenum)GL_TRUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameBufferNewWithColor01(void)
{
  TestApplication application;
  uint32_t width = 64;
  uint32_t height = 64;
  FrameBuffer frameBuffer = FrameBuffer::New( width, height );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferColorAttachmentCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferDepthAttachment(), (GLenum)GL_FALSE, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferStencilAttachment(), (GLenum)GL_FALSE, TEST_LOCATION);
  // check that texture is not empty handle
  DALI_TEST_CHECK( frameBuffer.GetColorTexture() );
  END_TEST;
}

int UtcDaliFrameBufferNewWithColor02(void)
{
  TestApplication application;
  uint32_t width = 64;
  uint32_t height = 64;
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::COLOR );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferColorAttachmentCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferDepthAttachment(), (GLenum)GL_FALSE, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferStencilAttachment(), (GLenum)GL_FALSE, TEST_LOCATION);
  // check that texture is not empty handle
  DALI_TEST_CHECK( frameBuffer.GetColorTexture() );
  END_TEST;
}

int UtcDaliFrameBufferNewWithColor03(void)
{
  TestApplication application;
  uint32_t width = 64;
  uint32_t height = 64;
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::COLOR_DEPTH );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferColorAttachmentCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferDepthAttachment(), (GLenum)GL_TRUE, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferStencilAttachment(), (GLenum)GL_FALSE, TEST_LOCATION);
  // check that texture is not empty handle
  DALI_TEST_CHECK( frameBuffer.GetColorTexture() );
  END_TEST;
}

int UtcDaliFrameBufferNewWithColor04(void)
{
  TestApplication application;
  uint32_t width = 64;
  uint32_t height = 64;
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::COLOR_STENCIL );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferColorAttachmentCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferDepthAttachment(), (GLenum)GL_FALSE, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferStencilAttachment(), (GLenum)GL_TRUE, TEST_LOCATION);
  // check that texture is not empty handle
  DALI_TEST_CHECK( frameBuffer.GetColorTexture() );
  END_TEST;
}

int UtcDaliFrameBufferNewWithColor05(void)
{
  TestApplication application;
  uint32_t width = 64;
  uint32_t height = 64;
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::COLOR_DEPTH_STENCIL );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferColorAttachmentCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferDepthAttachment(), (GLenum)GL_TRUE, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferStencilAttachment(), (GLenum)GL_TRUE, TEST_LOCATION);
  // check that texture is not empty handle
  DALI_TEST_CHECK( frameBuffer.GetColorTexture() );
  END_TEST;
}

int UtcDaliFrameBufferCopyConstructor(void)
{
  TestApplication application;

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::NONE );

  FrameBuffer frameBufferCopy( frameBuffer );

  DALI_TEST_CHECK( frameBufferCopy );

  END_TEST;
}

int UtcDaliFrameBufferAssignmentOperator(void)
{
  TestApplication application;

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::NONE );

  FrameBuffer frameBuffer2;
  DALI_TEST_CHECK( !frameBuffer2 );

  frameBuffer2 = frameBuffer;
  DALI_TEST_CHECK( frameBuffer2 );

  END_TEST;
}

int UtcDaliFrameBufferDownCast01(void)
{
  TestApplication application;
  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::NONE );

  BaseHandle handle(frameBuffer);
  FrameBuffer frameBuffer2 = FrameBuffer::DownCast(handle);
  DALI_TEST_CHECK( frameBuffer2 );

  END_TEST;
}

int UtcDaliFrameBufferDownCast02(void)
{
  TestApplication application;

  Handle handle = Handle::New(); // Create a custom object
  FrameBuffer frameBuffer = FrameBuffer::DownCast(handle);
  DALI_TEST_CHECK( !frameBuffer );
  END_TEST;
}

int UtcDaliFrameBufferAttachColorTexture01(void)
{
  TestApplication application;

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::DEPTH_STENCIL );
  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height );
  frameBuffer.AttachColorTexture( texture );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferColorAttachmentCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferDepthAttachment(), (GLenum)GL_TRUE, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferStencilAttachment(), (GLenum)GL_TRUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameBufferAttachColorTexture02(void)
{
  TestApplication application;

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::NONE );
  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height );
  texture.GenerateMipmaps();

  //Attach mipmap 1
  frameBuffer.AttachColorTexture( texture, 0u, 1u );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferColorAttachmentCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferDepthAttachment(), (GLenum)GL_FALSE, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferStencilAttachment(), (GLenum)GL_FALSE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameBufferAttachColorTexture03(void)
{
  TestApplication application;

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::NONE );
  Texture texture = Texture::New( TextureType::TEXTURE_CUBE, Pixel::RGBA8888, width, height );
  texture.GenerateMipmaps();

  //Attach NEGATIVE_Y face of the cubemap
  frameBuffer.AttachColorTexture( texture, 0u, CubeMapLayer::NEGATIVE_Y );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferColorAttachmentCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferDepthAttachment(), (GLenum)GL_FALSE, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferStencilAttachment(), (GLenum)GL_FALSE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameBufferAttachColorTexture04(void)
{
  TestApplication application;

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, static_cast<FrameBuffer::Attachment::Mask>( FrameBuffer::Attachment::DEPTH | FrameBuffer::Attachment::STENCIL ) );
  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height );
  frameBuffer.AttachColorTexture( texture );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferColorAttachmentCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferDepthAttachment(), (GLenum)GL_TRUE, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferStencilAttachment(), (GLenum)GL_TRUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameBufferAttachColorTexture05(void)
{
  TestApplication application;

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::NONE );
  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height );

  // N.B. it doesn't make sense per se, however the OGL standard doesn't seem to forbid attaching the same texture to different slots.
  for (int i = 0; i < Dali::DevelFrameBuffer::MAX_COLOR_ATTACHMENTS + 1; ++i)
  {
    frameBuffer.AttachColorTexture( texture );
  }

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferColorAttachmentCount(), Dali::DevelFrameBuffer::MAX_COLOR_ATTACHMENTS, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferDepthAttachment(), (GLenum)GL_FALSE, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferStencilAttachment(), (GLenum)GL_FALSE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameBufferAttachDepthTexture01(void)
{
  TestApplication application;

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::DEPTH_STENCIL );
  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height );
  frameBuffer.AttachColorTexture( texture );

  Texture textureDepth = Texture::New( TextureType::TEXTURE_2D, Pixel::DEPTH_UNSIGNED_INT, width, height );
  DevelFrameBuffer::AttachDepthTexture( frameBuffer, textureDepth );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferColorAttachmentCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferDepthAttachment(), (GLenum)GL_TRUE, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferStencilAttachment(), (GLenum)GL_TRUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameBufferAttachDepthStencilTexture01(void)
{
  TestApplication application;

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::DEPTH_STENCIL );
  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height );
  frameBuffer.AttachColorTexture( texture );

  Texture textureStencil = Texture::New( TextureType::TEXTURE_2D, Pixel::DEPTH_STENCIL, width, height );
  DevelFrameBuffer::AttachDepthStencilTexture( frameBuffer, textureStencil );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferColorAttachmentCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferDepthAttachment(), (GLenum)GL_TRUE, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferStencilAttachment(), (GLenum)GL_TRUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameBufferGetColorTexture01(void)
{
  TestApplication application;

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::NONE );
  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height );
  frameBuffer.AttachColorTexture( texture );

  DALI_TEST_EQUALS(frameBuffer.GetColorTexture(), texture, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameBufferGetColorTexture02(void)
{
  TestApplication application;

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::NONE );
  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height );
  frameBuffer.AttachColorTexture( texture, 0u, 1u );

  DALI_TEST_EQUALS(frameBuffer.GetColorTexture(), texture, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameBufferGetColorTexture03(void)
{ // FrameBuffer::GetColorTexture() and GetColorTexture(0) are equivalent
  TestApplication application;

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::NONE );
  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height );
  frameBuffer.AttachColorTexture( texture, 0u, 1u );

  DALI_TEST_EQUALS(frameBuffer.GetColorTexture(), texture, TEST_LOCATION);
  DALI_TEST_EQUALS(DevelFrameBuffer::GetColorTexture(frameBuffer, 0), texture, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameBufferGetColorTexture04(void)
{
  TestApplication application;

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::NONE );
  Texture textures[] = {
      Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height ),
      Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height ),
      Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height ),
      Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height ),
      Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height ),
      Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height ),
      Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height ),
      Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height ),
  };

  for (auto& t: textures)
  {
    frameBuffer.AttachColorTexture( t, 0u, 1u );
  }

  for (unsigned int i = 0; i < std::extent<decltype(textures)>::value; ++i)
  {
    DALI_TEST_EQUALS(DevelFrameBuffer::GetColorTexture(frameBuffer, i), textures[i], TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliFrameBufferGetDepthTexture01(void)
{
  TestApplication application;

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::NONE );
  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height );
  frameBuffer.AttachColorTexture( texture );

  Texture textureDepth = Texture::New( TextureType::TEXTURE_2D, Pixel::DEPTH_FLOAT, width, height );
  DevelFrameBuffer::AttachDepthTexture( frameBuffer, textureDepth );

  DALI_TEST_EQUALS(DevelFrameBuffer::GetDepthTexture( frameBuffer ), textureDepth, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameBufferGetDepthStencilTexture01(void)
{
  TestApplication application;

  unsigned int width(64);
  unsigned int height(64);
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::NONE );
  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height );
  frameBuffer.AttachColorTexture( texture );

  Texture textureStencil = Texture::New( TextureType::TEXTURE_2D, Pixel::DEPTH_STENCIL, width, height );
  DevelFrameBuffer::AttachDepthStencilTexture( frameBuffer, textureStencil );

  DALI_TEST_EQUALS(DevelFrameBuffer::GetDepthStencilTexture( frameBuffer ), textureStencil, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFramebufferContextLoss(void)
{
  tet_infoline("UtcDaliFramebufferContextLoss\n");
  TestApplication application;

  //Create the texture
  unsigned int width(64);
  unsigned int height(64);
  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height );
  DALI_TEST_CHECK( texture );
  FrameBuffer frameBuffer = FrameBuffer::New( width, height, FrameBuffer::Attachment::NONE );
  DALI_TEST_CHECK( frameBuffer );
  frameBuffer.AttachColorTexture( texture, 0u, 1u );

  Geometry geometry = CreateQuadGeometry();
  Shader shader = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  application.SendNotification();
  application.Render(16);

  // Lose & regain context (in render 'thread')
  application.ResetContext();
  DALI_TEST_CHECK( frameBuffer );

  END_TEST;
}
