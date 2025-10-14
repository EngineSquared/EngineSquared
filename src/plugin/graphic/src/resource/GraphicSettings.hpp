#pragma once

#include "resource/Limits.hpp"
#include <ranges>

namespace Plugin::Graphic::Resource {

enum class WindowSystem {
    None,
    GLFW
};

enum class PowerPreference {
    LowPower,
    HighPerformance
};

class GraphicSettings {
  private:
    using RequiredFeatureContainer = std::vector<WGPUFeatureName>;

  public:
    GraphicSettings() = default;
    ~GraphicSettings() = default;

    WindowSystem GetWindowSystem() const { return windowSystem; }
    GraphicSettings &SetWindowSystem(WindowSystem system)
    {
        windowSystem = system;
        return *this;
    }

    PowerPreference GetPowerPreference() const { return powerPreference; }
    GraphicSettings &SetPowerPreference(PowerPreference preference)
    {
        powerPreference = preference;
        return *this;
    }

    Limits &GetWantedLimits() { return wantedLimits; }

    const RequiredFeatureContainer &GetRequiredFeatures() const { return requiredFeatures; }

    GraphicSettings &AddRequiredFeature(wgpu::FeatureName feature)
    {
        requiredFeatures.push_back(feature);
        return *this;
    }

    GraphicSettings &RemoveRequiredFeature(wgpu::FeatureName feature)
    {
        if (auto it = _GetRequiredFeature(feature); it != requiredFeatures.end())
        {
            requiredFeatures.erase(it);
        }
        return *this;
    }
    bool HasRequiredFeature(wgpu::FeatureName feature) const
    {
        auto it = _GetRequiredFeature(feature);
        return it != requiredFeatures.end();
    }

  private:
    RequiredFeatureContainer::iterator _GetRequiredFeature(const wgpu::FeatureName &feature)
    {
        return std::ranges::find(requiredFeatures, static_cast<WGPUFeatureName>(feature));
    }

    RequiredFeatureContainer::const_iterator _GetRequiredFeature(const wgpu::FeatureName &feature) const
    {
        return _GetRequiredFeature(feature);
    }

    WindowSystem windowSystem = WindowSystem::GLFW;
    PowerPreference powerPreference = PowerPreference::HighPerformance;
    Limits wantedLimits = Limits(wgpu::Default);
    RequiredFeatureContainer requiredFeatures;
};
} // namespace Plugin::Graphic::Resource
