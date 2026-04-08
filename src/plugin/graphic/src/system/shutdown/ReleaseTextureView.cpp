#include "system/shutdown/ReleaseTextureView.hpp"
#include "resource/TextureViewContainer.hpp"

void Graphic::System::ReleaseTextureView(Engine::Core &core) { core.DeleteResource<Resource::TextureViewContainer>(); }
