#include "system/initialization/ReleaseAdapter.hpp"
#include "exception/AdapterCreationError.hpp"
#include "resource/Context.hpp"
#include "resource/GraphicSettings.hpp"

void Graphic::System::ReleaseAdapter(Engine::Core &core)
{
    auto &context = core.GetResource<Resource::Context>();
    context.adapter->release();
    context.adapter.reset();
}
