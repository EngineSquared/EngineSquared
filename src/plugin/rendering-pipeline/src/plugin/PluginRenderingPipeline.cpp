#include "scheduler/Shutdown.hpp"
#include "scheduler/Startup.hpp"
#include "scheduler/Update.hpp"

#include "RenderingPipeline.hpp"

void RenderingPipeline::Plugin::Bind()
{
    this->RegisterScheduler<Init>();
    this->RegisterScheduler<Setup>();
    this->GetCore().SetSchedulerBefore<Setup, Engine::Scheduler::Startup>();
    this->GetCore().SetSchedulerBefore<Init, Setup>();
    // Init, Setup, Startup

    this->RegisterScheduler<PreUpdate>();
    this->RegisterScheduler<Preparation>();
    this->RegisterScheduler<Extraction>();
    this->RegisterScheduler<PipelineCreation>();
    this->RegisterScheduler<Batching>();
    this->RegisterScheduler<CommandCreation>();
    this->RegisterScheduler<Submission>();
    this->RegisterScheduler<Presentation>();
    this->GetCore().SetSchedulerAfter<PreUpdate, Engine::Scheduler::Startup>();
    this->GetCore().SetSchedulerBefore<PreUpdate, Engine::Scheduler::Update>();
    this->GetCore().SetSchedulerBefore<Engine::Scheduler::Update, Preparation>();
    this->GetCore().SetSchedulerBefore<Preparation, Extraction>();
    this->GetCore().SetSchedulerBefore<Extraction, PipelineCreation>();
    this->GetCore().SetSchedulerBefore<PipelineCreation, Batching>();
    this->GetCore().SetSchedulerBefore<Batching, CommandCreation>();
    this->GetCore().SetSchedulerBefore<CommandCreation, Submission>();
    this->GetCore().SetSchedulerBefore<Submission, Presentation>();
    this->GetCore().SetSchedulerBefore<Presentation, Engine::Scheduler::Shutdown>();
    // Preparation, Extraction, PipelineCreation, Batching, CommandCreation, Submission, Presentation
}
