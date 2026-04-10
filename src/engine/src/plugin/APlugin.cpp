#include "plugin/APlugin.hpp"

namespace Engine {
APlugin::APlugin(Core &core) : _core(core) {}

Core &APlugin::GetCore() { return _core; }
} // namespace Engine
