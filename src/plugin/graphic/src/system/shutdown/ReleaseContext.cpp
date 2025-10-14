#include "system/shutdown/ReleaseContext.hpp"
#include "resource/Context.hpp"

void Plugin::Graphic::System::ReleaseContext(Engine::Core &core) {
    auto &context = core.GetResource<Resource::Context>();
    context.Release();
}
