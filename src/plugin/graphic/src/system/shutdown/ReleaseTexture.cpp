#include "system/shutdown/ReleaseTexture.hpp"
#include "resource/TextureContainer.hpp"

void Graphic::System::ReleaseTexture(Engine::Core &core)
{
    core.DeleteResource<Resource::TextureContainer>();
}
