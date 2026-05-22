#include "system/initialization/ReleaseAdapter.hpp"
#include "exception/AdapterCreationError.hpp"
#include "resource/Adapter.hpp"
#include "resource/GraphicSettings.hpp"

void Graphic::System::ReleaseAdapter(Engine::Core &core) { core.DeleteResource<Resource::Adapter>(); }
