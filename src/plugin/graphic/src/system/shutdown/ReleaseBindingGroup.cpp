#include "system/shutdown/ReleaseBindingGroup.hpp"
#include "resource/BindGroupManager.hpp"

void Graphic::System::ReleaseBindingGroup(Engine::Core &core) {
    core.DeleteResource<Resource::BindGroupManager>();
}
