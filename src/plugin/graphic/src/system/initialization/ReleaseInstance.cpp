#include "system/initialization/ReleaseInstance.hpp"
#include "resource/Instance.hpp"

namespace Graphic::System {
void ReleaseInstance(Engine::Core &core) { core.DeleteResource<Resource::Instance>(); }
} // namespace Graphic::System
