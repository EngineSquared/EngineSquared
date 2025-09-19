#include "scheduler/Shutdown.hpp"
#include "scheduler/Startup.hpp"
#include "scheduler/Update.hpp"

#include "RenderingPipeline.hpp"

void Plugin::RenderingPipeline::Plugin::Bind()
{
    this->RegisterScheduler<Init>();
    this->RegisterScheduler<Setup>();
    this->GetCore().SetSchedulerBefore<Setup, Engine::Scheduler::Startup>();
    this->GetCore().SetSchedulerBefore<Init, Setup>();
    // Init, Setup, Startup

    this->RegisterScheduler<PreUpdate>();
    this->RegisterScheduler<RenderSetup>();
    this->RegisterScheduler<ToGPU>();
    this->RegisterScheduler<Draw>();
    this->GetCore().SetSchedulerAfter<PreUpdate, Engine::Scheduler::Startup>();
    this->GetCore().SetSchedulerBefore<PreUpdate, Engine::Scheduler::Update>();
    this->GetCore().SetSchedulerBefore<Engine::Scheduler::Update, RenderSetup>();
    this->GetCore().SetSchedulerBefore<RenderSetup, ToGPU>();
    this->GetCore().SetSchedulerBefore<ToGPU, Draw>();
    this->GetCore().SetSchedulerBefore<Draw, Engine::Scheduler::Shutdown>();
    // PreUpdate, Update, RenderSetup, ToGPU, Draw
}
