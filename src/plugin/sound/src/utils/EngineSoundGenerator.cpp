#include "EngineSoundGenerator.hpp"

namespace ES::Plugin::Sound::Utils {

static std::vector<float> GenerateCombustionSound(float base_frequency, float duration, ma_uint32 sample_rate)
{
    int num_samples = static_cast<int>(duration * sample_rate);
    std::vector<float> buffer(num_samples);

    float fundamental_freq = 30.0f + (base_frequency * 0.3f);

    static thread_local std::random_device rd;
    static thread_local std::mt19937 gen(rd());
    static thread_local std::uniform_real_distribution<float> noise_dist(-0.5f, 0.5f);

    for (int i = 0; i < num_samples; ++i)
    {
        float t = static_cast<float>(i) / sample_rate;

        float fundamental = sinf(2.0f * PI * fundamental_freq * t);
        float harmonic2 = 0.6f * sinf(2.0f * PI * fundamental_freq * 1.5f * t);
        float harmonic3 = 0.4f * sinf(2.0f * PI * fundamental_freq * 2.0f * t);
        float harmonic4 = 0.2f * sinf(2.0f * PI * fundamental_freq * 3.0f * t);

        float noise = noise_dist(gen) * 0.1f;
        float decay = expf(-t * 8.0f);
        float attack = (t < 0.01f) ? (t / 0.01f) : 1.0f;

        buffer[i] = (fundamental + harmonic2 + harmonic3 + harmonic4 + noise) * decay * attack;
    }

    return buffer;
}

static std::vector<float> GenerateSilence(float duration, ma_uint32 sample_rate)
{
    int num_samples = static_cast<int>(duration * sample_rate);
    return std::vector<float>(num_samples, 0.0f);
}

static std::vector<float> SliceAudio(const std::vector<float> &buffer, float duration, ma_uint32 sample_rate)
{
    int num_samples = static_cast<int>(duration * sample_rate);
    if (num_samples > buffer.size())
        num_samples = static_cast<int>(buffer.size());

    return std::vector<float>(buffer.begin(), buffer.begin() + num_samples);
}

static std::vector<float> ConcatenateAudio(const std::vector<std::vector<float>> &buffers)
{
    std::vector<float> result;
    for (const auto &buffer : buffers)
        result.insert(result.end(), buffer.begin(), buffer.end());

    return result;
}

static void CalculateEngineTimings(EngineData *data)
{
    data->strokes_per_sec = (data->rpm * 2.0f) / 60.0f;
    data->sec_between_fires = data->strokes / data->strokes_per_sec;
    data->fire_duration = data->sec_between_fires / data->strokes;
    data->cycle_duration = data->sec_between_fires;
}

static std::vector<float> GenerateCylinderSound(EngineData *data, float cylinder_timing, ma_uint32 sample_rate)
{
    float before_fire_duration = (cylinder_timing / 180.0f) / (data->strokes_per_sec / 2.0f);

    std::vector<float> before_fire_snd = SliceAudio(data->silence_sound, before_fire_duration, sample_rate);
    std::vector<float> fire_snd = SliceAudio(data->fire_sound, data->fire_duration, sample_rate);

    float after_fire_duration = data->sec_between_fires - data->fire_duration - before_fire_duration;
    std::vector<float> after_fire_snd = SliceAudio(data->silence_sound, after_fire_duration, sample_rate);

    std::vector<std::vector<float>> parts = {before_fire_snd, fire_snd, after_fire_snd};
    return ConcatenateAudio(parts);
}

void PrepareEngineBuffers(EngineData *data, ma_uint32 sample_rate)
{
    CalculateEngineTimings(data);

    float base_duration = std::max(0.08f, data->cycle_duration * 0.3f);
    float base_freq = 30.0f + (data->rpm * 0.006f);

    data->fire_sound = GenerateCombustionSound(base_freq, base_duration, sample_rate);
    data->silence_sound = GenerateSilence(base_duration, sample_rate);

    data->cylinder_buffers.clear();
    for (int i = 0; i < data->cylinders; ++i)
    {
        std::vector<float> cylinder_buffer = GenerateCylinderSound(data, data->cylinder_timings[i], sample_rate);
        data->cylinder_buffers.push_back(cylinder_buffer);
    }

    data->buffer_size = 0;
    for (const auto &buffer : data->cylinder_buffers)
        data->buffer_size = std::max(data->buffer_size, static_cast<int>(buffer.size()));

    data->buffers_ready = true;
}

float MixCylinders(EngineData *data, float time_seconds)
{
    if (!data->buffers_ready || data->buffer_size == 0)
        return 0.0f;

    float cycle_position = fmodf(time_seconds, data->cycle_duration) / data->cycle_duration;
    int cycle_sample = static_cast<int>(cycle_position * data->buffer_size);

    if (cycle_sample >= data->buffer_size)
        cycle_sample = data->buffer_size - 1;

    float mixed_sample = 0.0f;
    int active_cylinders = 0;

    for (int i = 0; i < data->cylinders; ++i)
    {
        if (cycle_sample < data->cylinder_buffers[i].size())
        {
            mixed_sample += data->cylinder_buffers[i][cycle_sample];
            active_cylinders++;
        }
    }

    if (active_cylinders > 0)
        mixed_sample /= active_cylinders;

    static float prev_sample = 0.0f;
    float filtered_sample = mixed_sample * 0.7f + prev_sample * 0.3f;
    prev_sample = filtered_sample;

    float compressed_sample = filtered_sample;
    if (fabsf(compressed_sample) > 0.7f)
        compressed_sample = (compressed_sample > 0) ? 0.7f : -0.7f;

    return compressed_sample * AMPLITUDE;
}

void EngineDataCallback(ma_device *pDevice, void *pOutput, ma_uint32 frameCount)
{
    ES::Engine::Core *core = static_cast<ES::Engine::Core *>(pDevice->pUserData);
    float *out = static_cast<float *>(pOutput);

    core.GetRegistry()
        .view<ES::Plugin::Sound::Component::EngineSound, ES::Plugin::Physics::Component::WheeledVehicle3D>()
        .each([&core, &out](auto entity, ES::Plugin::Sound::Component::EngineSound &engineSound,
                            ES::Plugin::Physics::Component::WheeledVehicle3D &vehicle) {
            auto controller =
                reinterpret_cast<JPH::WheeledVehicleController *>(vehicle.vehicleConstraint->GetController());

            engineSound.data.rpm = (controller) ? controller->GetEngine().GetCurrentRPM() : 900.0f;

            if (!engineSound.data.buffers_ready)
                PrepareEngineBuffers(&engineSound.data, pDevice->sampleRate);

            for (ma_uint32 i = 0; i < frameCount; ++i)
            {
                float current_time = engineSound.data.t;
                float engine_sample = MixCylinders(&engineSound.data, current_time);

                out[i * 2 + 0] = engine_sample;
                out[i * 2 + 1] = engine_sample;

                engineSound.data.t += 1.0f / pDevice->sampleRate;
            }
        });
}

} // namespace ES::Plugin::Sound::Utils
