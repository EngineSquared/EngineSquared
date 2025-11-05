# Issue #025: Implement Physics Serialization (Save/Load System)

**Milestone:** v0.8 - Ecosystem & Tools  
**Priority:** 🟡 MEDIUM  
**Estimated Effort:** 4-5 days  
**Dependencies:** None (standalone)  
**Related Issues:** #026 (Editor Integration)  
**Follow-up Issues:** None

---

## 📋 Description

Implement comprehensive serialization system for physics objects. Save/load rigid bodies, colliders, constraints, and settings to JSON/Binary format. Essential for game save systems, level editors, and scene management.

## 🎯 Goals

- ✅ Implement PhysicsSerializer resource
- ✅ Serialize RigidBody components (type, mass, velocity, etc.)
- ✅ Serialize all Collider types (Box, Sphere, Capsule, Mesh, etc.)
- ✅ Serialize Constraints (all types)
- ✅ Serialize Sensors and SoftBodies
- ✅ Support JSON format (human-readable)
- ✅ Support Binary format (compact, fast)
- ✅ Handle entity references (constraints, attachments)
- ✅ Versioning support (backward compatibility)
- ✅ Full test coverage
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Create/Modify

```
src/plugin/physics/src/
├── resource/
│   ├── PhysicsSerializer.hpp       # New file
│   └── PhysicsSerializer.cpp       # New file
└── tests/
    └── PhysicsSerializerTest.cpp   # New file
```

### PhysicsSerializer Resource

```cpp
/**************************************************************************
 * @file PhysicsSerializer.hpp
 * @brief Physics serializer (save/load system)
 **************************************************************************/

#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace Engine {
    class Core;
    class Entity;
}

namespace Physics::Resource {

/**
 * @brief Serialization format
 */
enum class SerializationFormat {
    JSON,       ///< JSON format (human-readable)
    Binary      ///< Binary format (compact, fast)
};

/**
 * @brief Serialization options
 */
struct SerializationOptions {
    SerializationFormat format = SerializationFormat::JSON;
    bool prettyPrint = true;        ///< JSON pretty print
    bool includeMetadata = true;    ///< Include version, timestamp, etc.
    bool compressBinary = true;     ///< Compress binary data
    
    /**
     * @brief Create default options (JSON, pretty)
     * @return SerializationOptions
     */
    static SerializationOptions Default() {
        return {};
    }
    
    /**
     * @brief Create compact options (binary, compressed)
     * @return SerializationOptions
     */
    static SerializationOptions Compact() {
        SerializationOptions opts;
        opts.format = SerializationFormat::Binary;
        opts.prettyPrint = false;
        return opts;
    }
};

/**
 * @brief Physics serializer
 * 
 * Save/load physics objects to JSON or Binary format.
 * 
 * **Supported components:**
 * - RigidBody (all types, properties)
 * - Colliders (Box, Sphere, Capsule, Mesh, Compound, HeightField)
 * - Constraints (all types)
 * - Sensors (all types)
 * - SoftBody (cloth, rope)
 * 
 * **Features:**
 * - JSON format (human-readable, editable)
 * - Binary format (compact, fast)
 * - Entity references (constraints, attachments)
 * - Versioning (backward compatibility)
 * - Selective serialization (filter by tags, layers)
 * 
 * @example
 * @code
 * // Save physics scene
 * Physics::Resource::PhysicsSerializer::SaveScene(core, "scene.json");
 * 
 * // Load physics scene
 * Physics::Resource::PhysicsSerializer::LoadScene(core, "scene.json");
 * 
 * // Save single entity
 * auto data = Physics::Resource::PhysicsSerializer::SerializeEntity(core, entity);
 * std::ofstream file("entity.json");
 * file << data.dump(2);
 * @endcode
 */
class PhysicsSerializer {
public:
    /**
     * @brief Save entire physics scene
     * @param core Engine core
     * @param filename Output filename
     * @param options Serialization options
     * 
     * Saves all physics entities to file.
     */
    static void SaveScene(Engine::Core &core, const std::string &filename,
                          const SerializationOptions &options = SerializationOptions::Default());
    
    /**
     * @brief Load entire physics scene
     * @param core Engine core
     * @param filename Input filename
     * 
     * Loads all physics entities from file.
     * Creates new entities in scene.
     */
    static void LoadScene(Engine::Core &core, const std::string &filename);
    
    /**
     * @brief Save single entity
     * @param core Engine core
     * @param entity Entity to save
     * @param filename Output filename
     * @param options Serialization options
     */
    static void SaveEntity(Engine::Core &core, Engine::Entity entity,
                           const std::string &filename,
                           const SerializationOptions &options = SerializationOptions::Default());
    
    /**
     * @brief Load single entity
     * @param core Engine core
     * @param filename Input filename
     * @return Loaded entity
     */
    static Engine::Entity LoadEntity(Engine::Core &core, const std::string &filename);
    
    /**
     * @brief Serialize entity to JSON
     * @param core Engine core
     * @param entity Entity to serialize
     * @return JSON object
     */
    static nlohmann::json SerializeEntity(Engine::Core &core, Engine::Entity entity);
    
    /**
     * @brief Deserialize entity from JSON
     * @param core Engine core
     * @param json JSON object
     * @return Deserialized entity
     */
    static Engine::Entity DeserializeEntity(Engine::Core &core, const nlohmann::json &json);
    
    /**
     * @brief Serialize RigidBody component
     * @param core Engine core
     * @param entity Entity with RigidBody
     * @return JSON object
     */
    static nlohmann::json SerializeRigidBody(Engine::Core &core, Engine::Entity entity);
    
    /**
     * @brief Deserialize RigidBody component
     * @param core Engine core
     * @param entity Entity to add component to
     * @param json JSON object
     */
    static void DeserializeRigidBody(Engine::Core &core, Engine::Entity entity,
                                     const nlohmann::json &json);
    
    /**
     * @brief Serialize Collider component
     * @param core Engine core
     * @param entity Entity with Collider
     * @return JSON object
     */
    static nlohmann::json SerializeCollider(Engine::Core &core, Engine::Entity entity);
    
    /**
     * @brief Deserialize Collider component
     * @param core Engine core
     * @param entity Entity to add component to
     * @param json JSON object
     */
    static void DeserializeCollider(Engine::Core &core, Engine::Entity entity,
                                     const nlohmann::json &json);
    
    /**
     * @brief Serialize Constraint component
     * @param core Engine core
     * @param entity Entity with Constraint
     * @return JSON object
     */
    static nlohmann::json SerializeConstraint(Engine::Core &core, Engine::Entity entity);
    
    /**
     * @brief Deserialize Constraint component
     * @param core Engine core
     * @param entity Entity to add component to
     * @param json JSON object
     * @param entityMap Entity ID mapping (for references)
     */
    static void DeserializeConstraint(Engine::Core &core, Engine::Entity entity,
                                       const nlohmann::json &json,
                                       const std::unordered_map<uint32_t, Engine::Entity> &entityMap);
    
    /**
     * @brief Check file format version
     * @param filename Input filename
     * @return Version string (e.g., "0.8.0")
     */
    static std::string GetFileVersion(const std::string &filename);
    
    /**
     * @brief Check if file is compatible
     * @param filename Input filename
     * @return true if compatible with current version
     */
    static bool IsFileCompatible(const std::string &filename);
    
private:
    /// Current version
    static constexpr const char* VERSION = "0.8.0";
    
    /// Internal helpers
    static nlohmann::json CreateMetadata();
    static bool ValidateMetadata(const nlohmann::json &json);
    static std::unordered_map<uint32_t, Engine::Entity> BuildEntityMap(
        Engine::Core &core, const nlohmann::json &sceneJson
    );
};

} // namespace Physics::Resource
```

### JSON Format Example

```json
{
  "metadata": {
    "version": "0.8.0",
    "engine": "EngineSquared",
    "timestamp": "2025-11-05T12:34:56",
    "entityCount": 3
  },
  "entities": [
    {
      "id": 42,
      "name": "Player",
      "components": {
        "RigidBody": {
          "type": "Dynamic",
          "mass": 75.0,
          "linearVelocity": [0.0, 0.0, 0.0],
          "angularVelocity": [0.0, 0.0, 0.0],
          "gravityFactor": 1.0,
          "linearDamping": 0.05,
          "angularDamping": 0.05,
          "isSleeping": false,
          "allowSleeping": true
        },
        "Collider": {
          "type": "Capsule",
          "radius": 0.5,
          "height": 1.8,
          "offset": [0.0, 0.9, 0.0],
          "rotation": [0.0, 0.0, 0.0, 1.0],
          "material": {
            "friction": 0.5,
            "restitution": 0.0
          }
        }
      }
    },
    {
      "id": 100,
      "name": "Ground",
      "components": {
        "RigidBody": {
          "type": "Static"
        },
        "Collider": {
          "type": "Box",
          "halfExtents": [50.0, 0.5, 50.0],
          "material": {
            "friction": 0.8,
            "restitution": 0.0
          }
        }
      }
    },
    {
      "id": 150,
      "name": "Rope",
      "components": {
        "Constraint": {
          "type": "Distance",
          "entityA": 42,
          "entityB": 100,
          "distance": 5.0,
          "stiffness": 0.9
        }
      }
    }
  ]
}
```

## 📝 Implementation Tasks

### Phase 1: Create PhysicsSerializer (6-8 hours)

- [ ] Create `resource/PhysicsSerializer.hpp`
- [ ] Implement SerializationFormat enum
- [ ] Implement SerializationOptions struct
- [ ] Implement SaveScene/LoadScene
- [ ] Implement SaveEntity/LoadEntity
- [ ] Implement SerializeEntity/DeserializeEntity
- [ ] Implement metadata functions
- [ ] Add Doxygen comments

### Phase 2: Implement Component Serializers (10-12 hours)

- [ ] Implement SerializeRigidBody/DeserializeRigidBody
  - Type (Dynamic, Kinematic, Static)
  - Mass, inertia
  - Velocities (linear, angular)
  - Damping
  - Gravity factor
  - Sleeping state
- [ ] Implement SerializeCollider/DeserializeCollider
  - All collider types (Box, Sphere, Capsule, Mesh, etc.)
  - Offset, rotation
  - Material (friction, restitution)
- [ ] Implement SerializeConstraint/DeserializeConstraint
  - All constraint types
  - Entity references (A, B)
  - Constraint parameters
- [ ] Implement Sensor serialization
- [ ] Implement SoftBody serialization

### Phase 3: Handle Entity References (4-5 hours)

- [ ] Implement BuildEntityMap()
  - Map old IDs to new entities
- [ ] Resolve constraint references
- [ ] Resolve attachment references
- [ ] Handle missing entities (warnings)

### Phase 4: Versioning (3-4 hours)

- [ ] Implement GetFileVersion()
- [ ] Implement IsFileCompatible()
- [ ] Handle version migration
  - v0.2 → v0.8 (add new fields)
- [ ] Add deprecation warnings

### Phase 5: Binary Format (Optional) (4-5 hours)

- [ ] Implement binary serialization
- [ ] Use MessagePack or custom format
- [ ] Compression (zlib)

### Phase 6: Unit Tests (8-10 hours)

- [ ] Create `tests/PhysicsSerializerTest.cpp`
- [ ] Test save/load scene
- [ ] Test save/load entity
- [ ] Test RigidBody serialization
- [ ] Test Collider serialization
- [ ] Test Constraint serialization
- [ ] Test entity references
- [ ] Test versioning
- [ ] Test round-trip (save → load → save)

### Phase 7: Integration Examples (6-8 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add save/load example
- [ ] Add scene import/export
- [ ] Add entity duplication (serialize + deserialize)

### Phase 8: Documentation (4-5 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Serialization" section
  - Explain JSON format
  - Explain versioning
- [ ] Add practical examples

## 🧪 Testing Checklist

### Unit Tests

- [ ] Scene saves correctly
- [ ] Scene loads correctly
- [ ] Entity saves correctly
- [ ] Entity loads correctly
- [ ] RigidBody serializes correctly
- [ ] Colliders serialize correctly
- [ ] Constraints serialize correctly
- [ ] Entity references resolve correctly
- [ ] Versioning works
- [ ] Round-trip preserves data

### Integration Tests

- [ ] Save/load full scene
- [ ] Save/load partial scene
- [ ] Load v0.2 file in v0.8
- [ ] Handle corrupted file

### Performance Tests

- [ ] Save 1000 entities: < 1s
- [ ] Load 1000 entities: < 1s

### Edge Cases

- [ ] Empty scene
- [ ] Missing entity reference
- [ ] Invalid JSON
- [ ] Incompatible version

## 📚 Code Examples

### Example 1: Save/Load Scene

```cpp
// Create physics scene
auto ground = CreateGround(core, glm::vec3(0, 0, 0), glm::vec3(50, 0.5f, 50));
auto player = CreatePlayer(core, glm::vec3(0, 5, 0));
auto crate = CreateCrate(core, glm::vec3(5, 5, 5), 50.0f);

// Save scene
Physics::Resource::PhysicsSerializer::SaveScene(core, "scene.json");
Logger::Info("Scene saved");

// Later...

// Load scene
Physics::Resource::PhysicsSerializer::LoadScene(core, "scene.json");
Logger::Info("Scene loaded");
```

### Example 2: Save/Load Single Entity

```cpp
// Save player entity
Physics::Resource::PhysicsSerializer::SaveEntity(core, player, "player.json");

// Load player entity (create duplicate)
auto playerClone = Physics::Resource::PhysicsSerializer::LoadEntity(core, "player.json");
Logger::Info("Player duplicated");
```

### Example 3: Serialize to String (for Network)

```cpp
// Serialize entity to JSON string
auto json = Physics::Resource::PhysicsSerializer::SerializeEntity(core, entity);
std::string jsonString = json.dump();

// Send over network
Network::Send(jsonString);

// On receiving end
auto receivedJson = nlohmann::json::parse(jsonString);
auto entity = Physics::Resource::PhysicsSerializer::DeserializeEntity(core, receivedJson);
Logger::Info("Entity received");
```

### Example 4: Save with Compact Binary Format

```cpp
// Save with binary format (smaller file, faster)
auto options = Physics::Resource::SerializationOptions::Compact();
Physics::Resource::PhysicsSerializer::SaveScene(core, "scene.bin", options);

// File size comparison:
// scene.json: 125 KB
// scene.bin:   45 KB (compressed)
```

### Example 5: Version Compatibility Check

```cpp
// Check if file is compatible before loading
std::string filename = "old_scene.json";

if (!Physics::Resource::PhysicsSerializer::IsFileCompatible(filename)) {
    auto version = Physics::Resource::PhysicsSerializer::GetFileVersion(filename);
    Logger::Warning("File version {} may be incompatible (current: 0.8.0)", version);
    
    // Ask user for confirmation
    if (!UI::Dialog::Confirm("Load potentially incompatible scene?")) {
        return;
    }
}

// Load scene
Physics::Resource::PhysicsSerializer::LoadScene(core, filename);
```

### Example 6: Selective Serialization (Filter by Tag)

```cpp
// Save only entities with "Saveable" tag
void SaveGameState(Engine::Core &core) {
    nlohmann::json sceneJson;
    sceneJson["metadata"] = /* ... */;
    sceneJson["entities"] = nlohmann::json::array();
    
    // Iterate all entities
    core.GetRegistry().each([&](auto entityID) {
        Engine::Entity entity(entityID, &core);
        
        // Check if entity has "Saveable" tag
        if (entity.HasComponent<Tag>(core)) {
            auto &tag = entity.GetComponent<Tag>(core);
            if (tag.name == "Saveable") {
                // Serialize this entity
                auto entityJson = Physics::Resource::PhysicsSerializer::SerializeEntity(core, entity);
                sceneJson["entities"].push_back(entityJson);
            }
        }
    });
    
    // Write to file
    std::ofstream file("savegame.json");
    file << sceneJson.dump(2);
    Logger::Info("Game saved");
}
```

### Example 7: Entity Duplication (Clone with Physics)

```cpp
// Duplicate entity (including physics components)
Engine::Entity CloneEntity(Engine::Core &core, Engine::Entity original) {
    // Serialize original
    auto json = Physics::Resource::PhysicsSerializer::SerializeEntity(core, original);
    
    // Deserialize into new entity
    auto clone = Physics::Resource::PhysicsSerializer::DeserializeEntity(core, json);
    
    // Offset position slightly
    if (clone.HasComponent<Object::Component::Transform>(core)) {
        auto &transform = clone.GetComponent<Object::Component::Transform>(core);
        transform.position += glm::vec3(2, 0, 0);
    }
    
    Logger::Info("Entity cloned");
    return clone;
}
```

## 🐛 Error Handling

### Cases to Handle

1. **File not found**
   ```cpp
   if (!std::filesystem::exists(filename)) {
       Logger::Error("PhysicsSerializer: File not found '{}'", filename);
       throw std::runtime_error("File not found");
   }
   ```

2. **Invalid JSON**
   ```cpp
   try {
       auto json = nlohmann::json::parse(file);
   } catch (const nlohmann::json::exception &e) {
       Logger::Error("PhysicsSerializer: Invalid JSON: {}", e.what());
       throw;
   }
   ```

3. **Missing entity reference**
   ```cpp
   if (entityMap.find(entityID) == entityMap.end()) {
       Logger::Warning("PhysicsSerializer: Entity {} not found, skipping constraint", entityID);
       return;  // Skip this constraint
   }
   ```

4. **Incompatible version**
   ```cpp
   if (fileVersion != VERSION) {
       Logger::Warning("PhysicsSerializer: File version {} (current: {})", fileVersion, VERSION);
       // Attempt migration
   }
   ```

## 💡 Design Considerations

### JSON Schema

```cpp
// Use nlohmann/json library
// Clear structure: metadata + entities
// Each entity: id + name + components
// Each component: type + properties
```

### Entity References

```cpp
// Problem: Entity IDs change between sessions
// Solution: Use temporary IDs in file, remap on load
// 1. Save: entity.GetId() → JSON
// 2. Load: Create new entities, build ID map
// 3. Resolve references using map
```

### Versioning Strategy

```cpp
// Backward compatibility
// v0.2 file → v0.8 engine: Add default values for new fields
// v0.8 file → v0.2 engine: Ignore unknown fields (warning)
```

## 📊 Success Criteria

- [ ] PhysicsSerializer resource implemented
- [ ] Save/load scene works
- [ ] Save/load entity works
- [ ] RigidBody serialization works
- [ ] All colliders serialize correctly
- [ ] Constraints serialize correctly
- [ ] Entity references resolve correctly
- [ ] Versioning works
- [ ] Binary format works (optional)
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples demonstrate usage
- [ ] Performance acceptable (< 1s for 1000 entities)
- [ ] Documentation complete
- [ ] Code review approved

## 🔗 References

- nlohmann/json: https://github.com/nlohmann/json
- JSON Schema: https://json-schema.org/
- MessagePack: https://msgpack.org/ (binary format)
- Unity Serialization: https://docs.unity3d.com/Manual/script-Serialization.html

## 🚀 Follow-up Issues

- #026: Editor Integration (use serializer for save/load in editor)
- #027: Scene System (integrate with scene management)
- #029: Asset Pipeline (import/export physics assets)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.8`, `priority:medium`, `serialization`, `save-load`, `tools`  
**Milestone:** v0.8 - Ecosystem & Tools
