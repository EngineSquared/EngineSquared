#pragma once

#include "core/Core.hpp"
#include "utils/webgpu.hpp"

namespace Graphic::Resource {
class AGPUBuffer {
  public:
    virtual ~AGPUBuffer() = default;
    virtual void Create(Engine::Core &core) = 0;
    virtual void Destroy(Engine::Core &core) = 0;

    virtual bool IsCreated(Engine::Core &core) const = 0;
    virtual void Update(Engine::Core &core) = 0;

    virtual const wgpu::Buffer &GetBuffer() const = 0;
};
} // namespace Graphic::Resource
