# Issue #022: Implement Physics Sensors (Detection Zones for AI)

**Milestone:** v0.7 - Advanced Physics  
**Priority:** 🟢 MEDIUM  
**Estimated Effort:** 3-4 days  
**Dependencies:** #009 (Raycast for LOS), #011 (Overlap Queries), #014 (Trigger Events)  
**Related Issues:** #018 (Character Controller)  
**Follow-up Issues:** #026 (Editor Integration for sensor visualization)

---

## 📋 Description

Implement physics sensors for AI detection and gameplay mechanics. Sensors are trigger volumes (sphere, box, cone, cylinder) that detect entities entering/leaving without physical collision. Useful for enemy vision, aggro radius, patrol zones, detection volumes, and gameplay triggers.

## 🎯 Goals

- ✅ Implement Sensor component
- ✅ Implement SphereDetector (aggro radius, proximity)
- ✅ Implement BoxDetector (area detection)
- ✅ Implement ConeDetector (field of view, vision cone)
- ✅ Implement CylinderDetector (vertical zones)
- ✅ Line-of-sight checks (raycast-based)
- ✅ Entity filtering (tags, layers)
- ✅ Event system (OnEntityEnter/Exit/Stay)
- ✅ Full test coverage
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Create/Modify

```
src/plugin/physics/src/
├── component/
│   ├── Sensor.hpp                  # New file
│   ├── SphereDetector.hpp          # New file
│   ├── BoxDetector.hpp             # New file
│   ├── ConeDetector.hpp            # New file
│   └── CylinderDetector.hpp        # New file
├── resource/
│   ├── SensorManager.hpp           # New file
│   └── SensorManager.cpp           # New file
├── system/
│   ├── SensorSystem.hpp            # New file
│   └── SensorSystem.cpp            # New file
└── tests/
    └── SensorTest.cpp              # New file
```

### Sensor Component

```cpp
/**************************************************************************
 * @file Sensor.hpp
 * @brief Sensor component (detection zones)
 **************************************************************************/

#pragma once

#include <functional>
#include <unordered_set>
#include <string>
#include <glm/glm.hpp>

namespace Engine {
    class Core;
    class Entity;
}

namespace Physics::Component {

/**
 * @brief Sensor type
 */
enum class SensorType {
    Sphere,     ///< Spherical detection (aggro radius)
    Box,        ///< Box detection (area)
    Cone,       ///< Cone detection (field of view)
    Cylinder    ///< Cylinder detection (vertical zone)
};

/**
 * @brief Sensor filter (what to detect)
 */
struct SensorFilter {
    std::unordered_set<std::string> includeTags;   ///< Detect entities with these tags
    std::unordered_set<std::string> excludeTags;   ///< Ignore entities with these tags
    int layerMask = ~0;                            ///< Layer mask (bit mask)
    bool requireLineOfSight = false;               ///< Require clear line-of-sight
    
    /**
     * @brief Check if entity passes filter
     * @param core Engine core
     * @param entity Entity to check
     * @param sensorPos Sensor position (for LOS check)
     * @return true if entity passes filter
     */
    bool PassesFilter(Engine::Core &core, Engine::Entity entity, const glm::vec3 &sensorPos) const;
    
    /**
     * @brief Create filter that detects everything
     * @return SensorFilter
     */
    static SensorFilter All() {
        return {};
    }
    
    /**
     * @brief Create filter that detects specific tag
     * @param tag Tag to detect (e.g., "Player", "Enemy")
     * @return SensorFilter
     */
    static SensorFilter ByTag(const std::string &tag) {
        SensorFilter f;
        f.includeTags.insert(tag);
        return f;
    }
    
    /**
     * @brief Create filter with line-of-sight requirement
     * @return SensorFilter
     */
    static SensorFilter WithLineOfSight() {
        SensorFilter f;
        f.requireLineOfSight = true;
        return f;
    }
};

/**
 * @brief Sensor component
 * 
 * Detects entities entering/leaving trigger volume.
 * Does NOT cause physical collision (like triggers).
 * 
 * **Common uses:**
 * - Enemy aggro radius (detect player)
 * - Enemy vision cone (field of view)
 * - Patrol zones (AI waypoints)
 * - Proximity triggers (doors, lights)
 * - Detection volumes (security cameras)
 * 
 * @example
 * @code
 * // Create enemy with aggro radius
 * auto enemy = core.CreateEntity();
 * 
 * // Add sphere detector (10m aggro radius)
 * Physics::Component::SphereDetector detector(10.0f);
 * detector.filter = Physics::Component::SensorFilter::ByTag("Player");
 * 
 * detector.onEntityEnter = [](Engine::Core &core, Engine::Entity sensor, Engine::Entity detected) {
 *     Logger::Info("Player entered aggro radius!");
 *     // Start chasing player
 * };
 * 
 * enemy.AddComponent<Physics::Component::SphereDetector>(core, detector);
 * @endcode
 */
struct Sensor {
    /// Type
    SensorType type = SensorType::Sphere;
    
    /// Enabled
    bool enabled = true;
    
    /// Filter
    SensorFilter filter;
    
    /// Detected entities (currently inside sensor)
    std::unordered_set<Engine::Entity> detectedEntities;
    
    /// Callbacks
    std::function<void(Engine::Core &, Engine::Entity sensor, Engine::Entity detected)> onEntityEnter;
    std::function<void(Engine::Core &, Engine::Entity sensor, Engine::Entity detected)> onEntityExit;
    std::function<void(Engine::Core &, Engine::Entity sensor, Engine::Entity detected)> onEntityStay;
    
    /**
     * @brief Check if entity is detected
     * @param entity Entity to check
     * @return true if entity is currently detected
     */
    bool IsDetected(Engine::Entity entity) const {
        return detectedEntities.find(entity) != detectedEntities.end();
    }
    
    /**
     * @brief Get detected entity count
     * @return Number of detected entities
     */
    size_t GetDetectedCount() const {
        return detectedEntities.size();
    }
    
    /**
     * @brief Clear detected entities
     */
    void Clear() {
        detectedEntities.clear();
    }
};

} // namespace Physics::Component
```

### SphereDetector Component

```cpp
/**************************************************************************
 * @file SphereDetector.hpp
 * @brief Sphere detector component (proximity detection)
 **************************************************************************/

#pragma once

#include "Sensor.hpp"

namespace Physics::Component {

/**
 * @brief Sphere detector component
 * 
 * Detects entities within spherical radius.
 * 
 * **Common uses:**
 * - Enemy aggro radius
 * - Proximity triggers
 * - Area effects (explosion damage)
 * 
 * @example
 * @code
 * // Detect player within 15m
 * Physics::Component::SphereDetector detector(15.0f);
 * detector.filter = Physics::Component::SensorFilter::ByTag("Player");
 * detector.onEntityEnter = [](auto &core, auto sensor, auto player) {
 *     Logger::Info("Player detected!");
 * };
 * enemy.AddComponent<Physics::Component::SphereDetector>(core, detector);
 * @endcode
 */
struct SphereDetector : public Sensor {
    float radius = 1.0f;                          ///< Detection radius (meters)
    
    /**
     * @brief Create sphere detector
     * @param detectionRadius Radius (meters)
     * @return SphereDetector
     */
    static SphereDetector Create(float detectionRadius) {
        SphereDetector d;
        d.type = SensorType::Sphere;
        d.radius = detectionRadius;
        return d;
    }
    
    /**
     * @brief Constructor
     * @param detectionRadius Radius (meters)
     */
    SphereDetector(float detectionRadius = 1.0f) {
        type = SensorType::Sphere;
        radius = detectionRadius;
    }
};

} // namespace Physics::Component
```

### BoxDetector Component

```cpp
/**************************************************************************
 * @file BoxDetector.hpp
 * @brief Box detector component (area detection)
 **************************************************************************/

#pragma once

#include "Sensor.hpp"
#include <glm/glm.hpp>

namespace Physics::Component {

/**
 * @brief Box detector component
 * 
 * Detects entities within box volume.
 * 
 * **Common uses:**
 * - Room detection (enter/exit rooms)
 * - Area triggers (checkpoints)
 * - Zone detection (safe zones, danger zones)
 * 
 * @example
 * @code
 * // Detect player in room (5m x 3m x 5m)
 * Physics::Component::BoxDetector detector(glm::vec3(5, 3, 5));
 * detector.filter = Physics::Component::SensorFilter::ByTag("Player");
 * detector.onEntityEnter = [](auto &core, auto sensor, auto player) {
 *     Logger::Info("Player entered room!");
 * };
 * room.AddComponent<Physics::Component::BoxDetector>(core, detector);
 * @endcode
 */
struct BoxDetector : public Sensor {
    glm::vec3 halfExtents = glm::vec3(1.0f);      ///< Box half-extents (meters)
    
    /**
     * @brief Create box detector
     * @param extents Box extents (full size, meters)
     * @return BoxDetector
     */
    static BoxDetector Create(const glm::vec3 &extents) {
        BoxDetector d;
        d.type = SensorType::Box;
        d.halfExtents = extents * 0.5f;
        return d;
    }
    
    /**
     * @brief Constructor
     * @param extents Box extents (full size, meters)
     */
    BoxDetector(const glm::vec3 &extents = glm::vec3(1.0f)) {
        type = SensorType::Box;
        halfExtents = extents * 0.5f;
    }
};

} // namespace Physics::Component
```

### ConeDetector Component

```cpp
/**************************************************************************
 * @file ConeDetector.hpp
 * @brief Cone detector component (field of view)
 **************************************************************************/

#pragma once

#include "Sensor.hpp"
#include <glm/glm.hpp>

namespace Physics::Component {

/**
 * @brief Cone detector component
 * 
 * Detects entities within cone-shaped field of view.
 * Useful for AI vision and spotlights.
 * 
 * **Common uses:**
 * - Enemy vision cone (stealth games)
 * - Spotlight detection
 * - Camera field of view
 * 
 * @example
 * @code
 * // Enemy vision (10m range, 60° FOV)
 * Physics::Component::ConeDetector detector(10.0f, 60.0f);
 * detector.filter = Physics::Component::SensorFilter::ByTag("Player");
 * detector.filter.requireLineOfSight = true;  // Can't see through walls
 * 
 * detector.onEntityEnter = [](auto &core, auto enemy, auto player) {
 *     Logger::Info("Player spotted!");
 *     // Alert AI
 * };
 * 
 * enemy.AddComponent<Physics::Component::ConeDetector>(core, detector);
 * @endcode
 */
struct ConeDetector : public Sensor {
    float range = 10.0f;                          ///< Cone range (meters)
    float angle = 45.0f;                          ///< Cone half-angle (degrees)
    glm::vec3 direction = glm::vec3(0, 0, 1);    ///< Cone direction (local space)
    
    /**
     * @brief Create cone detector
     * @param detectionRange Range (meters)
     * @param fovAngle Field of view angle (degrees, full cone = 2 * angle)
     * @return ConeDetector
     */
    static ConeDetector Create(float detectionRange, float fovAngle) {
        ConeDetector d;
        d.type = SensorType::Cone;
        d.range = detectionRange;
        d.angle = fovAngle * 0.5f;  // Half-angle
        return d;
    }
    
    /**
     * @brief Constructor
     * @param detectionRange Range (meters)
     * @param fovAngle Field of view angle (degrees)
     */
    ConeDetector(float detectionRange = 10.0f, float fovAngle = 90.0f) {
        type = SensorType::Cone;
        range = detectionRange;
        angle = fovAngle * 0.5f;  // Half-angle
    }
};

} // namespace Physics::Component
```

### CylinderDetector Component

```cpp
/**************************************************************************
 * @file CylinderDetector.hpp
 * @brief Cylinder detector component (vertical zones)
 **************************************************************************/

#pragma once

#include "Sensor.hpp"

namespace Physics::Component {

/**
 * @brief Cylinder detector component
 * 
 * Detects entities within cylindrical volume.
 * 
 * **Common uses:**
 * - Vertical zones (elevator shafts)
 * - Patrol areas (circular patrol path)
 * - Tower defense (tower range)
 * 
 * @example
 * @code
 * // Tower detection zone (10m radius, 20m height)
 * Physics::Component::CylinderDetector detector(10.0f, 20.0f);
 * detector.filter = Physics::Component::SensorFilter::ByTag("Enemy");
 * detector.onEntityEnter = [](auto &core, auto tower, auto enemy) {
 *     Logger::Info("Enemy in range!");
 *     // Start shooting
 * };
 * tower.AddComponent<Physics::Component::CylinderDetector>(core, detector);
 * @endcode
 */
struct CylinderDetector : public Sensor {
    float radius = 1.0f;                          ///< Cylinder radius (meters)
    float height = 2.0f;                          ///< Cylinder height (meters)
    
    /**
     * @brief Create cylinder detector
     * @param detectionRadius Radius (meters)
     * @param detectionHeight Height (meters)
     * @return CylinderDetector
     */
    static CylinderDetector Create(float detectionRadius, float detectionHeight) {
        CylinderDetector d;
        d.type = SensorType::Cylinder;
        d.radius = detectionRadius;
        d.height = detectionHeight;
        return d;
    }
    
    /**
     * @brief Constructor
     * @param detectionRadius Radius (meters)
     * @param detectionHeight Height (meters)
     */
    CylinderDetector(float detectionRadius = 1.0f, float detectionHeight = 2.0f) {
        type = SensorType::Cylinder;
        radius = detectionRadius;
        height = detectionHeight;
    }
};

} // namespace Physics::Component
```

### SensorManager Resource

```cpp
/**************************************************************************
 * @file SensorManager.hpp
 * @brief Sensor manager utilities
 **************************************************************************/

#pragma once

namespace Engine {
    class Core;
    class Entity;
}

namespace Physics::Resource {

/**
 * @brief Sensor manager
 * 
 * Utilities for sensor queries and management.
 */
class SensorManager {
public:
    /**
     * @brief Check if entity is in any sensor
     * @param core Engine core
     * @param entity Entity to check
     * @return true if entity is detected by any sensor
     */
    static bool IsDetected(Engine::Core &core, Engine::Entity entity);
    
    /**
     * @brief Get all sensors detecting entity
     * @param core Engine core
     * @param entity Entity to check
     * @return Vector of sensor entities
     */
    static std::vector<Engine::Entity> GetDetectingSensors(
        Engine::Core &core,
        Engine::Entity entity
    );
    
    /**
     * @brief Find closest sensor detecting entity
     * @param core Engine core
     * @param entity Entity to check
     * @return Closest sensor entity (or invalid if none)
     */
    static Engine::Entity GetClosestSensor(
        Engine::Core &core,
        Engine::Entity entity
    );
    
    /**
     * @brief Find entities in sensor radius (manual query)
     * @param core Engine core
     * @param position Center position
     * @param radius Radius
     * @param filter Sensor filter
     * @return Vector of detected entities
     */
    static std::vector<Engine::Entity> QuerySphere(
        Engine::Core &core,
        const glm::vec3 &position,
        float radius,
        const Component::SensorFilter &filter = Component::SensorFilter::All()
    );
    
    /**
     * @brief Check line-of-sight between two positions
     * @param core Engine core
     * @param from Start position
     * @param to End position
     * @return true if clear line-of-sight (no obstacles)
     */
    static bool HasLineOfSight(
        Engine::Core &core,
        const glm::vec3 &from,
        const glm::vec3 &to
    );
};

} // namespace Physics::Resource
```

## 📝 Implementation Tasks

### Phase 1: Create Components (3-4 hours)

- [ ] Create `component/Sensor.hpp`
- [ ] Implement SensorType enum
- [ ] Implement SensorFilter struct
- [ ] Implement Sensor component
- [ ] Create `component/SphereDetector.hpp`
- [ ] Create `component/BoxDetector.hpp`
- [ ] Create `component/ConeDetector.hpp`
- [ ] Create `component/CylinderDetector.hpp`
- [ ] Add Doxygen comments
- [ ] Add to `Physics.hpp`

### Phase 2: Implement SensorManager (6-8 hours)

- [ ] Create `resource/SensorManager.hpp`
- [ ] Implement IsDetected()
- [ ] Implement GetDetectingSensors()
- [ ] Implement GetClosestSensor()
- [ ] Implement QuerySphere()
- [ ] Implement HasLineOfSight() (raycast)

### Phase 3: Implement SensorSystem (10-12 hours)

- [ ] Create `system/SensorSystem.hpp`
- [ ] Implement Initialize() / Shutdown()
- [ ] Implement Update()
  - Query potential entities (broad phase)
  - Check each sensor against entities
  - Handle Enter events (new detections)
  - Handle Stay events (continuous detection)
  - Handle Exit events (lost detections)
  - Apply filters (tags, layers, LOS)
- [ ] Integrate with trigger system
- [ ] Optimize with spatial partitioning

### Phase 4: Unit Tests (8-10 hours)

- [ ] Create `tests/SensorTest.cpp`
- [ ] Test sphere detection
- [ ] Test box detection
- [ ] Test cone detection (FOV)
- [ ] Test cylinder detection
- [ ] Test entity filtering (tags, layers)
- [ ] Test line-of-sight
- [ ] Test Enter/Exit/Stay callbacks
- [ ] Test multiple sensors per entity

### Phase 5: Integration Examples (8-10 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add "Enemy Aggro" example (sphere detector)
- [ ] Add "Vision Cone" example (cone detector)
- [ ] Add "Room Detection" example (box detector)
- [ ] Add "Tower Defense" example (cylinder detector)
- [ ] Add "Stealth Game" example (LOS check)

### Phase 6: Documentation (4-5 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Physics Sensors" section
  - Explain sensor types
  - Explain filtering
  - Explain line-of-sight
- [ ] Add practical examples

## 🧪 Testing Checklist

### Unit Tests

- [ ] Sphere detects entities in radius
- [ ] Box detects entities in volume
- [ ] Cone detects entities in FOV
- [ ] Cylinder detects entities in zone
- [ ] Filters work (tags, layers)
- [ ] LOS check works
- [ ] Enter callback fires
- [ ] Exit callback fires
- [ ] Stay callback fires

### Integration Tests

- [ ] Enemy detects player (aggro)
- [ ] Vision cone works (stealth)
- [ ] Room detection works
- [ ] Tower defense range works
- [ ] LOS blocks through walls

### Performance Tests

- [ ] 100 sensors: < 5ms per frame
- [ ] 1000 entities: < 10ms detection

### Edge Cases

- [ ] Sensor disabled (no detection)
- [ ] Entity at exact boundary
- [ ] Multiple sensors overlap
- [ ] Sensor moves while detecting

## 📚 Code Examples

### Example 1: Enemy Aggro (Sphere Detector)

```cpp
// Create enemy with aggro radius
Engine::Entity CreateEnemy(Engine::Core &core, glm::vec3 position) {
    auto enemy = core.CreateEntity();
    
    // Transform
    enemy.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(position));
    
    // Aggro radius (15m)
    Physics::Component::SphereDetector detector(15.0f);
    detector.filter = Physics::Component::SensorFilter::ByTag("Player");
    
    detector.onEntityEnter = [](Engine::Core &core, Engine::Entity enemy, Engine::Entity player) {
        Logger::Info("Player entered aggro radius!");
        
        // Start chasing
        auto &ai = enemy.GetComponent<AI::Component::AIAgent>(core);
        ai.SetTarget(player);
        ai.SetState(AI::AIState::Chase);
        
        // Play alert sound
        Audio::PlaySound("sounds/enemy_alert.wav", 
            enemy.GetComponent<Object::Component::Transform>(core).position);
    };
    
    detector.onEntityExit = [](Engine::Core &core, Engine::Entity enemy, Engine::Entity player) {
        Logger::Info("Player left aggro radius");
        
        // Return to patrol
        auto &ai = enemy.GetComponent<AI::Component::AIAgent>(core);
        ai.ClearTarget();
        ai.SetState(AI::AIState::Patrol);
    };
    
    enemy.AddComponent<Physics::Component::SphereDetector>(core, detector);
    
    Logger::Info("Enemy created with {}m aggro radius", detector.radius);
    return enemy;
}
```

### Example 2: Vision Cone (Cone Detector + LOS)

```cpp
// Create guard with vision cone (stealth game)
Engine::Entity CreateGuard(Engine::Core &core, glm::vec3 position) {
    auto guard = core.CreateEntity();
    
    // Transform
    guard.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(position));
    
    // Vision cone (12m range, 90° FOV)
    Physics::Component::ConeDetector detector(12.0f, 90.0f);
    detector.filter = Physics::Component::SensorFilter::ByTag("Player");
    detector.filter.requireLineOfSight = true;  // Can't see through walls
    
    detector.onEntityEnter = [](Engine::Core &core, Engine::Entity guard, Engine::Entity player) {
        Logger::Info("Player spotted!");
        
        // Trigger alarm
        auto &ai = guard.GetComponent<AI::Component::AIAgent>(core);
        ai.SetAlertLevel(AI::AlertLevel::Combat);
        ai.SetTarget(player);
        
        // Visual feedback
        Particles::SpawnExclamation(core, guard);
        Audio::PlaySound("sounds/alert.wav");
    };
    
    detector.onEntityStay = [](Engine::Core &core, Engine::Entity guard, Engine::Entity player) {
        // Keep tracking player
        auto &ai = guard.GetComponent<AI::Component::AIAgent>(core);
        ai.UpdateTargetPosition(
            player.GetComponent<Object::Component::Transform>(core).position
        );
    };
    
    detector.onEntityExit = [](Engine::Core &core, Engine::Entity guard, Engine::Entity player) {
        Logger::Info("Player escaped vision");
        
        auto &ai = guard.GetComponent<AI::Component::AIAgent>(core);
        ai.SetAlertLevel(AI::AlertLevel::Investigate);
        // Investigate last known position
    };
    
    guard.AddComponent<Physics::Component::ConeDetector>(core, detector);
    
    Logger::Info("Guard created with vision cone ({}m, {}°)", detector.range, detector.angle * 2);
    return guard;
}
```

### Example 3: Room Detection (Box Detector)

```cpp
// Create room with entry detection
Engine::Entity CreateRoom(Engine::Core &core, glm::vec3 center, glm::vec3 size) {
    auto room = core.CreateEntity();
    
    // Transform
    room.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(center));
    
    // Room detector
    Physics::Component::BoxDetector detector(size);
    detector.filter = Physics::Component::SensorFilter::ByTag("Player");
    
    detector.onEntityEnter = [](Engine::Core &core, Engine::Entity room, Engine::Entity player) {
        Logger::Info("Player entered room");
        
        // Update minimap
        UI::Minimap::SetCurrentRoom(room);
        
        // Save checkpoint
        Game::SaveCheckpoint(player, room);
        
        // Trigger room events (music, lighting, etc.)
        Events::TriggerRoomEnter(core, room);
    };
    
    detector.onEntityExit = [](Engine::Core &core, Engine::Entity room, Engine::Entity player) {
        Logger::Info("Player left room");
        Events::TriggerRoomExit(core, room);
    };
    
    room.AddComponent<Physics::Component::BoxDetector>(core, detector);
    
    Logger::Info("Room created ({}x{}x{}m)", size.x, size.y, size.z);
    return room;
}
```

### Example 4: Tower Defense (Cylinder Detector)

```cpp
// Create tower with attack range
Engine::Entity CreateTower(Engine::Core &core, glm::vec3 position) {
    auto tower = core.CreateEntity();
    
    // Transform
    tower.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(position));
    
    // Attack range (8m radius, 10m height)
    Physics::Component::CylinderDetector detector(8.0f, 10.0f);
    detector.filter = Physics::Component::SensorFilter::ByTag("Enemy");
    
    // Track detected enemies
    std::vector<Engine::Entity> enemiesInRange;
    
    detector.onEntityEnter = [&](Engine::Core &core, Engine::Entity tower, Engine::Entity enemy) {
        Logger::Info("Enemy entered tower range");
        enemiesInRange.push_back(enemy);
    };
    
    detector.onEntityExit = [&](Engine::Core &core, Engine::Entity tower, Engine::Entity enemy) {
        Logger::Info("Enemy left tower range");
        auto it = std::find(enemiesInRange.begin(), enemiesInRange.end(), enemy);
        if (it != enemiesInRange.end()) {
            enemiesInRange.erase(it);
        }
    };
    
    tower.AddComponent<Physics::Component::CylinderDetector>(core, detector);
    
    // Tower shoot logic (separate system)
    // Finds closest enemy in enemiesInRange and shoots
    
    Logger::Info("Tower created with {}m range", detector.radius);
    return tower;
}
```

### Example 5: Manual Query (Find Nearby Enemies)

```cpp
// Find all enemies near player
void FindNearbyEnemies(Engine::Core &core, Engine::Entity player) {
    auto &transform = player.GetComponent<Object::Component::Transform>(core);
    
    // Query 20m radius
    auto filter = Physics::Component::SensorFilter::ByTag("Enemy");
    auto enemies = Physics::Resource::SensorManager::QuerySphere(
        core,
        transform.position,
        20.0f,
        filter
    );
    
    Logger::Info("Found {} enemies nearby", enemies.size());
    
    // Display on radar
    for (auto enemy : enemies) {
        auto &enemyTransform = enemy.GetComponent<Object::Component::Transform>(core);
        UI::Radar::AddBlip(enemyTransform.position);
    }
}
```

### Example 6: Line-of-Sight Check

```cpp
// Check if player can see enemy
bool CanSeeEnemy(Engine::Core &core, Engine::Entity player, Engine::Entity enemy) {
    auto &playerTransform = player.GetComponent<Object::Component::Transform>(core);
    auto &enemyTransform = enemy.GetComponent<Object::Component::Transform>(core);
    
    // Check distance first (optimization)
    float distance = glm::distance(playerTransform.position, enemyTransform.position);
    if (distance > 50.0f) {
        return false;  // Too far
    }
    
    // Check LOS (raycast)
    bool hasLOS = Physics::Resource::SensorManager::HasLineOfSight(
        core,
        playerTransform.position + glm::vec3(0, 1.5f, 0),  // Eye level
        enemyTransform.position + glm::vec3(0, 1.0f, 0)    // Torso
    );
    
    if (hasLOS) {
        Logger::Info("Player can see enemy");
        return true;
    } else {
        Logger::Info("Enemy behind cover");
        return false;
    }
}
```

## 🐛 Error Handling

### Cases to Handle

1. **Invalid radius/size**
   ```cpp
   if (radius <= 0.0f) {
       Logger::Error("SphereDetector: Radius must be positive");
       radius = 1.0f;
   }
   ```

2. **No sensor component**
   ```cpp
   if (!entity.HasComponent<Physics::Component::Sensor>(core)) {
       Logger::Error("Entity {} has no sensor component", entity.GetId());
       return;
   }
   ```

3. **Invalid filter (empty tags)**
   ```cpp
   if (filter.includeTags.empty() && filter.excludeTags.empty()) {
       Logger::Warning("Sensor filter is empty (detects everything)");
   }
   ```

4. **Entity deleted during callback**
   ```cpp
   // Check validity before using entity
   if (!entity.IsValid(core)) {
       Logger::Warning("Entity deleted during sensor callback");
       return;
   }
   ```

## 💡 Design Considerations

### Sensor vs Trigger

| Feature | Sensor | Trigger |
|---------|--------|---------|
| Purpose | Detection | Interaction |
| Collision | No | Yes (ghost) |
| Callbacks | Enter/Exit/Stay | TriggerEnter/Exit |
| Filtering | Advanced | Basic |
| LOS | Yes | No |
| Use case | AI detection | Gameplay triggers |

### Performance Optimization

```cpp
// Broad phase: Only check entities near sensor
// Use spatial partitioning (octree, grid)
// Update frequency: Sensors can update at lower frequency (e.g., 10 Hz instead of 60 Hz)
```

### Line-of-Sight Implementation

```cpp
// Raycast from sensor to entity
// Ignore trigger colliders
// Only block on static geometry
bool HasLineOfSight(from, to) {
    auto result = Raycast(from, to - from);
    return !result.hit || result.entity == target;
}
```

## 📊 Success Criteria

- [ ] Sensor component implemented
- [ ] SphereDetector implemented
- [ ] BoxDetector implemented
- [ ] ConeDetector implemented
- [ ] CylinderDetector implemented
- [ ] SensorFilter implemented
- [ ] SensorManager implemented
- [ ] Line-of-sight check working
- [ ] Enter/Exit/Stay callbacks working
- [ ] Entity filtering working
- [ ] SensorSystem implemented
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples demonstrate usage
- [ ] Performance acceptable (< 5ms for 100 sensors)
- [ ] Documentation complete
- [ ] Code review approved

## 🔗 References

- Unity Triggers: https://docs.unity3d.com/Manual/CollidersOverview.html
- Unreal Overlap Events: https://docs.unrealengine.com/en-US/API/Runtime/Engine/Components/UPrimitiveComponent/
- AI Detection: https://www.gamasutra.com/view/feature/131774/implementing_a_robust_ai_sensory_.php
- FOV Cone: https://antongerdelan.net/opengl/raycasting.html

## 🚀 Follow-up Issues

- #025: AI System Integration (use sensors for AI perception)
- #026: Debug Visualization (draw sensor volumes)
- #024: Performance Tuning (optimize broad phase)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.7`, `priority:medium`, `sensors`, `ai`, `detection`  
**Milestone:** v0.7 - Advanced Physics
