#include "system/shutdown/ReleaseGPUBuffer.hpp"
#include "resource/GPUBufferContainer.hpp"

void Graphic::System::ReleaseGPUBuffer(Engine::Core &core)
{
    core.DeleteResource<Resource::GPUBufferContainer>();
}
