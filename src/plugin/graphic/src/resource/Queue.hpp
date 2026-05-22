#pragma once

#include "utils/webgpu.hpp"

namespace Graphic::Resource {
class Queue {
  public:
    explicit Queue(wgpu::Queue wgpuQueue_) : wgpuQueue(wgpuQueue_) {}
    ~Queue()
    {
        if (wgpuQueue)
        {
            wgpuQueue.release();
            wgpuQueue = nullptr;
        }
    }

    Queue(Queue &other) = delete;
    Queue &operator=(Queue &other) = delete;

    // @note While using && ctor, you're destroying the previous Queue and will be invalid
    Queue(Queue &&other) noexcept : wgpuQueue(other.wgpuQueue) { other.wgpuQueue = nullptr; }

    // @note While using && assignement, you're destroying the previous Queue and will be invalid
    Queue &operator=(Queue &&other) noexcept
    {
        if (this != &other)
        {
            if (wgpuQueue)
            {
                wgpuQueue.release();
            }
            wgpuQueue = std::move(other.wgpuQueue);
            other.wgpuQueue = nullptr;
        }
        return *this;
    }

    wgpu::Queue &operator*(void) { return wgpuQueue; };
    wgpu::Queue *operator->(void) { return &wgpuQueue; };

    const wgpu::Queue &operator*(void) const { return wgpuQueue; };
    const wgpu::Queue *operator->(void) const { return &wgpuQueue; };

  private:
    // @note We will assume that Queue is not null all the time
    wgpu::Queue wgpuQueue = nullptr;
};
} // namespace Graphic::Resource
