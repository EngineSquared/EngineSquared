#include "system/initialization/CreateDefaultTexture.hpp"
#include "resource/Context.hpp"
#include "resource/Sampler.hpp"
#include "resource/SamplerContainer.hpp"
#include "utils/DefaultSampler.hpp"

namespace Graphic::System {
void CreateDefaultSampler(Engine::Core &core)
{
    const auto &context = core.GetResource<Resource::Context>();
    Resource::SamplerContainer &samplerContainer = core.GetResource<Resource::SamplerContainer>();

    Resource::Sampler defaultSampler(context.deviceContext.GetDevice().value());

    samplerContainer.Add(Utils::DEFAULT_SAMPLER_ID, std::move(defaultSampler));
}
}
