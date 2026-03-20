#pragma once

#include <entt/entt.hpp>

namespace DynamicLibrary::Resource {

using RuntimeView =
    entt::basic_runtime_view<entt::basic_sparse_set<Engine::Id>, std::allocator<entt::basic_sparse_set<Engine::Id> *>>;

}
