#include "system/shutdown/ReleaseShader.hpp"
#include "resource/ShaderContainer.hpp"

void Graphic::System::ReleaseShader(Engine::Core &core) { core.DeleteResource<Resource::ShaderContainer>(); }
