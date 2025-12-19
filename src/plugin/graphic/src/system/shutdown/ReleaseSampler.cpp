#include "system/shutdown/ReleaseSampler.hpp"
#include "resource/SamplerContainer.hpp"

void Graphic::System::ReleaseSampler(Engine::Core &core) { core.DeleteResource<Resource::SamplerContainer>(); }
