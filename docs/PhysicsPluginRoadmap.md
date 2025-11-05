# Physics Plugin - Complete Roadmap & Implementation Guide

> **Document de Synthèse Complet**
> Version 2.0 - Novembre 2025
> Ce document unifie les analyses de RIGIDBODY_USAGE.md, PHYSICS_COMPARISON.md, ARCHITECTURE_FUTURE_COMPATIBILITY.md et l'étude approfondie de Jolt Physics pour guider le développement complet du plugin Physics d'EngineSquared.

---

## 📋 Table des Matières

1. [Vue d'Ensemble](#-vue-densemble)
2. [État Actuel](#-état-actuel)
3. [Analyse Comparative](#-analyse-comparative)
4. [Architecture](#-architecture)
5. [Features Jolt](#-features-jolt-physics)
6. [Roadmap Détaillée](#-roadmap-détaillée)
7. [API Design Patterns](#-api-design-patterns)
8. [Références](#-références)

---

## 🎯 Vue d'Ensemble

### Résumé Consolidé

**Architecture Nouvelle Génération**
- Séparation publique `RigidBody` / interne `RigidBodyInternal` avec composants collider dédiés
- Encapsulation totale de Jolt : API glm-friendly, aucun type Jolt exposé
- Conversions centralisées, gestion automatique du lifecycle via hooks entt

**Features Livrées (v0.2)**
- ✅ Box et Default colliders
- ✅ Motion types: Static, Kinematic, Dynamic
- ✅ Propriétés baseline: mass, friction, restitution, damping, gravity, activation, sleeping
- ✅ Auto-création Transform/DefaultCollider
- ✅ Exemples complets et tests unitaires

**Gaps Actuels**
- ⚠️ Pas d'API forces/impulses/velocity
- ⚠️ Pas de kinematic targets
- ⚠️ Colliders avancés manquants (sphere/capsule/mesh/compound)
- ⚠️ Propriétés fines non exposées (axis locks, CCD, sensors, materials)
- ⚠️ Pas de constraints ni collision queries

**Validation Architecturale**
- ✅ Architecture couvre >95% des features Jolt via patterns extensibles
- ✅ Alignement avec usabilité de Godot/Unreal/Unity
- ✅ API streamlined pour utilisateurs finaux

---

## 📊 État Actuel

### ✅ Fonctionnalités Implémentées (v0.2)

#### Composants Core

```cpp
namespace Physics::Component {
    // ✅ RigidBody - Composant principal
    struct RigidBody {
        MotionType motionType;      // Static, Kinematic, Dynamic
        Activation activation;       // Activate, DontActivate
        float mass = 1.0f;

        static RigidBody CreateStatic();
        static RigidBody CreateKinematic();
        static RigidBody CreateDynamic(float mass);
    };

    // ✅ RigidBodyInternal - Encapsulation Jolt (jamais exposé)
    struct RigidBodyInternal {
        JPH::BodyID bodyID;
    };

    // ✅ BoxCollider
    struct BoxCollider {
        glm::vec3 halfExtents;
        static BoxCollider CreateCube(float halfSize);
    };

    // ✅ DefaultCollider - Auto-généré depuis mesh bounds
    struct DefaultCollider { };
}
```

#### Systèmes

```cpp
namespace Physics::System {
    void InitRigidBodySystem(Core &core);    // ✅ Hooks entt
    void PhysicsUpdate(Core &core);          // ✅ Simulation Jolt
    void SyncTransformSystem(Core &core);    // ✅ Sync Jolt → Transform
}
```

#### Exemple Fonctionnel

L'exemple `physics_usage` valide :
- ✅ Static bodies (sol)
- ✅ Dynamic bodies (cubes tombants)
- ✅ Kinematic bodies (plateformes)
- ✅ BoxCollider + DefaultCollider
- ✅ Friction, restitution, mass
- ✅ 100 steps simulation @ 60 FPS

### ⚠️ TODO Prioritaire

#### 🔴 Priorité HAUTE (v0.3 - Core Completeness)

**1. Forces & Impulsions**
```cpp
namespace Physics::Resource {
    void AddForce(Engine::Core&, Engine::Entity, const glm::vec3 &force);
    void AddForceAtPoint(Engine::Core&, Engine::Entity, const glm::vec3 &force, const glm::vec3 &point);
    void AddTorque(Engine::Core&, Engine::Entity, const glm::vec3 &torque);
    void AddImpulse(Engine::Core&, Engine::Entity, const glm::vec3 &impulse);
    void AddImpulseAtPoint(Engine::Core&, Engine::Entity, const glm::vec3 &impulse, const glm::vec3 &point);
    void AddAngularImpulse(Engine::Core&, Engine::Entity, const glm::vec3 &angularImpulse);
}
```

**2. Velocity Control**
```cpp
namespace Physics::Resource {
    void SetLinearVelocity(Engine::Core&, Engine::Entity, const glm::vec3 &velocity);
    glm::vec3 GetLinearVelocity(Engine::Core&, Engine::Entity);
    void SetAngularVelocity(Engine::Core&, Engine::Entity, const glm::vec3 &angularVel);
    glm::vec3 GetAngularVelocity(Engine::Core&, Engine::Entity);
    void MoveKinematic(Engine::Core&, Engine::Entity, const glm::vec3 &targetPos,
                       const glm::quat &targetRot, float deltaTime);
}
```

**3. Propriétés Physiques Complètes**
```cpp
namespace Physics::Component {
    struct RigidBody {
        // ⚠️ À ajouter
        float friction = 0.5f;
        float restitution = 0.0f;
        float linearDamping = 0.05f;
        float angularDamping = 0.05f;
        float gravityFactor = 1.0f;
        MotionQuality motionQuality = MotionQuality::Discrete;
        bool allowSleeping = true;
    };
}
```

**4. SphereCollider & CapsuleCollider**
```cpp
namespace Physics::Component {
    struct SphereCollider {
        float radius = 0.5f;
    };

    struct CapsuleCollider {
        float height = 1.0f;
        float radius = 0.5f;
    };
}
```

#### 🟡 Priorité MOYENNE (v0.4 - Collider Expansion)

**5. MeshCollider**
```cpp
struct MeshCollider {
    std::vector<glm::vec3> vertices;
    std::vector<uint32_t> indices;
    bool convex = false;  // false = triangle mesh, true = convex hull
};
```

**6. Compound Colliders**
```cpp
struct CompoundCollider {
    struct Child {
        entt::entity colliderEntity;
        glm::vec3 offset;
        glm::quat rotation;
    };
    std::vector<Child> children;
};
```

**7. HeightFieldCollider (Terrain)**
```cpp
struct HeightFieldCollider {
    std::vector<float> heightSamples;
    uint32_t sampleCountX;
    uint32_t sampleCountZ;
    glm::vec3 scale;
};
```

#### 🟢 Priorité BASSE (v0.5+ - Advanced Features)

**8. Collision Queries**
```cpp
struct RayCastResult {
    bool hit;
    entt::entity entity;
    glm::vec3 position;
    glm::vec3 normal;
    float fraction;
};

RayCastResult CastRay(Core&, const glm::vec3 &origin, const glm::vec3 &dir, float maxDist);
std::vector<entt::entity> OverlapSphere(Core&, const glm::vec3 &center, float radius);
```

**9. Sensors & Triggers**
```cpp
struct RigidBody {
    bool isSensor = false;  // Trigger volume
};

// Events
struct TriggerEnterEvent { entt::entity self; entt::entity other; };
struct TriggerExitEvent { entt::entity self; entt::entity other; };
struct CollisionEnterEvent { entt::entity self; entt::entity other; glm::vec3 point; };
```

**10. Constraints & Joints**
```cpp
struct HingeConstraint {
    entt::entity bodyA;
    entt::entity bodyB;
    glm::vec3 anchorA;
    glm::vec3 axisA;
    // Motor, limits...
};

struct SliderConstraint { /* ... */ };
struct FixedConstraint { /* ... */ };
struct SixDOFConstraint { /* ... */ };
```

### 📈 Métriques de Complétion

| Catégorie | Implémenté | P. HAUTE | P. MOYENNE | P. BASSE | Total |
|-----------|------------|----------|------------|----------|-------|
| **Rigid Bodies** | 3/8 (38%) | 3 | 0 | 2 | 8 |
| **Colliders** | 2/7 (29%) | 2 | 3 | 0 | 7 |
| **Forces/Motion** | 0/6 (0%) | 2 | 0 | 1 | 6 |
| **Queries** | 0/4 (0%) | 0 | 0 | 2 | 4 |
| **Constraints** | 0/5 (0%) | 0 | 0 | 5 | 5 |
| **Events** | 0/3 (0%) | 0 | 0 | 3 | 3 |
| **TOTAL** | **5/33 (15%)** | **7** | **3** | **13** | **33** |

---

## 🔍 Analyse Comparative

### Ancien Code (`main` branch)

#### ❌ Problèmes Majeurs

**1. Exposition Types Jolt**
```cpp
// ❌ ANCIEN : Utilisateur doit inclure Jolt
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

struct RigidBody3D {
    std::shared_ptr<JPH::ShapeSettings> shapeSettings;  // ❌ Type Jolt exposé
    JPH::Body *body;                                     // ❌ Pointeur raw exposé
    JPH::EMotionType motionType;                         // ❌ Enum Jolt exposé
    JPH::ObjectLayer layer;                              // ❌ Type Jolt exposé
    std::function<void(JPH::BodyCreationSettings&)> onBodyCreationSettings;  // ❌ Callback Jolt
};
```

**Conséquences :**
- 🔴 Couplage fort : impossible de changer de moteur physique
- 🔴 Complexité : utilisateur doit apprendre Jolt
- 🔴 Headers polluants : Jolt.h requis partout
- 🔴 Pas d'auto-complétion sans connaissance Jolt

**2. Gestion Dangereuse du Body**
```cpp
JPH::Body *body;  // ❌ Dangling pointer possible
```

**Problèmes :**
- 🔴 Ownership ambigu
- 🔴 Null checks partout
- 🔴 Utilisateur peut casser la synchronisation

**3. Double Synchronisation**
```cpp
// Ordre critique :
1. SyncRigidBodiesToTransforms   // Transform → Jolt
2. PhysicsUpdate                  // Simulation
3. SyncTransformsToRigidBodies    // Jolt → Transform
```

**Problèmes :**
- 🔴 Conflit potentiel de source de vérité
- 🔴 Double overhead performance
- 🔴 Ordre d'exécution fragile

### Nouveau Code (branche actuelle)

#### ✅ Améliorations Majeures

**1. Encapsulation Totale**
```cpp
// ✅ NOUVEAU : Aucun type Jolt exposé
#include "Physics.hpp"

struct RigidBody {
    MotionType motionType;      // ✅ Enum propre
    Activation activation;       // ✅ Enum propre
    float mass = 1.0f;          // ✅ Types standard
};

struct RigidBodyInternal {
    JPH::BodyID bodyID;  // ✅ ID immutable, jamais null
};

struct BoxCollider {
    glm::vec3 halfExtents;  // ✅ GLM (déjà utilisé partout)
};
```

**Avantages :**
- 🟢 Découplage total
- 🟢 API simple et familière
- 🟢 Include minimal
- 🟢 Auto-complétion complète

**2. Ownership Clair**
```cpp
// ✅ BodyID = Type-safe, thread-safe
struct RigidBodyInternal {
    JPH::BodyID bodyID;  // Immutable, jamais invalid
};
```

**Avantages :**
- 🟢 PhysicsSystem possède les bodies
- 🟢 Pas de null checks
- 🟢 Encapsulation garantie

**3. Synchronisation Unidirectionnelle**
```cpp
// ✅ Source de vérité : Jolt Physics
void SyncTransformSystem(Core &core) {
    // SEULEMENT Jolt → Transform
    transform.position = FromJoltVec3(bodyInterface.GetPosition(bodyID));
    transform.rotation = FromJoltQuat(bodyInterface.GetRotation(bodyID));
}
```

**Avantages :**
- 🟢 Pas de conflit
- 🟢 Une seule sync par frame
- 🟢 Ordre d'exécution simple

### Tableau Comparatif Complet

| Feature | Ancien | Nouveau | Verdict |
|---------|--------|---------|---------|
| **Encapsulation** |
| Types Jolt exposés | ❌ Oui | ✅ Non | 🟢 **MAJEUR** |
| Include Jolt requis | ❌ Oui | ✅ Non | 🟢 **MAJEUR** |
| API simple | ❌ Non | ✅ Oui | 🟢 **MAJEUR** |
| **Colliders** |
| Box | ✅ Via ShapeSettings | ✅ BoxCollider | 🟡 Équivalent |
| Sphere | ✅ Via ShapeSettings | ⚠️ TODO | 🔴 Manquant |
| Capsule | ✅ Via ShapeSettings | ⚠️ TODO | 🔴 Manquant |
| Mesh | ✅ Via ShapeSettings | ⚠️ TODO | 🔴 Manquant |
| Compound | ✅ Via ShapeSettings | ⚠️ TODO | 🔴 Manquant |
| Default auto | ❌ Non | ✅ Oui | 🟢 **Amélioration** |
| **Properties** |
| Mass | 🟡 Via callback | ✅ Direct | 🟢 Amélioration |
| Friction | 🟡 Via callback | ⚠️ TODO | 🔴 Manquant |
| Restitution | 🟡 Via callback | ⚠️ TODO | 🔴 Manquant |
| Damping | 🟡 Via callback | ⚠️ TODO | 🔴 Manquant |
| **Forces** |
| AddForce | 🟡 Via body* | ⚠️ TODO | 🔴 Manquant |
| AddImpulse | 🟡 Via body* | ⚠️ TODO | 🔴 Manquant |
| SetVelocity | 🟡 Via body* | ⚠️ TODO | 🔴 Manquant |
| **Architecture** |
| Séparation Public/Internal | ❌ Non | ✅ Oui | 🟢 **MAJEUR** |
| Type safety | ❌ Body* | ✅ BodyID | 🟢 **MAJEUR** |
| Sync unidirectionnelle | ❌ Non | ✅ Oui | 🟢 Amélioration |
| **Tests** |
| Tests unitaires | ❌ Non | ✅ Oui | 🟢 **Amélioration** |
| Exemples complets | ❌ Non | ✅ Oui | 🟢 **Amélioration** |

**Bilan :**
- ✅ **8 améliorations majeures**
- 🟡 **5 équivalents**
- ❌ **7 fonctionnalités à réimplémenter**

---

---

## 🏗️ Architecture

### Principes Fondamentaux

#### 1. Séparation des Responsabilités

```
┌─────────────────────────────────────────┐
│         API Publique (Physics)          │
│  - RigidBody (propriétés physiques)     │
│  - Colliders (BoxCollider, Sphere...)   │
│  - Fonctions utilitaires (AddForce...)  │
└─────────────────┬───────────────────────┘
                  │ Utilise
┌─────────────────▼───────────────────────┐
│      Composants Internes (jamais        │
│             vus par user)               │
│  - RigidBodyInternal (BodyID)           │
│  - PhysicsManager (PhysicsSystem Jolt)  │
└─────────────────┬───────────────────────┘
                  │ Encapsule
┌─────────────────▼───────────────────────┐
│          Jolt Physics Engine            │
│  - JPH::PhysicsSystem                   │
│  - JPH::BodyInterface                   │
│  - JPH::Shape*, JPH::Constraint*        │
└─────────────────────────────────────────┘
```

#### 2. Pattern Composant/Internal

**Pour chaque feature physics :**

```cpp
// Composant PUBLIC (propriétés, configuration)
namespace Physics::Component {
    struct FeatureName {
        // Propriétés utilisateur (types moteur uniquement)
        float property1;
        glm::vec3 property2;

        // Factory methods
        static FeatureName Create(...);
    };
}

// Composant INTERNE (handle Jolt, jamais exposé)
namespace Physics::Component {
    struct FeatureNameInternal {
        JPH::SomeID joltHandle;  // ID, pas de pointeur
    };
}

// Système (gestion lifecycle + logique)
namespace Physics::System {
    void InitFeatureSystem(Core &core) {
        // Hooks entt pour création/destruction automatique
        core.GetRegistry().on_construct<FeatureName>()
            .connect<&OnFeatureConstruct>();
        core.GetRegistry().on_destroy<FeatureName>()
            .connect<&OnFeatureDestroy>();
    }

    void OnFeatureConstruct(entt::registry &registry, entt::entity entity);
    void OnFeatureDestroy(entt::registry &registry, entt::entity entity);
}
```

**Exemple Concret : RigidBody**

```cpp
// PUBLIC
struct RigidBody {
    MotionType motionType;
    float mass;
    // ... autres propriétés
};

// INTERNAL (jamais exposé)
struct RigidBodyInternal {
    JPH::BodyID bodyID;  // ✅ ID = type-safe, thread-safe
};

// SYSTEM
void InitRigidBodySystem(Core &core) {
    core.GetRegistry().on_construct<RigidBody>()
        .connect<&OnRigidBodyConstruct>();
}

void OnRigidBodyConstruct(entt::registry &registry, entt::entity entity) {
    auto &rb = registry.get<RigidBody>(entity);

    // 1. Récupérer/créer Transform
    auto &transform = GetOrCreateTransform(registry, entity);

    // 2. Récupérer/créer Collider
    JPH::ShapeSettings *shape = GetOrCreateColliderShape(registry, entity);

    // 3. Créer BodyCreationSettings depuis RigidBody
    JPH::BodyCreationSettings settings(
        shape,
        ToJoltVec3(transform.position),
        ToJoltQuat(transform.rotation),
        ToJoltMotionType(rb.motionType),
        ToJoltLayer(rb.objectLayer)
    );

    settings.mMassPropertiesOverride.mMass = rb.mass;
    // ... autres propriétés

    // 4. Créer body Jolt
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();
    JPH::BodyID bodyID = physicsManager.CreateBody(settings);

    // 5. Stocker BodyID dans Internal
    registry.emplace<RigidBodyInternal>(entity, bodyID);

    // 6. Activer si demandé
    if (rb.activation == Activation::Activate) {
        physicsManager.ActivateBody(bodyID);
    }
}
```

#### 3. Conversions Centralisées

```cpp
// JoltConversions.hpp - Toutes les conversions au même endroit
namespace Physics::Resource {
    // Vec3
    JPH::Vec3 ToJoltVec3(const glm::vec3 &v);
    glm::vec3 FromJoltVec3(const JPH::Vec3 &v);

    // Quaternion
    JPH::Quat ToJoltQuat(const glm::quat &q);
    glm::quat FromJoltQuat(const JPH::Quat &q);

    // Enums
    JPH::EMotionType ToJoltMotionType(MotionType mt);
    MotionType FromJoltMotionType(JPH::EMotionType mt);

    // Shapes
    JPH::ShapeSettings* CreateJoltShape(const BoxCollider &collider);
    JPH::ShapeSettings* CreateJoltShape(const SphereCollider &collider);
    // ...
}
```

#### 4. PhysicsManager - Hub Central

```cpp
namespace Physics::Resource {
    class PhysicsManager {
    private:
        // Jolt core
        JPH::PhysicsSystem physicsSystem;
        JPH::BodyInterface &bodyInterface;

        // Mappings
        std::unordered_map<JPH::BodyID, entt::entity> bodyToEntity;

        // Listeners
        std::unique_ptr<ContactListener> contactListener;

    public:
        // Body management
        JPH::BodyID CreateBody(const JPH::BodyCreationSettings &settings);
        void DestroyBody(JPH::BodyID bodyID);
        void ActivateBody(JPH::BodyID bodyID);

        // Queries
        RayCastResult CastRay(const glm::vec3 &origin, const glm::vec3 &dir, float maxDist);

        // Simulation
        void Update(float deltaTime);

        // Mapping
        entt::entity GetEntity(JPH::BodyID bodyID) const;
        JPH::BodyID GetBodyID(entt::entity entity) const;
    };
}
```

### Extensibilité Validée

**L'architecture supporte 95%+ des features Jolt sans refactoring :**

| Feature Jolt | Compatible | Pattern | Effort |
|--------------|------------|---------|--------|
| **Shapes** | ✅ 100% | Composant collider + dispatcher | Faible |
| **Body Properties** | ✅ 100% | Membres RigidBody | Faible |
| **Forces** | ✅ 100% | Fonctions Physics namespace | Faible |
| **Constraints** | ✅ 100% | Composant + Internal + System | Moyen |
| **Queries** | ✅ 100% | Fonctions + PhysicsManager | Moyen |
| **Sensors** | ✅ 100% | Membre + ContactListener | Moyen |
| **Soft Bodies** | ⚠️ 95% | Système parallèle (même pattern) | Élevé |
| **Vehicles** | ⚠️ 95% | Addon system | Élevé |
| **Characters** | ✅ 100% | Plugin séparé | Moyen |

---

## 📚 Features Jolt Physics

### Vue d'Ensemble Complète

Jolt Physics offre (source : https://github.com/jrouwe/JoltPhysics) :

#### 1. Shapes (Formes de Collision)

**Primitives :**
- ✅ Sphere
- ✅ Box
- ✅ Capsule
- ✅ TaperedCapsule
- ✅ Cylinder
- ✅ TaperedCylinder

**Complexes :**
- ✅ ConvexHull
- ✅ Mesh (triangle mesh)
- ✅ HeightField (terrain)
- ✅ Plane

**Composites :**
- ✅ StaticCompound
- ✅ MutableCompound
- ✅ RotatedTranslated
- ✅ Scaled
- ✅ OffsetCenterOfMass

**Implémentation EngineSquared :**

```cpp
// Pattern unifié pour tous les colliders
namespace Physics::Component {
    struct SphereCollider {
        float radius = 0.5f;
    };

    struct CapsuleCollider {
        float height = 1.0f;
        float radius = 0.5f;
    };

    struct CylinderCollider {
        float height = 1.0f;
        float radius = 0.5f;
    };

    struct ConvexHullCollider {
        std::vector<glm::vec3> points;
    };

    struct MeshCollider {
        std::vector<glm::vec3> vertices;
        std::vector<uint32_t> indices;
        bool convex = false;  // false = triangle mesh, true = convex
    };

    struct HeightFieldCollider {
        std::vector<float> heightSamples;
        uint32_t sampleCountX;
        uint32_t sampleCountZ;
        glm::vec3 scale = glm::vec3(1.0f);
        glm::vec3 offset = glm::vec3(0.0f);
    };

    struct CompoundCollider {
        struct Child {
            entt::entity colliderEntity;  // Entity avec son propre collider
            glm::vec3 offset = glm::vec3(0.0f);
            glm::quat rotation = glm::quat(1, 0, 0, 0);
        };
        std::vector<Child> children;
        bool mutable_ = false;  // true = MutableCompound (modifiable runtime)
    };
}

// Dispatcher dans RigidBodySystem
JPH::ShapeSettings* GetColliderShape(entt::registry &registry, entt::entity entity) {
    // Priorité : Explicit > Compound > Default
    if (registry.all_of<BoxCollider>(entity)) {
        return Resource::CreateJoltShape(registry.get<BoxCollider>(entity));
    }
    if (registry.all_of<SphereCollider>(entity)) {
        return Resource::CreateJoltShape(registry.get<SphereCollider>(entity));
    }
    // ... tous les autres colliders
    if (registry.all_of<CompoundCollider>(entity)) {
        return Resource::CreateCompoundShape(registry, registry.get<CompoundCollider>(entity));
    }
    if (registry.all_of<DefaultCollider>(entity)) {
        return Resource::CreateDefaultShape(registry, entity);
    }

    // Pas de collider : créer DefaultCollider
    registry.emplace<DefaultCollider>(entity);
    return Resource::CreateDefaultShape(registry, entity);
}
```

#### 2. Body Properties (Propriétés Physiques)

**BodyCreationSettings Jolt expose :**

```cpp
// Mapping complet dans RigidBody
struct RigidBody {
    // Motion
    MotionType motionType = MotionType::Dynamic;
    MotionQuality motionQuality = MotionQuality::Discrete;  // CCD

    // Mass properties
    float mass = 1.0f;
    glm::vec3 inertia = glm::vec3(0.0f);  // 0 = auto-calculate
    float inertiaMultiplier = 1.0f;

    // Surface properties
    float friction = 0.5f;
    float restitution = 0.0f;  // Bounciness

    // Damping (air resistance)
    float linearDamping = 0.05f;
    float angularDamping = 0.05f;

    // Gravity
    float gravityFactor = 1.0f;  // 1.0 = normal, 0.0 = no gravity

    // Activation
    Activation activation = Activation::Activate;
    bool allowSleeping = true;

    // Collision
    uint16_t objectLayer = 0;  // Collision layer
    bool isSensor = false;     // Trigger volume

    // Advanced
    bool useManifoldReduction = true;
    bool applyGyroscopicForce = false;
    bool enhancedInternalEdgeRemoval = false;
    bool collideKinematicVsNonDynamic = false;

    // Axis locks
    bool lockPositionX = false;
    bool lockPositionY = false;
    bool lockPositionZ = false;
    bool lockRotationX = false;
    bool lockRotationY = false;
    bool lockRotationZ = false;
};
```

#### 3. Forces & Motion Control

**BodyInterface Jolt APIs :**

```cpp
namespace Physics {
    // Forces (appliquées au centre de masse)
    void AddForce(Core &core, entt::entity entity, const glm::vec3 &force);
    void AddTorque(Core &core, entt::entity entity, const glm::vec3 &torque);

    // Forces à un point
    void AddForceAtPoint(Core &core, entt::entity entity,
                         const glm::vec3 &force, const glm::vec3 &point);

    // Impulsions (instantanées)
    void AddImpulse(Core &core, entt::entity entity, const glm::vec3 &impulse);
    void AddAngularImpulse(Core &core, entt::entity entity, const glm::vec3 &angularImpulse);
    void AddImpulseAtPoint(Core &core, entt::entity entity,
                           const glm::vec3 &impulse, const glm::vec3 &point);

    // Velocity control
    void SetLinearVelocity(Core &core, entt::entity entity, const glm::vec3 &velocity);
    glm::vec3 GetLinearVelocity(Core &core, entt::entity entity);
    void AddLinearVelocity(Core &core, entt::entity entity, const glm::vec3 &deltaVelocity);

    void SetAngularVelocity(Core &core, entt::entity entity, const glm::vec3 &angularVel);
    glm::vec3 GetAngularVelocity(Core &core, entt::entity entity);
    void AddAngularVelocity(Core &core, entt::entity entity, const glm::vec3 &deltaAngularVel);

    // Kinematic movement
    void MoveKinematic(Core &core, entt::entity entity,
                       const glm::vec3 &targetPosition,
                       const glm::quat &targetRotation,
                       float deltaTime);

    // Position/Rotation (pour static/kinematic)
    void SetPosition(Core &core, entt::entity entity, const glm::vec3 &position);
    void SetRotation(Core &core, entt::entity entity, const glm::quat &rotation);

    // Activation control
    void ActivateBody(Core &core, entt::entity entity);
    void DeactivateBody(Core &core, entt::entity entity);
    bool IsActive(Core &core, entt::entity entity);
}
```

#### 4. Constraints (Contraintes & Joints)

**Types Jolt :**
- Fixed (soudure)
- Point (ball-socket)
- Distance (spring/rope)
- Hinge (charnière)
- Slider (prismatic)
- Cone (cone limit)
- SwingTwist (épaule humanoïde)
- SixDOF (6 degrés de liberté)
- Path (spline suivie)
- Gear (engrenage)
- RackAndPinion
- Pulley (poulie)
- Vehicle (système véhicule complet)

**Pattern EngineSquared :**

```cpp
namespace Physics::Component {
    // Base commune
    struct Constraint {
        entt::entity bodyA = entt::null;
        entt::entity bodyB = entt::null;  // entt::null = world
        bool collideConnected = false;

        // Breakable
        float maxForce = FLT_MAX;
        float maxTorque = FLT_MAX;
    };

    // Hinge (charnière)
    struct HingeConstraint : Constraint {
        glm::vec3 anchorA = glm::vec3(0.0f);
        glm::vec3 anchorB = glm::vec3(0.0f);
        glm::vec3 axisA = glm::vec3(0, 1, 0);
        glm::vec3 axisB = glm::vec3(0, 1, 0);

        // Limits
        bool enableLimits = false;
        float minAngle = -glm::pi<float>();
        float maxAngle = glm::pi<float>();

        // Motor
        bool enableMotor = false;
        float motorTargetVelocity = 0.0f;
        float motorMaxTorque = 0.0f;
    };

    // Slider (prismatic)
    struct SliderConstraint : Constraint {
        glm::vec3 anchorA = glm::vec3(0.0f);
        glm::vec3 anchorB = glm::vec3(0.0f);
        glm::vec3 axisA = glm::vec3(1, 0, 0);
        glm::vec3 axisB = glm::vec3(1, 0, 0);

        // Limits
        bool enableLimits = false;
        float minDistance = -FLT_MAX;
        float maxDistance = FLT_MAX;

        // Motor
        bool enableMotor = false;
        float motorTargetVelocity = 0.0f;
        float motorMaxForce = 0.0f;
    };

    // Fixed (soudure)
    struct FixedConstraint : Constraint {
        glm::vec3 anchorA = glm::vec3(0.0f);
        glm::vec3 anchorB = glm::vec3(0.0f);
        glm::quat rotationA = glm::quat(1, 0, 0, 0);
        glm::quat rotationB = glm::quat(1, 0, 0, 0);
    };

    // ... Autres constraints
}

// Internal + System (même pattern que RigidBody)
namespace Physics::Component {
    struct HingeConstraintInternal {
        JPH::Constraint *constraint;  // Owned par PhysicsSystem
    };
}

namespace Physics::System {
    void InitConstraintSystem(Core &core);
    void OnHingeConstraintConstruct(entt::registry &registry, entt::entity entity);
    void OnHingeConstraintDestroy(entt::registry &registry, entt::entity entity);
}

// Events pour breakable constraints
namespace Physics {
    struct ConstraintBreakEvent {
        entt::entity constraintEntity;
        entt::entity bodyA;
        entt::entity bodyB;
        float breakForce;
    };
}
```

#### 5. Collision Queries

**Jolt APIs :**
- BroadPhaseQuery : tests rapides (AABBs)
- NarrowPhaseQuery : tests précis (shapes)

**EngineSquared API :**

```cpp
namespace Physics {
    // Raycast
    struct RayCastResult {
        bool hit = false;
        entt::entity entity = entt::null;
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 normal = glm::vec3(0.0f);
        float fraction = 0.0f;  // [0, 1] le long du rayon
    };

    RayCastResult CastRay(Core &core,
                          const glm::vec3 &origin,
                          const glm::vec3 &direction,
                          float maxDistance,
                          uint32_t layerMask = 0xFFFFFFFF);

    std::vector<RayCastResult> CastRayAll(Core &core,
                                          const glm::vec3 &origin,
                                          const glm::vec3 &direction,
                                          float maxDistance,
                                          uint32_t layerMask = 0xFFFFFFFF);

    // Shape cast
    struct ShapeCastResult {
        bool hit = false;
        entt::entity entity = entt::null;
        glm::vec3 contactPoint = glm::vec3(0.0f);
        glm::vec3 contactNormal = glm::vec3(0.0f);
        float fraction = 0.0f;
    };

    ShapeCastResult CastSphere(Core &core,
                               const glm::vec3 &center,
                               float radius,
                               const glm::vec3 &direction,
                               float maxDistance,
                               uint32_t layerMask = 0xFFFFFFFF);

    ShapeCastResult CastBox(Core &core,
                            const glm::vec3 &center,
                            const glm::vec3 &halfExtents,
                            const glm::quat &rotation,
                            const glm::vec3 &direction,
                            float maxDistance,
                            uint32_t layerMask = 0xFFFFFFFF);

    // Overlap queries
    std::vector<entt::entity> OverlapSphere(Core &core,
                                            const glm::vec3 &center,
                                            float radius,
                                            uint32_t layerMask = 0xFFFFFFFF);

    std::vector<entt::entity> OverlapBox(Core &core,
                                         const glm::vec3 &center,
                                         const glm::vec3 &halfExtents,
                                         const glm::quat &rotation,
                                         uint32_t layerMask = 0xFFFFFFFF);

    std::vector<entt::entity> OverlapAABB(Core &core,
                                          const glm::vec3 &min,
                                          const glm::vec3 &max,
                                          uint32_t layerMask = 0xFFFFFFFF);

    // Body queries
    glm::vec3 GetWorldSpaceBounds(Core &core, entt::entity entity);
    glm::vec3 GetWorldSpaceSurfaceNormal(Core &core, entt::entity entity, const glm::vec3 &point);
}
```

#### 6. Collision Filtering

**Jolt Layers :**
- ObjectLayer (16 ou 32 bits)
- BroadPhaseLayer
- GroupFilter (ragdolls)

**EngineSquared API :**

```cpp
namespace Physics {
    // Layers prédéfinies
    enum class CollisionLayer : uint16_t {
        Static = 0,
        Dynamic = 1,
        Kinematic = 2,
        Player = 3,
        Enemy = 4,
        Projectile = 5,
        Trigger = 6,
        Debris = 7,
        // ... jusqu'à 16 ou 32 layers
    };

    // Configuration globale (dans PhysicsManager)
    void SetLayerCollision(CollisionLayer layerA, CollisionLayer layerB, bool collide);
    bool GetLayerCollision(CollisionLayer layerA, CollisionLayer layerB);

    // Par body
    void SetCollisionLayer(Core &core, entt::entity entity, CollisionLayer layer);
    CollisionLayer GetCollisionLayer(Core &core, entt::entity entity);

    // Collision mask (quelles layers ce body peut toucher)
    void SetCollisionMask(Core &core, entt::entity entity, uint32_t mask);
    uint32_t GetCollisionMask(Core &core, entt::entity entity);

    // Group filter (pour ragdolls : bodies du même groupe ne collident pas)
    void SetCollisionGroup(Core &core, entt::entity entity, uint32_t groupID);
    uint32_t GetCollisionGroup(Core &core, entt::entity entity);
}
```

#### 7. Sensors & Events

**Jolt ContactListener :**

```cpp
class EngineContactListener : public JPH::ContactListener {
    entt::registry *registry;

public:
    // Validation (filtrer collisions avant calcul)
    ValidateResult OnContactValidate(const Body &body1, const Body &body2, ...) override;

    // Callbacks
    void OnContactAdded(const Body &body1, const Body &body2, const ContactManifold &manifold, ContactSettings &settings) override;
    void OnContactPersisted(const Body &body1, const Body &body2, const ContactManifold &manifold, ContactSettings &settings) override;
    void OnContactRemoved(const SubShapeIDPair &subShapePair) override;
};
```

**EngineSquared Events :**

```cpp
namespace Physics {
    // Collision events (bodies non-sensor)
    struct CollisionEnterEvent {
        entt::entity self;
        entt::entity other;
        glm::vec3 contactPoint;
        glm::vec3 contactNormal;
        float penetrationDepth;
    };

    struct CollisionStayEvent {
        entt::entity self;
        entt::entity other;
        glm::vec3 contactPoint;
        glm::vec3 contactNormal;
    };

    struct CollisionExitEvent {
        entt::entity self;
        entt::entity other;
    };

    // Trigger events (sensor bodies)
    struct TriggerEnterEvent {
        entt::entity sensor;
        entt::entity other;
    };

    struct TriggerStayEvent {
        entt::entity sensor;
        entt::entity other;
    };

    struct TriggerExitEvent {
        entt::entity sensor;
        entt::entity other;
    };
}

// Usage dans gameplay code
void OnTriggerEnter(const TriggerEnterEvent &event) {
    if (registry.all_of<PlayerTag>(event.other)) {
        // Player entered trigger zone
        registry.emplace<ActivateEvent>(event.sensor);
    }
}

// Registration
core.GetRegistry().on_construct<TriggerEnterEvent>()
    .connect<&OnTriggerEnter>();
```

#### 8. Advanced Features

**Sleeping (optimisation)**
```cpp
namespace Physics {
    void SetAllowSleeping(Core &core, entt::entity entity, bool allow);
    void ActivateBody(Core &core, entt::entity entity);
    void DeactivateBody(Core &core, entt::entity entity);
    void ActivateBodiesInAABB(Core &core, const glm::vec3 &min, const glm::vec3 &max);
    bool IsActive(Core &core, entt::entity entity);
}
```

**Materials (friction/restitution presets)**
```cpp
namespace Physics {
    struct PhysicsMaterial {
        std::string name;
        float friction = 0.5f;
        float restitution = 0.0f;
    };

    // Presets
    PhysicsMaterial Material_Ice();        // friction=0.1, restitution=0.0
    PhysicsMaterial Material_Rubber();     // friction=0.9, restitution=0.8
    PhysicsMaterial Material_Wood();       // friction=0.6, restitution=0.2

    // Application
    void SetMaterial(Core &core, entt::entity entity, const PhysicsMaterial &material);
}
```

**Ragdolls**
```cpp
namespace Physics {
    struct Ragdoll {
        entt::entity rootBone;
        std::vector<entt::entity> bones;
        std::vector<entt::entity> constraints;
        uint32_t collisionGroup;  // Bones don't collide with each other
    };

    Ragdoll CreateRagdollFromSkeleton(Core &core, const Skeleton &skeleton);
    void SetRagdollActive(Core &core, Ragdoll &ragdoll, bool active);
}
```

**Vehicles (système dédié)**
```cpp
namespace Physics {
    struct Vehicle {
        entt::entity chassis;  // RigidBody
        std::vector<Wheel> wheels;
        float enginePower;
        float maxSteeringAngle;
    };

    struct Wheel {
        entt::entity visualEntity;  // Pour rotation visuelle
        glm::vec3 suspensionOffset;
        float radius;
        float suspensionStiffness;
        float dampingCompression;
        float dampingRelaxation;
    };

    void UpdateVehicle(Core &core, Vehicle &vehicle, float steering, float throttle, float brake);
}
```

**Soft Bodies (système séparé)**
```cpp
namespace Physics::Component {
    struct SoftBody {
        std::vector<glm::vec3> vertices;
        std::vector<uint32_t> indices;

        // Constraints
        float edgeStiffness = 1.0f;
        float bendStiffness = 0.5f;
        float volumeStiffness = 0.0f;  // Internal pressure

        // Collision
        bool collideWithRigidBodies = true;
        uint16_t objectLayer = 0;
    };

    struct SoftBodyInternal {
        JPH::SoftBodyID softBodyID;
    };
}
```

---

---

## 🗺️ Roadmap Détaillée

### Vue d'Ensemble des Milestones

```
v0.2 (ACTUEL) ────────────────────────────────> Feature Complete (v1.0)
    │
    ├─> v0.3: Core Completeness (2-3 semaines)
    │     • Forces & Velocity API
    │     • Propriétés physiques complètes
    │     • Sphere & Capsule colliders
    │
    ├─> v0.4: Collider Expansion (3-4 semaines)
    │     • Mesh collider
    │     • Convex hull
    │     • Compound colliders
    │     • HeightField (terrain)
    │
    ├─> v0.5: Queries & Events (2-3 semaines)
    │     • Raycast & Shape cast
    │     • Overlap queries
    │     • Collision/Trigger events
    │     • Layers & filtering
    │
    ├─> v0.6: Constraints (4-5 semaines)
    │     • Hinge, Slider, Fixed
    │     • Distance, Cone, SwingTwist
    │     • SixDOF
    │     • Motors & breakable
    │
    ├─> v0.7: Advanced Dynamics (3-4 semaines)
    │     • Character controllers
    │     • Ragdolls
    │     • Materials system
    │     • Debug visualization
    │
    └─> v0.8+: Ecosystem (4-6 semaines)
          • Vehicles
          • Soft bodies
          • Water buoyancy
          • Performance tools
```

---

### 🔴 v0.3: Core Completeness (Priorité HAUTE)

**Objectif :** Rendre RigidBody feature-complete pour usage basique
**Durée estimée :** 2-3 semaines
**Effort :** ~80-120 heures

#### Phase 3.1: Forces & Velocity API (1 semaine)

**Tasks :**

1. **Implémenter Forces API** (1-2 jours)
   ```cpp
   // À créer : src/plugin/physics/src/api/ForcesAPI.hpp
   namespace Physics {
       void AddForce(Core &core, entt::entity entity, const glm::vec3 &force);
       void AddForceAtPoint(Core &core, entt::entity entity,
                            const glm::vec3 &force, const glm::vec3 &point);
       void AddTorque(Core &core, entt::entity entity, const glm::vec3 &torque);

       void AddImpulse(Core &core, entt::entity entity, const glm::vec3 &impulse);
       void AddImpulseAtPoint(Core &core, entt::entity entity,
                              const glm::vec3 &impulse, const glm::vec3 &point);
       void AddAngularImpulse(Core &core, entt::entity entity, const glm::vec3 &angularImpulse);
   }
   ```

   **Implémentation :**
   ```cpp
   void AddForce(Core &core, entt::entity entity, const glm::vec3 &force) {
       auto &registry = core.GetRegistry();
       if (!registry.all_of<RigidBodyInternal>(entity)) {
           Logger::Error("Entity has no RigidBody");
           return;
       }

       auto &internal = registry.get<RigidBodyInternal>(entity);
       auto &physicsManager = core.GetResource<Resource::PhysicsManager>();

       // Thread-safe : BodyInterface garantit thread-safety
       physicsManager.GetBodyInterface().AddForce(
           internal.bodyID,
           Resource::ToJoltVec3(force)
       );
   }
   ```

   **Checklist :**
   - [ ] Implémenter toutes les fonctions forces
   - [ ] Tests unitaires (ForceTest.cpp)
   - [ ] Exemple dans physics_usage (cube propulsé par force)
   - [ ] Documentation API

2. **Implémenter Velocity API** (1 jour)
   ```cpp
   // À ajouter dans ForcesAPI.hpp
   namespace Physics {
       void SetLinearVelocity(Core &core, entt::entity entity, const glm::vec3 &velocity);
       glm::vec3 GetLinearVelocity(Core &core, entt::entity entity);
       void AddLinearVelocity(Core &core, entt::entity entity, const glm::vec3 &deltaVel);

       void SetAngularVelocity(Core &core, entt::entity entity, const glm::vec3 &angularVel);
       glm::vec3 GetAngularVelocity(Core &core, entt::entity entity);
       void AddAngularVelocity(Core &core, entt::entity entity, const glm::vec3 &deltaAngularVel);
   }
   ```

   **Checklist :**
   - [ ] Implémentation
   - [ ] Tests unitaires
   - [ ] Exemple (contrôle direct de velocity)

3. **Kinematic Target API** (1-2 jours)
   ```cpp
   namespace Physics {
       void MoveKinematic(Core &core, entt::entity entity,
                          const glm::vec3 &targetPosition,
                          const glm::quat &targetRotation,
                          float deltaTime);

       // Helper pour mouvement simple
       void SetKinematicVelocity(Core &core, entt::entity entity, const glm::vec3 &velocity);
   }
   ```

   **Checklist :**
   - [ ] Implémentation
   - [ ] Tests unitaires
   - [ ] Exemple (plateforme mobile)
   - [ ] Validation collision kinematic vs dynamic

4. **Tests d'Intégration** (1 jour)
   - [ ] Scène avec forces multiples
   - [ ] Test impulse vs force
   - [ ] Test kinematic movement
   - [ ] Performance benchmark (1000+ bodies avec forces)

#### Phase 3.2: Propriétés Physiques (3-4 jours)

**Tasks :**

1. **Étendre RigidBody Component** (1 jour)
   ```cpp
   // À modifier : src/plugin/physics/src/component/RigidBody.hpp
   struct RigidBody {
       // Existing
       MotionType motionType = MotionType::Dynamic;
       Activation activation = Activation::Activate;
       float mass = 1.0f;

       // ⚠️ À AJOUTER :
       // Surface properties
       float friction = 0.5f;
       float restitution = 0.0f;

       // Damping
       float linearDamping = 0.05f;
       float angularDamping = 0.05f;

       // Gravity
       float gravityFactor = 1.0f;

       // CCD
       MotionQuality motionQuality = MotionQuality::Discrete;

       // Sleeping
       bool allowSleeping = true;

       // Collision
       uint16_t objectLayer = 0;
   };
   ```

2. **Mettre à Jour OnRigidBodyConstruct** (1 jour)
   ```cpp
   // À modifier : src/plugin/physics/src/system/RigidBodySystem.cpp
   void OnRigidBodyConstruct(entt::registry &registry, entt::entity entity) {
       // ... existing code ...

       // ⚠️ Appliquer nouvelles propriétés
       settings.mFriction = rb.friction;
       settings.mRestitution = rb.restitution;
       settings.mLinearDamping = rb.linearDamping;
       settings.mAngularDamping = rb.angularDamping;
       settings.mGravityFactor = rb.gravityFactor;
       settings.mMotionQuality = ToJoltMotionQuality(rb.motionQuality);
       settings.mAllowSleeping = rb.allowSleeping;
       settings.mObjectLayer = rb.objectLayer;

       // ... rest of function ...
   }
   ```

3. **Runtime Property Modification** (1 jour)
   ```cpp
   // À créer : src/plugin/physics/src/api/PropertyAPI.hpp
   namespace Physics {
       // Friction
       void SetFriction(Core &core, entt::entity entity, float friction);
       float GetFriction(Core &core, entt::entity entity);

       // Restitution
       void SetRestitution(Core &core, entt::entity entity, float restitution);
       float GetRestitution(Core &core, entt::entity entity);

       // Damping
       void SetLinearDamping(Core &core, entt::entity entity, float damping);
       void SetAngularDamping(Core &core, entt::entity entity, float damping);

       // Gravity
       void SetGravityFactor(Core &core, entt::entity entity, float factor);

       // Motion quality (CCD)
       void SetMotionQuality(Core &core, entt::entity entity, MotionQuality quality);
   }
   ```

4. **Tests & Documentation** (1 jour)
   - [ ] Tests unitaires pour chaque propriété
   - [ ] Test modification runtime
   - [ ] Exemple bouncy ball (high restitution)
   - [ ] Exemple damping (air resistance)
   - [ ] Mettre à jour RIGIDBODY_USAGE.md

#### Phase 3.3: Sphere & Capsule Colliders (3-4 jours)

**Tasks :**

1. **SphereCollider Component** (1 jour)
   ```cpp
   // À créer : src/plugin/physics/src/component/SphereCollider.hpp
   namespace Physics::Component {
       struct SphereCollider {
           float radius = 0.5f;

           static SphereCollider Create(float radius) {
               return {radius};
           }
       };
   }
   ```

   **Implémentation builder :**
   ```cpp
   // À ajouter dans JoltConversions.hpp
   namespace Physics::Resource {
       JPH::ShapeSettings* CreateJoltShape(const SphereCollider &collider) {
           return new JPH::SphereShapeSettings(collider.radius);
       }
   }
   ```

2. **CapsuleCollider Component** (1 jour)
   ```cpp
   // À créer : src/plugin/physics/src/component/CapsuleCollider.hpp
   namespace Physics::Component {
       struct CapsuleCollider {
           float height = 1.0f;  // Distance between sphere centers
           float radius = 0.5f;

           static CapsuleCollider Create(float height, float radius) {
               return {height, radius};
           }
       };
   }
   ```

   **Builder :**
   ```cpp
   JPH::ShapeSettings* CreateJoltShape(const CapsuleCollider &collider) {
       return new JPH::CapsuleShapeSettings(
           collider.height / 2.0f,  // half height
           collider.radius
       );
   }
   ```

3. **Mettre à Jour Dispatcher** (1 jour)
   ```cpp
   // À modifier : RigidBodySystem.cpp
   JPH::ShapeSettings* GetColliderShape(entt::registry &registry, entt::entity entity) {
       // Priority order
       if (registry.all_of<BoxCollider>(entity)) {
           return Resource::CreateJoltShape(registry.get<BoxCollider>(entity));
       }
       // ⚠️ AJOUTER :
       if (registry.all_of<SphereCollider>(entity)) {
           return Resource::CreateJoltShape(registry.get<SphereCollider>(entity));
       }
       if (registry.all_of<CapsuleCollider>(entity)) {
           return Resource::CreateJoltShape(registry.get<CapsuleCollider>(entity));
       }
       // ... rest
   }
   ```

4. **Tests & Exemples** (1 jour)
   - [ ] Test unitaire SphereCollider
   - [ ] Test unitaire CapsuleCollider
   - [ ] Exemple bouncy balls (spheres)
   - [ ] Exemple capsule character controller (basic)
   - [ ] Validation collision sphere-box, capsule-box

**Livrables v0.3 :**
- ✅ API Forces/Impulses complète
- ✅ API Velocity control
- ✅ API Kinematic movement
- ✅ RigidBody avec toutes propriétés physiques
- ✅ SphereCollider & CapsuleCollider
- ✅ Tests unitaires complets
- ✅ Exemples mis à jour
- ✅ Documentation à jour

---

### 🟡 v0.4: Collider Expansion (Priorité MOYENNE)

**Objectif :** Supporter formes complexes
**Durée estimée :** 3-4 semaines
**Effort :** ~120-160 heures

#### Phase 4.1: MeshCollider (1-2 semaines)

**Tasks :**

1. **Mesh Collider Component** (2-3 jours)
   ```cpp
   // À créer : src/plugin/physics/src/component/MeshCollider.hpp
   namespace Physics::Component {
       struct MeshCollider {
           std::vector<glm::vec3> vertices;
           std::vector<uint32_t> indices;  // Triangles : groups of 3
           bool convex = false;  // false = triangle mesh, true = convex hull

           // Cache (optionnel, pour performance)
           mutable std::shared_ptr<JPH::ShapeSettings> cachedShape = nullptr;

           // Helpers
           static MeshCollider FromMesh(const Mesh &mesh);
           static MeshCollider ConvexFromMesh(const Mesh &mesh);
       };
   }
   ```

2. **Builder Jolt** (3-4 jours)
   ```cpp
   // À ajouter dans JoltConversions.hpp
   JPH::ShapeSettings* CreateJoltShape(const MeshCollider &collider) {
       // Cache check
       if (collider.cachedShape) {
           return collider.cachedShape.get();
       }

       if (collider.convex) {
           // Convex hull
           JPH::Array<JPH::Vec3> points;
           points.reserve(collider.vertices.size());
           for (const auto &v : collider.vertices) {
               points.push_back(ToJoltVec3(v));
           }

           auto shape = new JPH::ConvexHullShapeSettings(points);
           collider.cachedShape = std::shared_ptr<JPH::ShapeSettings>(shape);
           return shape;
       } else {
           // Triangle mesh
           JPH::TriangleList triangles;
           triangles.reserve(collider.indices.size() / 3);

           for (size_t i = 0; i < collider.indices.size(); i += 3) {
               JPH::Triangle tri;
               tri.mV[0] = ToJoltVec3(collider.vertices[collider.indices[i]]);
               tri.mV[1] = ToJoltVec3(collider.vertices[collider.indices[i+1]]);
               tri.mV[2] = ToJoltVec3(collider.vertices[collider.indices[i+2]]);
               triangles.push_back(tri);
           }

           auto shape = new JPH::MeshShapeSettings(triangles);
           collider.cachedShape = std::shared_ptr<JPH::ShapeSettings>(shape);
           return shape;
       }
   }
   ```

3. **Integration & Tests** (2-3 jours)
   - [ ] Test convex hull simple (cube, sphere approximation)
   - [ ] Test triangle mesh complexe (terrain, statue)
   - [ ] Performance test (1000+ triangles)
   - [ ] Exemple terrain simple
   - [ ] Validation collision mesh-sphere

4. **Outils Authoring** (2-3 jours)
   ```cpp
   // À créer : src/plugin/physics/src/utils/MeshColliderUtils.hpp
   namespace Physics::Utils {
       // Simplification mesh
       MeshCollider SimplifyMesh(const MeshCollider &original, float tolerance);

       // Convex decomposition (VHACD ou custom)
       std::vector<MeshCollider> DecomposeConvex(const MeshCollider &concave);

       // Validation
       bool IsConvex(const MeshCollider &collider);
       bool HasDegenerateTriangles(const MeshCollider &collider);

       // Stats
       struct MeshColliderStats {
           size_t vertexCount;
           size_t triangleCount;
           glm::vec3 bounds;
           float volume;
       };
       MeshColliderStats GetStats(const MeshCollider &collider);
   }
   ```

#### Phase 4.2: Compound Colliders (1 semaine)

**Tasks :**

1. **Component** (2-3 jours)
   ```cpp
   // À créer : src/plugin/physics/src/component/CompoundCollider.hpp
   namespace Physics::Component {
       struct CompoundCollider {
           struct Child {
               entt::entity colliderEntity;  // Entity avec BoxCollider, SphereCollider, etc.
               glm::vec3 offset = glm::vec3(0.0f);
               glm::quat rotation = glm::quat(1, 0, 0, 0);
               glm::vec3 scale = glm::vec3(1.0f);
           };

           std::vector<Child> children;
           bool mutable_ = false;  // true = MutableCompound (modifiable runtime)
       };
   }
   ```

2. **Builder Récursif** (2-3 jours)
   ```cpp
   JPH::ShapeSettings* CreateCompoundShape(entt::registry &registry,
                                           const CompoundCollider &collider) {
       if (collider.mutable_) {
           auto compound = new JPH::MutableCompoundShapeSettings();

           for (const auto &child : collider.children) {
               JPH::ShapeSettings *childShape = GetColliderShape(registry, child.colliderEntity);
               if (childShape) {
                   compound->AddShape(
                       ToJoltVec3(child.offset),
                       ToJoltQuat(child.rotation),
                       childShape
                   );
               }
           }

           return compound;
       } else {
           auto compound = new JPH::StaticCompoundShapeSettings();
           // Same logic
           return compound;
       }
   }
   ```

3. **Tests** (1-2 jours)
   - [ ] Test compound simple (2-3 boxes)
   - [ ] Test compound complexe (10+ shapes)
   - [ ] Test mutable compound (modification runtime)
   - [ ] Exemple : voiture (chassis + roues en compound)

#### Phase 4.3: HeightField (Terrain) (1 semaine)

**Tasks :**

1. **Component** (2 jours)
   ```cpp
   // À créer : src/plugin/physics/src/component/HeightFieldCollider.hpp
   namespace Physics::Component {
       struct HeightFieldCollider {
           std::vector<float> heightSamples;  // Row-major order
           uint32_t sampleCountX = 0;
           uint32_t sampleCountZ = 0;
           glm::vec3 scale = glm::vec3(1.0f);  // Horizontal & vertical scale
           glm::vec3 offset = glm::vec3(0.0f);

           // Helpers
           float GetHeight(uint32_t x, uint32_t z) const;
           void SetHeight(uint32_t x, uint32_t z, float height);

           static HeightFieldCollider FromHeightmap(const Image &heightmap, float heightScale);
       };
   }
   ```

2. **Builder** (2 jours)
   ```cpp
   JPH::ShapeSettings* CreateJoltShape(const HeightFieldCollider &collider) {
       auto shape = new JPH::HeightFieldShapeSettings(
           collider.heightSamples.data(),
           JPH::Vec3(collider.offset.x, collider.offset.y, collider.offset.z),
           JPH::Vec3(collider.scale.x, collider.scale.y, collider.scale.z),
           collider.sampleCountX
       );

       return shape;
   }
   ```

3. **Tests & Exemple** (2-3 jours)
   - [ ] Test terrain flat
   - [ ] Test terrain avec pentes
   - [ ] Exemple génération procédurale (Perlin noise)
   - [ ] Exemple heightmap loading (image → heightfield)
   - [ ] Performance test (256x256, 512x512)

**Livrables v0.4 :**
- ✅ MeshCollider (convex & triangle mesh)
- ✅ CompoundCollider (static & mutable)
- ✅ HeightFieldCollider (terrain)
- ✅ Outils authoring (simplification, decomposition)
- ✅ Tests & exemples complets

---

### 🟢 v0.5: Queries & Events (Priorité MOYENNE-BASSE)

**Objectif :** Collision detection & callbacks
**Durée estimée :** 2-3 semaines
**Effort :** ~80-120 heures

#### Phase 5.1: Raycast & Shape Cast (1 semaine)

**Tasks :**

1. **Raycast API** (2-3 jours)
   ```cpp
   // À créer : src/plugin/physics/src/api/QueryAPI.hpp
   namespace Physics {
       struct RayCastResult {
           bool hit = false;
           entt::entity entity = entt::null;
           glm::vec3 position = glm::vec3(0.0f);
           glm::vec3 normal = glm::vec3(0.0f);
           float fraction = 0.0f;  // [0, 1]
       };

       RayCastResult CastRay(Core &core,
                             const glm::vec3 &origin,
                             const glm::vec3 &direction,
                             float maxDistance,
                             uint32_t layerMask = 0xFFFFFFFF);

       std::vector<RayCastResult> CastRayAll(Core &core,
                                             const glm::vec3 &origin,
                                             const glm::vec3 &direction,
                                             float maxDistance,
                                             uint32_t layerMask = 0xFFFFFFFF);
   }
   ```

2. **Shape Cast API** (2-3 jours)
   ```cpp
   struct ShapeCastResult {
       bool hit = false;
       entt::entity entity = entt::null;
       glm::vec3 contactPoint = glm::vec3(0.0f);
       glm::vec3 contactNormal = glm::vec3(0.0f);
       float fraction = 0.0f;
   };

   ShapeCastResult CastSphere(Core &core, /* ... */);
   ShapeCastResult CastBox(Core &core, /* ... */);
   ShapeCastResult CastCapsule(Core &core, /* ... */);
   ```

3. **Tests** (1-2 jours)
   - [ ] Raycast hit/miss
   - [ ] RaycastAll multiple hits
   - [ ] ShapeCast sphere
   - [ ] Performance (10000 raycasts)

#### Phase 5.2: Overlap Queries (3-4 jours)

**Tasks :**

1. **API** (2 jours)
   ```cpp
   std::vector<entt::entity> OverlapSphere(Core &core, const glm::vec3 &center, float radius, uint32_t layerMask);
   std::vector<entt::entity> OverlapBox(Core &core, const glm::vec3 &center, const glm::vec3 &halfExtents, const glm::quat &rotation, uint32_t layerMask);
   std::vector<entt::entity> OverlapAABB(Core &core, const glm::vec3 &min, const glm::vec3 &max, uint32_t layerMask);
   ```

2. **Tests** (1-2 jours)
   - [ ] Overlap detection
   - [ ] Layer filtering
   - [ ] Exemple trigger zone

#### Phase 5.3: Collision Events (1 semaine)

**Tasks :**

1. **ContactListener Implementation** (3-4 jours)
   ```cpp
   // À créer : src/plugin/physics/src/system/ContactListener.hpp
   class EngineContactListener : public JPH::ContactListener {
       entt::registry *registry;

   public:
       void OnContactAdded(const JPH::Body &body1, const JPH::Body &body2,
                           const JPH::ContactManifold &manifold,
                           JPH::ContactSettings &settings) override {
           // Convertir BodyID → entt::entity via UserData
           entt::entity entity1 = GetEntityFromBody(body1);
           entt::entity entity2 = GetEntityFromBody(body2);

           // Déterminer si sensor
           bool isSensor1 = registry->all_of<RigidBody>(entity1) &&
                           registry->get<RigidBody>(entity1).isSensor;
           bool isSensor2 = registry->all_of<RigidBody>(entity2) &&
                           registry->get<RigidBody>(entity2).isSensor;

           if (isSensor1 || isSensor2) {
               // Trigger event
               registry->emplace<TriggerEnterEvent>(entity1, entity1, entity2);
           } else {
               // Collision event
               glm::vec3 contactPoint = FromJoltVec3(manifold.mWorldSpaceContactPointsOn1[0]);
               glm::vec3 normal = FromJoltVec3(manifold.mWorldSpaceNormal);

               registry->emplace<CollisionEnterEvent>(entity1, entity1, entity2, contactPoint, normal);
           }
       }

       // ... OnContactPersisted, OnContactRemoved
   };
   ```

2. **Event Components** (1 jour)
   ```cpp
   // À créer : src/plugin/physics/src/event/PhysicsEvents.hpp
   namespace Physics {
       struct CollisionEnterEvent {
           entt::entity self;
           entt::entity other;
           glm::vec3 contactPoint;
           glm::vec3 contactNormal;
           float penetrationDepth;
       };

       struct CollisionStayEvent { /* ... */ };
       struct CollisionExitEvent { /* ... */ };

       struct TriggerEnterEvent {
           entt::entity sensor;
           entt::entity other;
       };

       struct TriggerStayEvent { /* ... */ };
       struct TriggerExitEvent { /* ... */ };
   }
   ```

3. **Tests & Exemples** (2 jours)
   - [ ] Test collision enter/exit
   - [ ] Test trigger enter/exit
   - [ ] Exemple trigger zone (activate on enter)
   - [ ] Exemple collision damage system

#### Phase 5.4: Collision Layers (2-3 jours)

**Tasks :**

1. **Layer System** (1-2 jours)
   ```cpp
   // À créer : src/plugin/physics/src/resource/CollisionLayers.hpp
   namespace Physics {
       enum class CollisionLayer : uint16_t {
           Static = 0,
           Dynamic = 1,
           Kinematic = 2,
           Player = 3,
           Enemy = 4,
           Projectile = 5,
           Trigger = 6,
           Debris = 7,
           Custom1 = 8,
           // ... up to 16 or 32
       };

       void SetLayerCollision(CollisionLayer layerA, CollisionLayer layerB, bool collide);
       bool GetLayerCollision(CollisionLayer layerA, CollisionLayer layerB);
   }
   ```

2. **Tests** (1 jour)
   - [ ] Layer filtering tests
   - [ ] Exemple projectile (collides avec enemy, pas avec player)

**Livrables v0.5 :**
- ✅ Raycast & ShapeCast complets
- ✅ Overlap queries
- ✅ Collision & Trigger events
- ✅ Collision layers & filtering
- ✅ Tests & exemples gameplay

---

### 🔵 v0.6: Constraints & Joints (Priorité BASSE)

**Objectif :** Système contraintes complet
**Durée estimée :** 4-5 semaines
**Effort :** ~160-200 heures

#### Phase 6.1: Infrastructure Constraints (1 semaine)

**Tasks :**

1. **Base Constraint Component** (2-3 jours)
   ```cpp
   // À créer : src/plugin/physics/src/component/Constraint.hpp
   namespace Physics::Component {
       struct Constraint {
           entt::entity bodyA = entt::null;
           entt::entity bodyB = entt::null;  // entt::null = world
           bool collideConnected = false;

           // Breakable
           float maxForce = FLT_MAX;
           float maxTorque = FLT_MAX;

           // State
           bool broken = false;
       };

       struct ConstraintInternal {
           JPH::Constraint *constraint = nullptr;  // Owned by PhysicsSystem
       };
   }
   ```

2. **Constraint System** (2-3 jours)
   ```cpp
   // À créer : src/plugin/physics/src/system/ConstraintSystem.hpp
   namespace Physics::System {
       void InitConstraintSystem(Core &core);

       // Lifecycle hooks (generic for all constraint types)
       void OnConstraintConstruct(entt::registry &registry, entt::entity entity);
       void OnConstraintDestroy(entt::registry &registry, entt:entity entity);

       // Update (check breakable)
       void UpdateConstraints(Core &core);
   }
   ```

3. **Breakable Logic** (1-2 jours)
   ```cpp
   void UpdateConstraints(Core &core) {
       auto &registry = core.GetRegistry();
       auto view = registry.view<Constraint, ConstraintInternal>();

       for (auto entity : view) {
           auto &constraint = view.get<Constraint>(entity);
           auto &internal = view.get<ConstraintInternal>(entity);

           if (constraint.broken) continue;

           // Check forces
           float appliedForce = GetConstraintForce(internal.constraint);
           float appliedTorque = GetConstraintTorque(internal.constraint);

           if (appliedForce > constraint.maxForce || appliedTorque > constraint.maxTorque) {
               // Break constraint
               constraint.broken = true;

               // Emit event
               registry.emplace<ConstraintBreakEvent>(entity, entity,
                                                      constraint.bodyA, constraint.bodyB);

               // Destroy internal
               registry.remove<ConstraintInternal>(entity);
           }
       }
   }
   ```

#### Phase 6.2: Hinge & Slider (1 semaine)

**Tasks :**

1. **HingeConstraint** (2-3 jours)
   ```cpp
   // À créer : src/plugin/physics/src/component/HingeConstraint.hpp
   struct HingeConstraint : Constraint {
       glm::vec3 anchorA = glm::vec3(0.0f);
       glm::vec3 anchorB = glm::vec3(0.0f);
       glm::vec3 axisA = glm::vec3(0, 1, 0);
       glm::vec3 axisB = glm::vec3(0, 1, 0);

       // Limits
       bool enableLimits = false;
       float minAngle = -glm::pi<float>();
       float maxAngle = glm::pi<float>();

       // Motor
       bool enableMotor = false;
       float motorTargetVelocity = 0.0f;
       float motorMaxTorque = 0.0f;
   };
   ```

2. **SliderConstraint** (2-3 jours)
   ```cpp
   struct SliderConstraint : Constraint {
       glm::vec3 anchorA = glm::vec3(0.0f);
       glm::vec3 anchorB = glm::vec3(0.0f);
       glm::vec3 axisA = glm::vec3(1, 0, 0);
       glm::vec3 axisB = glm::vec3(1, 0, 0);

       bool enableLimits = false;
       float minDistance = -FLT_MAX;
       float maxDistance = FLT_MAX;

       bool enableMotor = false;
       float motorTargetVelocity = 0.0f;
       float motorMaxForce = 0.0f;
   };
   ```

3. **Tests** (1-2 jours)
   - [ ] Hinge door
   - [ ] Slider piston
   - [ ] Motor tests
   - [ ] Breakable tests

#### Phase 6.3: Fixed, Distance, Cone (1 semaine)

Similar pattern for:
- FixedConstraint (soudure)
- DistanceConstraint (rope/spring)
- ConeConstraint (cone limit)

#### Phase 6.4: Advanced Constraints (1-2 semaines)

- SwingTwistConstraint (épaule)
- SixDOFConstraint (6 degrees of freedom)
- PathConstraint (spline)
- GearConstraint
- RackAndPinionConstraint
- PulleyConstraint

**Livrables v0.6 :**
- ✅ Infrastructure constraints complète
- ✅ Hinge, Slider, Fixed, Distance, Cone
- ✅ SwingTwist, SixDOF
- ✅ Motors support
- ✅ Breakable constraints
- ✅ Tests & exemples (ragdoll, door, piston, rope)

---

### 🟣 v0.7: Advanced Dynamics (Priorité BASSE)

**Objectif :** Features avancées
**Durée estimée :** 3-4 semaines

#### Inclut :
- Character Controllers (rigid & virtual)
- Ragdolls helpers
- Materials system (presets)
- Debug visualization
- Serialization hooks

### 🟤 v0.8+: Ecosystem (Priorité BASSE)

**Objectif :** Extensions & outils
**Durée estimée :** 4-6 semaines

#### Inclut :
- Vehicle system
- Soft bodies
- Water buoyancy
- Performance profiling tools
- Editor integration

---

## 🎨 API Design Patterns

### Inspiration des Engines Populaires

#### 1. Unity Physics - Points forts à adopter

```cpp
// ✅ EngineSquared equivalent: Simple & intuitive API
auto &rb = entity.GetComponent<Physics::Component::RigidBody>(core);
Physics::Resource::AddForce(core, entity, glm::vec3(0, 10, 0));
Physics::Resource::SetLinearVelocity(core, entity, glm::vec3(0, 0, 5));

// Raycast simple
if (auto hit = Physics::Resource::CastRay(core, origin, direction, 100.0f); hit.hit) {
    Logger::Info("Hit entity: {}", entt::to_integral(hit.entity));
}

// Collision callbacks via ECS events
core.GetRegistry().on_construct<Physics::CollisionEnterEvent>()
    .connect<&OnCollisionEnter>();
```

#### 2. Best Practices

**✅ DO: Factory Methods (Component)**
```cpp
namespace Physics::Component {
    auto staticBody = RigidBody::CreateStatic();
    auto dynamicBody = RigidBody::CreateDynamic(mass);
}
```

**✅ DO: Free Functions dans Resource pour Actions**
```cpp
namespace Physics::Resource {
    // ForceApplicator.hpp - Responsabilité: Application de forces
    void AddForce(Engine::Core &core, Engine::Entity entity, const glm::vec3 &force);
    void AddImpulse(Engine::Core &core, Engine::Entity entity, const glm::vec3 &impulse);
    
    // VelocityController.hpp - Responsabilité: Contrôle de vélocité
    void SetLinearVelocity(Engine::Core &core, Engine::Entity entity, const glm::vec3 &velocity);
    glm::vec3 GetLinearVelocity(Engine::Core &core, Engine::Entity entity);
    
    // KinematicMover.hpp - Responsabilité: Mouvement cinématique
    void MoveKinematic(Engine::Core &core, Engine::Entity entity, ...);
}
// Usage:
Physics::Resource::AddForce(core, entity, force);
Physics::Resource::SetLinearVelocity(core, entity, velocity);
```

**✅ DO: ECS Events pour Callbacks**
```cpp
namespace Physics {
    struct CollisionEnterEvent {
        Engine::Entity self;
        Engine::Entity other;
    };
}
void OnCollisionEnter(const Physics::CollisionEnterEvent &event) {
    // Handle collision
}
core.GetRegistry().on_construct<CollisionEnterEvent>().connect<&OnCollisionEnter>();
```

**✅ DO: Respecter l'Architecture Plugin Standard**
```
src/plugin/physics/src/
├── component/        # Composants ECS (RigidBody, Colliders...)
├── system/          # Systèmes ECS (RigidBodySystem, SyncTransform...)
├── resource/        # ✅ Ressources & fonctions utilitaires
│   ├── PhysicsManager.hpp        # Gestion du système Jolt
│   ├── JoltConversions.hpp       # Conversions glm ↔ Jolt
│   ├── ForceApplicator.hpp       # Application forces/impulses
│   ├── VelocityController.hpp    # Contrôle vélocité
│   ├── KinematicMover.hpp        # Mouvement cinématique
│   └── CollisionQuery.hpp        # (future) Raycasts, overlaps...
├── plugin/          # Plugin principal (PhysicsPlugin)
└── exception/       # Exceptions

# ✅ Chaque fichier a une RESPONSABILITÉ CLAIRE (pas de fourre-tout)
```

**❌ DON'T: Créer un dossier utils/ fourre-tout**
```cpp
// ❌ NE PAS FAIRE - utils/ est un anti-pattern
src/plugin/physics/src/utils/Utils.hpp  // ❌ Trop générique, pas de sémantique
```

**❌ DON'T: Exposer Types Jolt**
```cpp
// ❌ NE PAS FAIRE
namespace Physics::Component {
    struct RigidBody {
        JPH::Body *body;  // ❌ Type Jolt exposé
    };
}
```

**❌ DON'T: Utiliser entt::entity directement (préférer Engine::Entity)**
```cpp
// ⚠️ ÉVITER (pour cohérence avec le reste de l'engine)
void AddForce(Core &core, entt::entity entity, ...);

// ✅ PRÉFÉRER
void AddForce(Engine::Core &core, Engine::Entity entity, ...);

---

## 📚 Références

### Documentation
- **Jolt Physics:** https://github.com/jrouwe/JoltPhysics
- **Jolt Docs:** https://jrouwe.github.io/JoltPhysics/
- **GDC Talk:** https://gdcvault.com/play/1027560/Architecting-Jolt-Physics-for-Horizon

### Intégrations Autres Engines
- **Godot Jolt:** https://github.com/godot-jolt/godot-jolt
- **Unreal Jolt:** https://github.com/Yadhu-S/UnrealJolt

### Documents Internes
- `RIGIDBODY_USAGE.md` - Guide utilisateur
- `PHYSICS_COMPARISON.md` - Analyse ancien vs nouveau
- `ARCHITECTURE_FUTURE_COMPATIBILITY.md` - Validation architecture
- PR #380: https://github.com/EngineSquared/EngineSquared/pull/380

---

## ✅ Next Steps Immédiats

### 1. Validation (1-2 jours)
- [ ] Review roadmap avec équipe
- [ ] Prioriser features selon besoins projet
- [ ] Ajuster estimations
- [ ] Assigner responsabilités

### 2. Infrastructure (2-3 jours)
- [ ] Créer branches milestones
- [ ] Setup CI/CD
- [ ] Templates issues/PRs

### 3. Démarrer v0.3 (immédiat)
- [ ] Branch `feature/physics-v0.3-forces-api`
- [ ] Implémenter AddForce/AddImpulse
- [ ] Tests unitaires
- [ ] Documentation

---

## 📝 Conclusion

**État v0.2 (Actuel):**
- ✅ Architecture solide et extensible
- ✅ Encapsulation Jolt complète
- ✅ Base RigidBody fonctionnelle

**Objectif v1.0:**
- 95%+ features Jolt supportées
- API simple style Unity/Godot
- Documentation complète

**Timeline Estimée:** 4-6 mois pour feature-complete

**Priorités:**
1. v0.3: Forces/Properties/Sphere/Capsule (2-3 semaines) - **CRITIQUE**
2. v0.4: Mesh/Compound/HeightField (3-4 semaines) - **HAUTE**
3. v0.5: Queries/Events (2-3 semaines) - **HAUTE**
4. v0.6: Constraints (4-5 semaines) - **MOYENNE**
5. v0.7-0.8: Advanced (7-10 semaines) - **BASSE**

---

**Document maintenu par l'équipe EngineSquared Physics**
**Version 2.0 - Novembre 2025**
