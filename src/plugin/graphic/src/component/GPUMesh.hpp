#pragma once

#include <entt/core/hashed_string.hpp>

namespace Graphic::Component {
struct GPUMesh {
    using Id = entt::hashed_string;
    Id pointBufferId{};
    Id indexBufferId{};
    // Id bindGroupId;
};
};
