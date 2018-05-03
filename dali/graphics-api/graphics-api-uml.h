//
// Created by adam.b on 01/05/18.
//

#ifndef DALI_VULKAN_161117_GRAPHICS_API_UML_H
#define DALI_VULKAN_161117_GRAPHICS_API_UML_H
/*
@startuml

package Memory {
class DeviceMemory {

}
class Allocator {

}
}
package Resource {
  class Image {

  }
  class Buffer {

  }
  class Sampler {

  }
  class Shader {

  }
  class Framebuffer {

  }
  class Pipeline {
  }
}

package Command {

 class CommandQueue {
 }

 class Command {
 }
}

package Device {
 class Graphics {
   Represents Graphics device
 }
 class Controller {

 }

 class PipelineCache {
 }
}

note "Bla" as N1
note as N2
Texture should be represented by two types of
resources
end note

N1 .. Resource
N2 .. Image
N2 .. Sampler

@enduml

@startuml

participant "Application" as A
participant "Shader" as S
participant "UpdateManager" as U
participant "Graphics::API::Controller" as G
participant "ShaderFactory" as SF
participant "Shader" as SH

A->S : New()
activate S
S->U : CreateShader()
U->G : GetShaderFactory()
create SF
G->SF : new
SF-->G
G-->U : :shaderFactory
U->G : CreateShader( shaderFactory )
G->SF : Create()
create SH
SF->SH : new
SH-->SF
SF-->G
G-->U



 ||100||






@enduml

*/



#endif //DALI_VULKAN_161117_GRAPHICS_API_UML_H
