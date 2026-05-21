#include "system/shutdown/ReleaseContext.hpp"
#include "resource/DeviceContext.hpp"

void Graphic::System::ReleaseContext(Engine::Core &core) { core.DeleteResource<Resource::DeviceContext>(); }
