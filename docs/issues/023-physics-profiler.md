# Issue #023: Implement Physics Profiler (Performance Metrics & Optimization)

**Milestone:** v0.8 - Ecosystem & Tools  
**Priority:** 🔴 HIGH  
**Estimated Effort:** 3-4 days  
**Dependencies:** None (standalone)  
**Related Issues:** #024 (Debug Visualization)  
**Follow-up Issues:** None

---

## 📋 Description

Implement comprehensive physics profiling system to monitor performance, detect bottlenecks, and optimize physics simulation. Provides real-time metrics, frame analysis, memory tracking, and performance recommendations.

## 🎯 Goals

- ✅ Implement PhysicsProfiler resource
- ✅ Real-time performance metrics (FPS, frame time, physics time)
- ✅ Per-system breakdown (collision, constraints, integration)
- ✅ Memory tracking (allocations, peak usage)
- ✅ Entity statistics (counts, types)
- ✅ Performance warnings and recommendations
- ✅ Export to CSV/JSON
- ✅ Integration with engine profiler
- ✅ Full test coverage
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Create/Modify

```
src/plugin/physics/src/
├── resource/
│   ├── PhysicsProfiler.hpp         # New file
│   └── PhysicsProfiler.cpp         # New file
├── system/
│   ├── PhysicsSystem.hpp           # Modify (add profiling)
│   └── PhysicsSystem.cpp           # Modify (add profiling)
└── tests/
    └── PhysicsProfilerTest.cpp     # New file
```

### PhysicsProfiler Resource

```cpp
/**************************************************************************
 * @file PhysicsProfiler.hpp
 * @brief Physics profiler (performance metrics)
 **************************************************************************/

#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>

namespace Engine {
    class Core;
}

namespace Physics::Resource {

/**
 * @brief Frame statistics
 */
struct FrameStats {
    /// Timing (milliseconds)
    float totalTime = 0.0f;           ///< Total physics time
    float collisionTime = 0.0f;       ///< Collision detection time
    float constraintTime = 0.0f;      ///< Constraint solving time
    float integrationTime = 0.0f;     ///< Integration time (velocity, position)
    float eventTime = 0.0f;           ///< Event processing time
    
    /// Entity counts
    int dynamicBodyCount = 0;         ///< Dynamic rigid bodies
    int staticBodyCount = 0;          ///< Static rigid bodies
    int kinematicBodyCount = 0;       ///< Kinematic rigid bodies
    int colliderCount = 0;            ///< Total colliders
    int constraintCount = 0;          ///< Total constraints
    int contactCount = 0;             ///< Active contacts
    
    /// Memory (bytes)
    size_t memoryUsage = 0;           ///< Current memory usage
    size_t peakMemory = 0;            ///< Peak memory usage
    
    /// FPS
    float fps = 0.0f;                 ///< Frames per second
    float deltaTime = 0.0f;           ///< Delta time (seconds)
};

/**
 * @brief Performance warning
 */
struct PerformanceWarning {
    enum class Type {
        HighFrameTime,      ///< Frame time exceeds budget
        TooManyBodies,      ///< Too many rigid bodies
        TooManyContacts,    ///< Too many active contacts
        MemoryLeak,         ///< Memory usage increasing
        UnbalancedLoad      ///< One system taking too long
    };
    
    Type type;
    std::string message;
    float severity;  // [0, 1]
    
    /**
     * @brief Get warning type name
     * @return Type name
     */
    const char* GetTypeName() const {
        switch (type) {
            case Type::HighFrameTime: return "HighFrameTime";
            case Type::TooManyBodies: return "TooManyBodies";
            case Type::TooManyContacts: return "TooManyContacts";
            case Type::MemoryLeak: return "MemoryLeak";
            case Type::UnbalancedLoad: return "UnbalancedLoad";
            default: return "Unknown";
        }
    }
};

/**
 * @brief Physics profiler
 * 
 * Monitors physics performance and provides optimization recommendations.
 * 
 * **Features:**
 * - Real-time metrics (frame time, FPS, entity counts)
 * - Per-system breakdown (collision, constraints, integration)
 * - Memory tracking (allocations, peak usage)
 * - Performance warnings (bottlenecks, issues)
 * - Export to CSV/JSON
 * 
 * @example
 * @code
 * // Enable profiler
 * Physics::Resource::PhysicsProfiler::Enable(core);
 * 
 * // Get current stats
 * auto stats = Physics::Resource::PhysicsProfiler::GetCurrentStats(core);
 * Logger::Info("Physics time: {:.2f}ms", stats.totalTime);
 * 
 * // Check for warnings
 * auto warnings = Physics::Resource::PhysicsProfiler::GetWarnings(core);
 * for (auto &warning : warnings) {
 *     Logger::Warning("{}: {}", warning.GetTypeName(), warning.message);
 * }
 * 
 * // Export to CSV
 * Physics::Resource::PhysicsProfiler::ExportToCSV(core, "physics_profile.csv");
 * @endcode
 */
class PhysicsProfiler {
public:
    /**
     * @brief Enable profiler
     * @param core Engine core
     */
    static void Enable(Engine::Core &core);
    
    /**
     * @brief Disable profiler
     * @param core Engine core
     */
    static void Disable(Engine::Core &core);
    
    /**
     * @brief Check if profiler is enabled
     * @param core Engine core
     * @return true if enabled
     */
    static bool IsEnabled(Engine::Core &core);
    
    /**
     * @brief Get current frame stats
     * @param core Engine core
     * @return FrameStats
     */
    static FrameStats GetCurrentStats(Engine::Core &core);
    
    /**
     * @brief Get average stats over N frames
     * @param core Engine core
     * @param frameCount Number of frames to average
     * @return FrameStats (averaged)
     */
    static FrameStats GetAverageStats(Engine::Core &core, int frameCount = 60);
    
    /**
     * @brief Get performance warnings
     * @param core Engine core
     * @return Vector of warnings
     */
    static std::vector<PerformanceWarning> GetWarnings(Engine::Core &core);
    
    /**
     * @brief Set performance budget (target frame time)
     * @param core Engine core
     * @param budget Target frame time (milliseconds)
     * 
     * If physics time exceeds budget, warning is generated.
     */
    static void SetBudget(Engine::Core &core, float budget);
    
    /**
     * @brief Get performance budget
     * @param core Engine core
     * @return Budget (milliseconds)
     */
    static float GetBudget(Engine::Core &core);
    
    /**
     * @brief Export stats to CSV
     * @param core Engine core
     * @param filename Output filename
     * @param frameCount Number of frames to export
     */
    static void ExportToCSV(Engine::Core &core, const std::string &filename, int frameCount = 300);
    
    /**
     * @brief Export stats to JSON
     * @param core Engine core
     * @param filename Output filename
     */
    static void ExportToJSON(Engine::Core &core, const std::string &filename);
    
    /**
     * @brief Reset profiler (clear history)
     * @param core Engine core
     */
    static void Reset(Engine::Core &core);
    
    /**
     * @brief Begin profiling section
     * @param core Engine core
     * @param name Section name (e.g., "Collision")
     * 
     * Use with EndSection() to measure custom sections.
     */
    static void BeginSection(Engine::Core &core, const std::string &name);
    
    /**
     * @brief End profiling section
     * @param core Engine core
     * @param name Section name
     */
    static void EndSection(Engine::Core &core, const std::string &name);
    
    /**
     * @brief Get section time
     * @param core Engine core
     * @param name Section name
     * @return Time (milliseconds)
     */
    static float GetSectionTime(Engine::Core &core, const std::string &name);
    
    /**
     * @brief Print stats to console
     * @param core Engine core
     */
    static void PrintStats(Engine::Core &core);
    
private:
    /// Internal state
    struct State {
        bool enabled = false;
        float budget = 16.67f;  // 60 FPS
        std::vector<FrameStats> history;
        std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> sectionStarts;
        std::unordered_map<std::string, float> sectionTimes;
    };
    
    static State& GetState(Engine::Core &core);
    static void UpdateWarnings(Engine::Core &core);
};

/**
 * @brief RAII profiler scope
 * 
 * Automatically begins/ends profiling section.
 * 
 * @example
 * @code
 * {
 *     Physics::Resource::ProfilerScope scope(core, "Collision");
 *     // ... collision detection code ...
 * }  // Automatically ends section
 * @endcode
 */
class ProfilerScope {
public:
    ProfilerScope(Engine::Core &core, const std::string &name)
        : m_core(core), m_name(name) {
        PhysicsProfiler::BeginSection(m_core, m_name);
    }
    
    ~ProfilerScope() {
        PhysicsProfiler::EndSection(m_core, m_name);
    }
    
private:
    Engine::Core &m_core;
    std::string m_name;
};

} // namespace Physics::Resource
```

## 📝 Implementation Tasks

### Phase 1: Create PhysicsProfiler (8-10 hours)

- [ ] Create `resource/PhysicsProfiler.hpp`
- [ ] Implement FrameStats struct
- [ ] Implement PerformanceWarning struct
- [ ] Implement Enable/Disable
- [ ] Implement GetCurrentStats
- [ ] Implement GetAverageStats
- [ ] Implement GetWarnings
- [ ] Implement SetBudget/GetBudget
- [ ] Implement BeginSection/EndSection
- [ ] Implement ProfilerScope RAII
- [ ] Add Doxygen comments

### Phase 2: Integrate with PhysicsSystem (6-8 hours)

- [ ] Modify `system/PhysicsSystem.cpp`
- [ ] Add profiling to Update()
  - Measure collision detection
  - Measure constraint solving
  - Measure integration
  - Measure event processing
- [ ] Collect entity statistics
- [ ] Track memory usage
- [ ] Update frame history

### Phase 3: Implement Export Functions (4-5 hours)

- [ ] Implement ExportToCSV()
  - CSV format: frame, totalTime, collisionTime, ...
- [ ] Implement ExportToJSON()
  - JSON format with metadata
- [ ] Implement PrintStats()
  - Console output with formatting

### Phase 4: Implement Warning System (3-4 hours)

- [ ] Implement UpdateWarnings()
- [ ] Check frame time vs budget
- [ ] Check entity counts (limits)
- [ ] Check memory growth
- [ ] Check system balance

### Phase 5: Unit Tests (6-8 hours)

- [ ] Create `tests/PhysicsProfilerTest.cpp`
- [ ] Test Enable/Disable
- [ ] Test stats collection
- [ ] Test averaging
- [ ] Test warnings
- [ ] Test export functions
- [ ] Test profiler scope

### Phase 6: Integration Examples (6-8 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add profiler UI overlay
- [ ] Add budget configuration
- [ ] Add warning display
- [ ] Add export example

### Phase 7: Documentation (3-4 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Performance Profiling" section
  - Explain metrics
  - Explain optimization tips
- [ ] Add practical examples

## 🧪 Testing Checklist

### Unit Tests

- [ ] Profiler enables/disables correctly
- [ ] Stats collected accurately
- [ ] Averaging works correctly
- [ ] Warnings trigger correctly
- [ ] Export functions work
- [ ] Profiler scope works

### Integration Tests

- [ ] Profiler tracks frame time
- [ ] Entity counts accurate
- [ ] Memory tracking works
- [ ] Warnings appear in UI
- [ ] Export files valid

### Performance Tests

- [ ] Profiler overhead: < 0.1ms per frame
- [ ] Memory overhead: < 100KB

### Edge Cases

- [ ] Profiler disabled (no overhead)
- [ ] Zero entities
- [ ] Very high entity count
- [ ] Memory leak detection

## 📚 Code Examples

### Example 1: Basic Profiling

```cpp
// Enable profiler
Physics::Resource::PhysicsProfiler::Enable(core);

// Set target frame time (60 FPS = 16.67ms)
Physics::Resource::PhysicsProfiler::SetBudget(core, 16.67f);

// Game loop
while (running) {
    core.Update();
    
    // Get current stats
    auto stats = Physics::Resource::PhysicsProfiler::GetCurrentStats(core);
    
    // Display stats (UI overlay)
    UI::Text::Draw(glm::vec2(10, 10), 
        fmt::format("Physics: {:.2f}ms", stats.totalTime));
    UI::Text::Draw(glm::vec2(10, 30), 
        fmt::format("  Collision: {:.2f}ms", stats.collisionTime));
    UI::Text::Draw(glm::vec2(10, 50), 
        fmt::format("  Constraints: {:.2f}ms", stats.constraintTime));
    UI::Text::Draw(glm::vec2(10, 70), 
        fmt::format("Bodies: {} dynamic, {} static", 
            stats.dynamicBodyCount, stats.staticBodyCount));
    
    // Check warnings
    auto warnings = Physics::Resource::PhysicsProfiler::GetWarnings(core);
    if (!warnings.empty()) {
        UI::Text::Draw(glm::vec2(10, 100), 
            fmt::format("⚠️ {} warnings", warnings.size()), 
            glm::vec4(1, 0, 0, 1));  // Red
    }
}
```

### Example 2: Average Stats Over Time

```cpp
// Get average over last 60 frames (1 second at 60 FPS)
auto avgStats = Physics::Resource::PhysicsProfiler::GetAverageStats(core, 60);

Logger::Info("Average physics time: {:.2f}ms", avgStats.totalTime);
Logger::Info("Average FPS: {:.1f}", avgStats.fps);
Logger::Info("Average contacts: {}", avgStats.contactCount);

// Performance report
if (avgStats.totalTime > 16.67f) {
    Logger::Warning("Physics time exceeds 60 FPS budget!");
    Logger::Info("  Collision: {:.2f}ms ({:.1f}%)", 
        avgStats.collisionTime, 
        (avgStats.collisionTime / avgStats.totalTime) * 100.0f);
    Logger::Info("  Constraints: {:.2f}ms ({:.1f}%)", 
        avgStats.constraintTime,
        (avgStats.constraintTime / avgStats.totalTime) * 100.0f);
}
```

### Example 3: Performance Warnings

```cpp
// Check for performance issues
auto warnings = Physics::Resource::PhysicsProfiler::GetWarnings(core);

for (auto &warning : warnings) {
    Logger::Warning("[{}] {}", warning.GetTypeName(), warning.message);
    
    // Take action based on warning type
    switch (warning.type) {
        case Physics::Resource::PerformanceWarning::Type::TooManyBodies:
            Logger::Info("💡 Recommendation: Use static meshes for non-moving objects");
            break;
            
        case Physics::Resource::PerformanceWarning::Type::TooManyContacts:
            Logger::Info("💡 Recommendation: Reduce collision complexity or use layers");
            break;
            
        case Physics::Resource::PerformanceWarning::Type::HighFrameTime:
            Logger::Info("💡 Recommendation: Lower physics update rate or optimize scene");
            break;
            
        case Physics::Resource::PerformanceWarning::Type::MemoryLeak:
            Logger::Info("💡 Recommendation: Check for entity leaks or memory allocations");
            break;
            
        case Physics::Resource::PerformanceWarning::Type::UnbalancedLoad:
            Logger::Info("💡 Recommendation: Profile individual systems for bottlenecks");
            break;
    }
}
```

### Example 4: Export to CSV (Analyze in Excel/Python)

```cpp
// Record 5 seconds of data (300 frames at 60 FPS)
Logger::Info("Recording physics profile...");
std::this_thread::sleep_for(std::chrono::seconds(5));

// Export to CSV
Physics::Resource::PhysicsProfiler::ExportToCSV(core, "physics_profile.csv", 300);
Logger::Info("Exported to physics_profile.csv");

// CSV format:
// frame,totalTime,collisionTime,constraintTime,integrationTime,dynamicBodies,contacts
// 0,12.34,5.67,3.21,2.45,150,320
// 1,13.21,6.01,3.45,2.50,150,325
// ...
```

### Example 5: Custom Profiling Sections

```cpp
// Profile custom code section
void MyCustomPhysicsCode(Engine::Core &core) {
    Physics::Resource::PhysicsProfiler::BeginSection(core, "CustomCode");
    
    // ... custom physics code ...
    
    Physics::Resource::PhysicsProfiler::EndSection(core, "CustomCode");
    
    // Get time
    float time = Physics::Resource::PhysicsProfiler::GetSectionTime(core, "CustomCode");
    Logger::Info("Custom code took {:.2f}ms", time);
}

// Or use RAII scope (recommended)
void MyCustomPhysicsCodeRAII(Engine::Core &core) {
    Physics::Resource::ProfilerScope scope(core, "CustomCode");
    
    // ... custom physics code ...
    
}  // Automatically ends section
```

### Example 6: Print Stats to Console

```cpp
// Print detailed stats
Physics::Resource::PhysicsProfiler::PrintStats(core);

// Output:
// ============================
// Physics Profiler Stats
// ============================
// Total Time:        12.34 ms
//   Collision:        5.67 ms (45.9%)
//   Constraints:      3.21 ms (26.0%)
//   Integration:      2.45 ms (19.9%)
//   Events:           1.01 ms (8.2%)
// 
// Entities:
//   Dynamic Bodies:   150
//   Static Bodies:    45
//   Kinematic Bodies: 12
//   Colliders:        207
//   Constraints:      89
//   Contacts:         320
// 
// Memory:
//   Current:          2.5 MB
//   Peak:             3.1 MB
// 
// FPS:               58.3
// ============================
```

### Example 7: Integration with ImGui

```cpp
// ImGui profiler window
void RenderPhysicsProfiler(Engine::Core &core) {
    if (!ImGui::Begin("Physics Profiler")) {
        ImGui::End();
        return;
    }
    
    // Enable/Disable
    bool enabled = Physics::Resource::PhysicsProfiler::IsEnabled(core);
    if (ImGui::Checkbox("Enable Profiler", &enabled)) {
        if (enabled) {
            Physics::Resource::PhysicsProfiler::Enable(core);
        } else {
            Physics::Resource::PhysicsProfiler::Disable(core);
        }
    }
    
    if (!enabled) {
        ImGui::End();
        return;
    }
    
    // Current stats
    auto stats = Physics::Resource::PhysicsProfiler::GetCurrentStats(core);
    
    ImGui::Text("Total Time: %.2f ms", stats.totalTime);
    ImGui::Indent();
    ImGui::Text("Collision: %.2f ms", stats.collisionTime);
    ImGui::Text("Constraints: %.2f ms", stats.constraintTime);
    ImGui::Text("Integration: %.2f ms", stats.integrationTime);
    ImGui::Unindent();
    
    ImGui::Separator();
    ImGui::Text("Entities");
    ImGui::Indent();
    ImGui::Text("Dynamic Bodies: %d", stats.dynamicBodyCount);
    ImGui::Text("Static Bodies: %d", stats.staticBodyCount);
    ImGui::Text("Contacts: %d", stats.contactCount);
    ImGui::Unindent();
    
    ImGui::Separator();
    
    // Warnings
    auto warnings = Physics::Resource::PhysicsProfiler::GetWarnings(core);
    if (!warnings.empty()) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "⚠️ %zu warnings", warnings.size());
        for (auto &warning : warnings) {
            ImGui::BulletText("%s: %s", warning.GetTypeName(), warning.message.c_str());
        }
    }
    
    ImGui::Separator();
    
    // Export buttons
    if (ImGui::Button("Export CSV")) {
        Physics::Resource::PhysicsProfiler::ExportToCSV(core, "physics_profile.csv");
        Logger::Info("Exported to physics_profile.csv");
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        Physics::Resource::PhysicsProfiler::Reset(core);
    }
    
    ImGui::End();
}
```

## 🐛 Error Handling

### Cases to Handle

1. **Profiler not enabled**
   ```cpp
   if (!PhysicsProfiler::IsEnabled(core)) {
       Logger::Warning("PhysicsProfiler: Not enabled");
       return FrameStats{};
   }
   ```

2. **Invalid section name**
   ```cpp
   if (name.empty()) {
       Logger::Error("PhysicsProfiler: Section name cannot be empty");
       return;
   }
   ```

3. **Export file error**
   ```cpp
   std::ofstream file(filename);
   if (!file.is_open()) {
       Logger::Error("PhysicsProfiler: Failed to open file '{}'", filename);
       return;
   }
   ```

4. **Missing EndSection**
   ```cpp
   // Automatically end section in destructor (RAII)
   // Or log warning if BeginSection without EndSection
   ```

## 💡 Design Considerations

### Performance Overhead

- Profiler should have minimal overhead (< 0.1ms per frame)
- Use high-resolution timer (std::chrono::high_resolution_clock)
- Avoid allocations in hot path
- Cache results

### CSV Format

```csv
frame,totalTime,collisionTime,constraintTime,integrationTime,dynamicBodies,staticBodies,contacts
0,12.34,5.67,3.21,2.45,150,45,320
1,13.21,6.01,3.45,2.50,150,45,325
```

### JSON Format

```json
{
  "metadata": {
    "engine": "EngineSquared",
    "version": "0.8",
    "date": "2025-11-05",
    "frameCount": 300
  },
  "budget": 16.67,
  "average": {
    "totalTime": 12.5,
    "collisionTime": 5.8,
    "constraintTime": 3.3,
    "integrationTime": 2.4,
    "dynamicBodies": 150,
    "contacts": 322
  },
  "warnings": [
    {
      "type": "HighFrameTime",
      "message": "Physics time exceeds budget (13.2ms > 16.67ms)",
      "severity": 0.8
    }
  ]
}
```

## 📊 Success Criteria

- [ ] PhysicsProfiler resource implemented
- [ ] Stats collection working
- [ ] Averaging working
- [ ] Warning system working
- [ ] Export to CSV working
- [ ] Export to JSON working
- [ ] Profiler scope RAII working
- [ ] Integration with PhysicsSystem complete
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples demonstrate usage
- [ ] Profiler overhead < 0.1ms per frame
- [ ] Documentation complete
- [ ] Code review approved

## 🔗 References

- Jolt Profiling: https://jrouwe.github.io/JoltPhysics/index.html#profiling
- Unity Profiler: https://docs.unity3d.com/Manual/Profiler.html
- Chrome Tracing Format: https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/preview

## 🚀 Follow-up Issues

- #024: Debug Visualization (integrate with profiler)
- #027: Performance Tuning (use profiler data)
- #028: Editor Integration (profiler window)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.8`, `priority:high`, `profiling`, `performance`, `tools`  
**Milestone:** v0.8 - Ecosystem & Tools
