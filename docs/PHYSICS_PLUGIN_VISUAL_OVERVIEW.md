# 🎨 Physics Plugin - Vue d'Ensemble Visuelle

**Quick Reference - Structure et Organisation**

---

## 🗂️ Organisation des Répertoires

```
src/plugin/physics/
│
├── 📦 PRODUCTION CODE (110 fichiers)
│   ├── component/     (19 fichiers) ──→ Composants ECS
│   ├── resource/      (44 fichiers) ──→ Managers & Resources
│   ├── system/        (17 fichiers) ──→ Systèmes ECS
│   ├── event/         (4 fichiers)  ──→ Types d'événements
│   ├── query/         (4 fichiers)  ──→ Résultats de requêtes
│   ├── debug/         (5 fichiers)  ──→ Debug & Profiling
│   ├── serialization/ (6 fichiers)  ──→ Save/Load
│   ├── editor/        (7 fichiers)  ──→ Intégration éditeur
│   ├── utils/         (6 fichiers)  ──→ Utilitaires
│   ├── exception/     (4 fichiers)  ──→ Gestion erreurs
│   └── plugin/        (2 fichiers)  ──→ Interface plugin
│
└── 🧪 TESTS (27 fichiers)
    └── tests/         ──→ Tests unitaires & intégration
```

---

## 🏗️ Pyramide de Dépendances

```
                          ┌─────────────────┐
                          │   v0.8 TOOLS    │
                          │  (22 fichiers)  │
                          │ Debug, Editor   │
                          └────────┬────────┘
                                   │
                    ┌──────────────┴──────────────┐
                    │    v0.7 ADVANCED FEATURES   │
                    │        (27 fichiers)        │
                    │  Character, Vehicle, etc.   │
                    └──────────────┬──────────────┘
                                   │
                          ┌────────┴────────┐
                          │  v0.6 EVENTS &  │
                          │   CONSTRAINTS   │
                          │  (21 fichiers)  │
                          └────────┬────────┘
                                   │
                    ┌──────────────┴──────────────┐
                    │                             │
           ┌────────┴────────┐         ┌─────────┴────────┐
           │  v0.4 COLLIDERS │         │   v0.5 QUERIES   │
           │   (9 fichiers)  │         │  (13 fichiers)   │
           │  Mesh, Terrain  │         │ Raycast, Overlap │
           └────────┬────────┘         └─────────┬────────┘
                    │                             │
                    └──────────────┬──────────────┘
                                   │
                          ┌────────┴────────┐
                          │ v0.3 FOUNDATION │
                          │  (18 fichiers)  │
                          │  RigidBody Core │
                          └─────────────────┘
```

---

## 🎯 Points d'Entrée Principaux

```
┌─────────────────────────────────────────────────────────────────┐
│                        Physics.hpp                              │
│                  (Header Public Principal)                      │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Components  │  │  Resources  │  │   Systems   │            │
│  ├─────────────┤  ├─────────────┤  ├─────────────┤            │
│  │ RigidBody   │  │PhysicsMan...│  │PhysicsUpd...│            │
│  │ Collider    │  │ForceApplic..│  │RigidBodySy..│            │
│  │ Constraint  │  │VelocityCon..│  │CollisionEv..│            │
│  │ Character   │  │RaycastSyst..│  │TriggerEven..│            │
│  │ Vehicle     │  │...          │  │...          │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
│                                                                 │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │   Events    │  │   Queries   │  │    Debug    │            │
│  ├─────────────┤  ├─────────────┤  ├─────────────┤            │
│  │ Collision   │  │ RaycastHit  │  │DebugDraw    │            │
│  │ Trigger     │  │ OverlapRes..│  │ Profiler    │            │
│  │ Contact     │  │ ShapeCastH..│  │ ...         │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 🔄 Flow d'Exécution Typique

```
┌──────────────────────────────────────────────────────────────────┐
│                    APPLICATION START                             │
└────────────────────────┬─────────────────────────────────────────┘
                         │
                         ↓
┌────────────────────────────────────────────────────────────────┐
│              PhysicsPlugin::Initialize()                       │
│  • Initialise Jolt Physics                                     │
│  • Crée PhysicsWorld                                           │
│  • Configure PhysicsManager                                    │
│  • Enregistre les systèmes                                     │
└────────────────────────┬───────────────────────────────────────┘
                         │
                         ↓
┌────────────────────────────────────────────────────────────────┐
│                    GAME LOOP (Each Frame)                      │
└────────────────────────┬───────────────────────────────────────┘
                         │
                         ↓
          ┌──────────────┴──────────────┐
          │                             │
          ↓                             ↓
┌─────────────────────┐       ┌─────────────────────┐
│  User Input Phase   │       │   Physics Update    │
│  • AddForce()       │       │   PhysicsUpdateSys. │
│  • SetVelocity()    │       │   • Simulate()      │
│  • MoveKinematic()  │       │   • Step Physics    │
│  • Raycast()        │       │   • Detect Collisions│
└─────────┬───────────┘       └─────────┬───────────┘
          │                             │
          └──────────────┬──────────────┘
                         │
                         ↓
          ┌──────────────────────────────┐
          │   Collision Detection Done   │
          │   • Trigger Events           │
          │   • Contact Callbacks        │
          │   • Sensor Detection         │
          └──────────────┬───────────────┘
                         │
                         ↓
          ┌──────────────────────────────┐
          │    Event Processing Phase    │
          │  CollisionEventSystem        │
          │  TriggerEventSystem          │
          └──────────────┬───────────────┘
                         │
                         ↓
          ┌──────────────────────────────┐
          │    Synchronization Phase     │
          │  RigidBodySyncSystem         │
          │  • Body → Transform          │
          │  • Update Positions          │
          └──────────────┬───────────────┘
                         │
                         ↓
          ┌──────────────────────────────┐
          │     Debug Rendering          │
          │  PhysicsDebugDrawSystem      │
          │  • Draw Colliders            │
          │  • Draw Constraints          │
          │  • Show Velocities           │
          └──────────────────────────────┘
```

---

## 📊 Distribution des Fonctionnalités

```
┌─────────────────────────────────────────────────────────────────┐
│                    PHYSICS PLUGIN FEATURES                      │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━┓                                │
│  ┃   CORE (v0.3)            ┃  ← Fondation indispensable      │
│  ┃  • RigidBody             ┃                                  │
│  ┃  • Forces & Impulses     ┃                                  │
│  ┃  • Velocity Control      ┃                                  │
│  ┃  • Kinematic Movement    ┃                                  │
│  ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━┛                                │
│           ↓                                                     │
│  ┌───────────────────────────┐  ┌───────────────────────────┐ │
│  │  COLLIDERS (v0.4)         │  │   QUERIES (v0.5)          │ │
│  │  • Sphere, Capsule, Box   │  │   • Raycast               │ │
│  │  • Mesh, Compound         │  │   • ShapeCast             │ │
│  │  • HeightField (Terrain)  │  │   • Overlap               │ │
│  └───────────────────────────┘  └───────────────────────────┘ │
│           ↓                             ↓                      │
│  ┌────────────────────────────────────────────────────────┐   │
│  │           EVENTS & CONSTRAINTS (v0.6)                   │   │
│  │  • Collision Events       • Fixed Constraint            │   │
│  │  • Trigger Events         • Distance Constraint         │   │
│  │  • Contact Callbacks      • Hinge, Slider, 6DOF         │   │
│  └────────────────────────────────────────────────────────┘   │
│           ↓                                                    │
│  ┌────────────────────────────────────────────────────────┐   │
│  │              ADVANCED FEATURES (v0.7)                   │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐    │   │
│  │  │ Character   │  │  Vehicle    │  │  Ragdoll    │    │   │
│  │  │ Controller  │  │  Physics    │  │  System     │    │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘    │   │
│  │  ┌─────────────┐  ┌─────────────┐                     │   │
│  │  │ Soft Body   │  │   Sensors   │                     │   │
│  │  │   Physics   │  │  Detection  │                     │   │
│  │  └─────────────┘  └─────────────┘                     │   │
│  └────────────────────────────────────────────────────────┘   │
│           ↓                                                    │
│  ┌────────────────────────────────────────────────────────┐   │
│  │           ECOSYSTEM & TOOLS (v0.8)                      │   │
│  │  ┌──────────────┐  ┌──────────────┐                   │   │
│  │  │  Profiler    │  │Debug Renderer│                   │   │
│  │  │  (Perf)      │  │(Visualization)                   │   │
│  │  └──────────────┘  └──────────────┘                   │   │
│  │  ┌──────────────┐  ┌──────────────┐                   │   │
│  │  │Serialization │  │Editor Bridge │                   │   │
│  │  │ (Save/Load)  │  │ (Integration)│                   │   │
│  │  └──────────────┘  └──────────────┘                   │   │
│  └────────────────────────────────────────────────────────┘   │
│                                                                │
└────────────────────────────────────────────────────────────────┘
```

---

## 🎮 Cas d'Usage Typiques

### 1️⃣ Personnage qui Saute (v0.3)

```cpp
// Composants requis: RigidBody
auto player = core.CreateEntity();
player.AddComponent<Physics::Component::RigidBody>();
player.AddComponent<Physics::Component::CapsuleCollider>();

// Appliquer force de saut
Physics::Resource::AddImpulse(core, player, glm::vec3(0, 500, 0));
```

**Fichiers utilisés:**
- `component/RigidBody.hpp`
- `component/CapsuleCollider.hpp`
- `resource/ForceApplicator.hpp`

---

### 2️⃣ Plateforme Mobile (v0.3)

```cpp
// Composants requis: RigidBody (Kinematic)
auto platform = core.CreateEntity();
auto& rb = platform.AddComponent<Physics::Component::RigidBody>();
rb.motionType = MotionType::Kinematic;

// Déplacer plateforme
Physics::Resource::MoveKinematic(core, platform, targetPos, targetRot, dt);
```

**Fichiers utilisés:**
- `component/RigidBody.hpp`
- `resource/KinematicMover.hpp`

---

### 3️⃣ Détection Ligne de Vue (v0.5)

```cpp
// Raycast pour détecter obstacle
auto hit = Physics::Resource::Raycast(core, origin, direction, maxDist);
if (hit.hasHit) {
    Logger::Info("Hit entity at distance: {}", hit.distance);
}
```

**Fichiers utilisés:**
- `resource/RaycastSystem.hpp`
- `query/RaycastHit.hpp`

---

### 4️⃣ Zone de Déclenchement (v0.6)

```cpp
// Créer trigger zone
auto trigger = core.CreateEntity();
trigger.AddComponent<Physics::Component::TriggerVolume>();
trigger.AddComponent<Physics::Component::BoxCollider>();

// Écouter événements
core.GetScheduler<FixedUpdate>().AddSystem([](Core& core) {
    auto view = core.GetRegistry().view<TriggerEvent>();
    for (auto [entity, event] : view.each()) {
        if (event.type == TriggerEventType::Enter) {
            // Joueur entre dans la zone
        }
    }
});
```

**Fichiers utilisés:**
- `component/TriggerVolume.hpp`
- `event/TriggerEvent.hpp`
- `system/TriggerEventSystem.hpp`

---

### 5️⃣ Porte avec Charnière (v0.6)

```cpp
// Créer porte avec contrainte hinge
auto door = core.CreateEntity();
door.AddComponent<Physics::Component::RigidBody>();

auto& hinge = door.AddComponent<Physics::Component::HingeConstraint>();
hinge.axis = glm::vec3(0, 1, 0); // Axe Y
hinge.minAngle = -90.0f;
hinge.maxAngle = 90.0f;
```

**Fichiers utilisés:**
- `component/HingeConstraint.hpp`
- `resource/ConstraintManager.hpp`

---

### 6️⃣ Contrôleur de Personnage (v0.7)

```cpp
// Character controller complet
auto player = core.CreateEntity();
auto& cc = player.AddComponent<Physics::Component::CharacterController>();
cc.height = 2.0f;
cc.radius = 0.5f;
cc.maxSlopeAngle = 45.0f;

// Dans le système de mise à jour
Physics::Resource::MoveCharacter(core, player, velocity, deltaTime);
```

**Fichiers utilisés:**
- `component/CharacterController.hpp`
- `resource/CharacterControllerManager.hpp`
- `system/CharacterControllerSystem.hpp`

---

### 7️⃣ Véhicule Physique (v0.7)

```cpp
// Créer véhicule avec roues
auto car = core.CreateEntity();
auto& vehicle = car.AddComponent<Physics::Component::VehicleController>();

// Ajouter 4 roues
vehicle.AddWheel({position: glm::vec3(-1, 0, 1), radius: 0.4f});
vehicle.AddWheel({position: glm::vec3( 1, 0, 1), radius: 0.4f});
vehicle.AddWheel({position: glm::vec3(-1, 0, -1), radius: 0.4f});
vehicle.AddWheel({position: glm::vec3( 1, 0, -1), radius: 0.4f});

// Contrôler véhicule
Physics::Resource::SetVehicleInput(core, car, throttle, brake, steering);
```

**Fichiers utilisés:**
- `component/VehicleController.hpp`
- `component/WheelController.hpp`
- `resource/VehiclePhysicsManager.hpp`

---

### 8️⃣ Debug Visualisation (v0.8)

```cpp
// Activer debug renderer
auto& debugRenderer = core.GetResource<Physics::Resource::PhysicsDebugRenderer>();
debugRenderer.SetMode(Physics::Debug::DebugRenderMode::Colliders);
debugRenderer.ShowVelocities(true);
debugRenderer.ShowConstraints(true);

// Afficher stats profiler
auto& profiler = core.GetResource<Physics::Resource::PhysicsProfiler>();
auto stats = profiler.GetFrameStats();
Logger::Info("Physics update: {} ms", stats.updateTime);
```

**Fichiers utilisés:**
- `resource/PhysicsDebugRenderer.hpp`
- `resource/PhysicsProfiler.hpp`
- `system/PhysicsDebugDrawSystem.hpp`

---

## 📦 Taille du Binaire (Estimation)

```
Compilé Debug:    ~15-20 MB   (avec symboles debug)
Compilé Release:  ~3-5 MB     (optimisé)
Jolt Physics:     ~2-3 MB     (dépendance)
────────────────────────────────────────
Total Release:    ~5-8 MB
```

---

## 🔧 Configuration Build

```lua
-- xmake.lua
target("physics")
    set_kind("static")
    
    -- Fichiers sources
    add_files("src/**.cpp")
    add_headerfiles("src/**.hpp")
    
    -- Dépendances
    add_deps("engine")
    add_packages("joltphysics", "glm", "entt")
    
    -- Optimisations
    if is_mode("release") then
        set_optimize("aggressive")
        add_defines("JPH_PROFILE_DISABLED")
    end
    
    -- Debug
    if is_mode("debug") then
        add_defines("PHYSICS_DEBUG")
        add_defines("JPH_DEBUG_RENDERER")
        add_defines("JPH_PROFILE_ENABLED")
    end
```

---

## 🎓 Courbe d'Apprentissage

```
Complexité
    ↑
 ⭐⭐⭐⭐⭐ │                                      ●━━━━━━ v0.8
    │                                    ●━━━━━━┘
 ⭐⭐⭐⭐  │                          ●━━━━━┘        v0.7
    │                    ●━━━━━━━━━┘
 ⭐⭐⭐   │          ●━━━━━┘                   v0.6
    │    ●━━━━━━━━┘                    v0.5
 ⭐⭐    │━━━━┘                        v0.4
    │                          v0.3
 ⭐     │
    └─────┴─────┴─────┴─────┴─────┴─────→ Temps d'apprentissage
         1w    2w    4w    8w   12w   16w
```

**Légende:**
- ⭐ Simple - Wrapping direct Jolt
- ⭐⭐ Facile - API directes
- ⭐⭐⭐ Moyen - Gestion d'état
- ⭐⭐⭐⭐ Avancé - Systèmes complexes
- ⭐⭐⭐⭐⭐ Expert - Features avancées

---

**Date:** 2025-11-05  
**Version:** 1.0 - Vue d'ensemble visuelle complète
