#include "PluginRenderingPipeline.hpp"
#include "RenderingPipeline.hpp"
#include "Shutdown.hpp"
#include "Startup.hpp"
#include "Update.hpp"

void ES::Plugin::RenderingPipeline::Plugin::Bind()
{
    this->RegisterScheduler<Init>();
    this->RegisterScheduler<Setup>();
    this->GetCore().SetSchedulerBefore<Setup, ES::Engine::Scheduler::Startup>();
    this->GetCore().SetSchedulerBefore<Init, Setup>();
    // Init, Setup, Startup

    this->RegisterScheduler<PreUpdate>();
    this->RegisterScheduler<RenderSetup>();
    this->RegisterScheduler<ToGPU>();
    this->RegisterScheduler<Draw>();
    this->GetCore().SetSchedulerAfter<PreUpdate, ES::Engine::Scheduler::Startup>();
    this->GetCore().SetSchedulerBefore<PreUpdate, ES::Engine::Scheduler::Update>();
    this->GetCore().SetSchedulerBefore<ES::Engine::Scheduler::Update, RenderSetup>();
    this->GetCore().SetSchedulerBefore<RenderSetup, ToGPU>();
    this->GetCore().SetSchedulerBefore<ToGPU, Draw>();
    this->GetCore().SetSchedulerBefore<Draw, ES::Engine::Scheduler::Shutdown>();
    // PreUpdate, Update, RenderSetup, ToGPU, Draw
}
