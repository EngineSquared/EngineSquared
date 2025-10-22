#include "scheduler/Shutdown.hpp"
#include "scheduler/Startup.hpp"
#include "scheduler/Update.hpp"

#include "RenderingPipeline.hpp"

void ES::Plugin::RenderingPipeline::Plugin::Bind()
{
    this->RegisterScheduler<Init>();
    this->RegisterScheduler<Setup>();
    this->GetCore().SetSchedulerBefore<Setup, ES::Engine::Scheduler::Startup>();
    this->GetCore().SetSchedulerBefore<Init, Setup>();
    // Init, Setup, Startup

    this->RegisterScheduler<PreUpdate>();
    this->RegisterScheduler<Preparation>();
    this->RegisterScheduler<Extraction>();
    this->RegisterScheduler<PipelineCreation>();
    this->RegisterScheduler<Batching>();
    this->RegisterScheduler<PipelineExecution>();
    this->RegisterScheduler<Submission>();
    this->RegisterScheduler<Presentation>();
    this->GetCore().SetSchedulerAfter<PreUpdate, ES::Engine::Scheduler::Startup>();
    this->GetCore().SetSchedulerBefore<PreUpdate, ES::Engine::Scheduler::Update>();
    this->GetCore().SetSchedulerBefore<ES::Engine::Scheduler::Update, Preparation>();
    this->GetCore().SetSchedulerBefore<Preparation, Extraction>();
    this->GetCore().SetSchedulerBefore<Extraction, PipelineCreation>();
    this->GetCore().SetSchedulerBefore<PipelineCreation, Batching>();
    this->GetCore().SetSchedulerBefore<Batching, PipelineExecution>();
    this->GetCore().SetSchedulerBefore<PipelineExecution, Submission>();
    this->GetCore().SetSchedulerBefore<Submission, Presentation>();
    this->GetCore().SetSchedulerBefore<Presentation, ES::Engine::Scheduler::Shutdown>();
    // Preparation, Extraction, PipelineCreation, Batching, PipelineExecution, Submission, Presentation
}
