#include "system/initialization/ReleaseInstance.hpp"
#include "resource/Context.hpp"

namespace Plugin::Graphic::System {
void ReleaseInstance(Engine::Core &core)
{
    auto &context = core.GetResource<Resource::Context>();

    if (context.instance.has_value())
    {
        context.instance->release();
        context.instance.reset();
    }
}
} // namespace Plugin::Graphic::System
