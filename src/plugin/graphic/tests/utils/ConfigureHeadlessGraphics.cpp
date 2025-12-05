#include "utils/ConfigureHeadlessGraphics.hpp"

#include "resource/GraphicSettings.hpp"

namespace Graphic::Tests::Utils {
    void ConfigureHeadlessGraphics(Engine::Core &core)
    {
        core.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    }
}
