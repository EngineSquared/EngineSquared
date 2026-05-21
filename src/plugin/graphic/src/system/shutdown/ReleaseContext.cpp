#include "system/shutdown/ReleaseContext.hpp"
#include "resource/Context.hpp"

void Graphic::System::ReleaseContext(Engine::Core &core) { core.DeleteResource<Resource::Context>(); }
