#pragma once

#include "core/Core.hpp"

namespace Graphic::Resource
{
    class AGPUBuffer
    {
    public:
        virtual ~AGPUBuffer() = default;
        virtual void Create(Engine::Core &core) = 0;
        virtual void Destroy(Engine::Core &core) = 0;

        virtual bool IsCreated(Engine::Core &core) const = 0;
        virtual void Update(Engine::Core &core) = 0;

        const wgpu::Buffer &GetBuffer() const
        {
            return _buffer;
        }

    private:
        wgpu::Buffer _buffer;
        bool _isCreated = false;
    };
} // namespace Graphic::Resource

