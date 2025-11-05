# Physics Plugin - Issue Dependencies Map

> Document de validation des dépendances entre issues
> Généré le 2025-11-05

---

## 📊 Graphe de Dépendances

```
v0.3 - Core Completeness (CRITICAL)
├─ #001: Forces/Impulses ───────────────┐
├─ #002: Velocity Control ──────────────┤
├─ #003: Kinematic Movement ────────────┤ (Base pour tout)
├─ #004: RigidBody Properties ──────────┤
└─ #005: Sphere/Capsule Colliders ──────┘
                │
                ├──→ v0.4 - Collider Expansion (HIGH)
                │    ├─ #006: MeshCollider
                │    ├─ #007: CompoundCollider
                │    └─ #008: HeightFieldCollider
                │
                ├──→ v0.5 - Collision Queries (HIGH)
                │    ├─ #009: Raycast ──────────────┐
                │    ├─ #010: ShapeCast ────────────┤ (Queries de base)
                │    ├─ #011: Overlap ──────────────┤
                │    └─ #012: Collision Filtering ──┘
                │                │
                │                ├──→ v0.6 - Events & Constraints
                │                │    ├─ #013: Collision Events ──┐
                │                │    ├─ #014: Trigger Events ─────┤
                │                │    ├─ #015: Basic Constraints ──┤
                │                │    ├─ #016: Hinge/Slider ───────┤
                │                │    └─ #017: 6DOF Constraint ────┘
                │                │                │
                │                │                ├──→ v0.7 - Advanced Physics
                │                │                │    ├─ #018: CharacterController (CRITICAL)
                │                │                │    ├─ #019: VehiclePhysics
                │                │                │    ├─ #020: RagdollSystem
                │                │                │    ├─ #021: SoftBody
                │                │                │    └─ #022: Sensors
                │                │                │
                │                │                └──→ v0.8 - Ecosystem & Tools
                │                │                     ├─ #023: Profiler
                │                │                     ├─ #024: Debug Renderer
                │                │                     ├─ #025: Serialization
                │                │                     └─ #026: Editor Integration
```

---

## 🔗 Matrice de Dépendances

### v0.3 - Core Completeness

| Issue | Titre | Dependencies | Related | Follow-up |
|-------|-------|--------------|---------|-----------|
| #001 | Forces/Impulses | None | #002, #003 | #018, #019 |
| #002 | Velocity Control | #001 | #003 | #018, #003 |
| #003 | Kinematic Movement | #001, #002 | #004 | #018 |
| #004 | RigidBody Properties | None | #001-003 | All v0.4+ |
| #005 | Sphere/Capsule | None | #006-008 | #006, #007 |

**Corrections :**
- ✅ #001-005 : Aucune dépendance externe (base)
- ✅ Tous sont des prérequis pour v0.4+

---

### v0.4 - Collider Expansion

| Issue | Titre | Dependencies | Related | Follow-up |
|-------|-------|--------------|---------|-----------|
| #006 | MeshCollider | #001-005 | #007, #008 | #021 |
| #007 | CompoundCollider | #001-005, #006 | #008 | #019, #020 |
| #008 | HeightFieldCollider | #001-005 | #006, #007 | #019 |

**Corrections :**
- ✅ Tous dépendent de v0.3 (core)
- ✅ #007 dépend aussi de #006 (pour compound de meshes)

---

### v0.5 - Collision Queries

| Issue | Titre | Dependencies | Related | Follow-up |
|-------|-------|--------------|---------|-----------|
| #009 | Raycast | #001-005 | #010, #011 | #018, #022 |
| #010 | ShapeCast | #001-005, #009 | #011 | #019 |
| #011 | Overlap | #001-005, #009 | #012 | #022 |
| #012 | Collision Filtering | #001-005 | #013, #014 | #018, #022 |

**Corrections :**
- ✅ Tous dépendent de v0.3 (core)
- ✅ #010 dépend de #009 (raycast est la base)
- ✅ #011 dépend de #009 (overlap utilise raycast internement)

---

### v0.6 - Collision Events & Constraints

| Issue | Titre | Dependencies | Related | Follow-up |
|-------|-------|--------------|---------|-----------|
| #013 | Collision Events | #001-012 | #014 | #018, #019 |
| #014 | Trigger Events | #001-012, #013 | #022 | #022 |
| #015 | Basic Constraints | #001-005 | #016, #017 | #020 |
| #016 | Hinge/Slider | #001-005, #015 | #017 | #019, #020 |
| #017 | 6DOF Constraint | #001-005, #015 | #016 | #020 |

**Corrections :**
- ✅ #013-014 dépendent de tout le core + queries (détection collisions)
- ✅ #015-017 dépendent seulement du core (constraints = feature séparée)
- ✅ #016-017 dépendent de #015 (base constraints)

---

### v0.7 - Advanced Physics

| Issue | Titre | Dependencies | Related | Follow-up |
|-------|-------|--------------|---------|-----------|
| #018 | CharacterController | #001-012 | #009, #012, #019 | #024, #026 |
| #019 | VehiclePhysics | #001-005, #015-017 | #018 | #024, #026 |
| #020 | RagdollSystem | #001-005, #015-017 | #021 | #024, #025 |
| #021 | SoftBody | #001-005, #015 | #020 | #024 |
| #022 | Sensors | #009, #011, #014 | #018 | #026 |

**Corrections importantes :**
- ⚠️ **#018** : Doit dépendre de `#001-012` (tout le core + queries/events)
- ⚠️ **#019** : Doit dépendre de `#001-005, #015-017` (core + constraints pour suspension)
- ⚠️ **#020** : Doit dépendre de `#001-005, #015-017` (core + constraints pour joints)
- ⚠️ **#021** : Doit dépendre de `#001-005, #015` (core + distance constraints)
- ⚠️ **#022** : Doit dépendre de `#009, #011, #014` (raycast, overlap, triggers)

---

### v0.8 - Ecosystem & Tools

| Issue | Titre | Dependencies | Related | Follow-up |
|-------|-------|--------------|---------|-----------|
| #023 | Profiler | None | #024 | None |
| #024 | Debug Renderer | #023 (optional) | #026 | None |
| #025 | Serialization | None | #026 | None |
| #026 | Editor Integration | #024, #025 | #023 | None |

**Corrections :**
- ⚠️ **#023** : Erreur dans Related (`#024` dupliqué) → Corriger
- ✅ #024 : Peut utiliser #023 (afficher profiling dans debug)
- ✅ #025 : Standalone (serialization)
- ✅ #026 : Dépend de #024 (gizmos) + #025 (save/load)

---

## 🔄 Actions de Correction

### Corrections Prioritaires

1. **#018 (CharacterController)**
   - ❌ Actuel: `#001-012 (Core physics + collisions + queries)`
   - ✅ Corriger: `#001-012 (Core + Queries + Events)`

2. **#019 (VehiclePhysics)**
   - ❌ Actuel: Pas clair
   - ✅ Corriger: `#001-005 (Core), #015-017 (Constraints for suspension)`

3. **#020 (RagdollSystem)**
   - ❌ Actuel: Pas clair
   - ✅ Corriger: `#001-005 (Core), #015-017 (Constraints for joints)`

4. **#021 (SoftBody)**
   - ❌ Actuel: Pas clair
   - ✅ Corriger: `#001-005 (Core), #015 (Distance constraints)`

5. **#022 (Sensors)**
   - ❌ Actuel: `#014 (Trigger Events), #009 (Raycast)`
   - ✅ Corriger: `#009 (Raycast), #011 (Overlap), #014 (Triggers)`

6. **#023 (Profiler)**
   - ❌ Actuel: `Related: #024 (Debug Visualization), #024 (Performance Tuning)` (duplication!)
   - ✅ Corriger: `Related: #024 (Debug Visualization)`

---

## 📋 Template Corrigé

### Format Standard

```markdown
**Dependencies:** #XXX (Description courte), #YYY (Description)
**Related Issues:** #AAA (Nom), #BBB (Nom)
**Follow-up Issues:** #CCC (Nom), #DDD (Nom)
```

### Exemples

**Issue avec dépendances complexes:**
```markdown
**Dependencies:** #001-005 (Core Physics), #015-017 (Constraints)
**Related Issues:** #018 (CharacterController), #020 (RagdollSystem)
**Follow-up Issues:** #024 (Debug Visualization), #026 (Editor Integration)
```

**Issue standalone:**
```markdown
**Dependencies:** None (standalone)
**Related Issues:** #024 (Debug Visualization)
**Follow-up Issues:** None
```

---

## ✅ Validation

### Checklist

- [ ] Aucune dépendance circulaire
- [ ] Toutes les dépendances existent
- [ ] Related issues sont bidirectionnels (si A→B alors B→A)
- [ ] Follow-up issues pointent vers features logiques
- [ ] v0.3 n'a aucune dépendance (base)
- [ ] v0.8 peut avoir des dépendances optionnelles

### Tests

```bash
# Vérifier dépendances circulaires
grep -r "Dependencies:" docs/issues/*.md | sort

# Vérifier issues inexistantes
grep -r "#0[0-9][0-9]" docs/issues/*.md | grep -v "^#" | sort -u
```

---

**Document maintenu par l'équipe EngineSquared Physics**  
**Version 1.0 - Novembre 2025**
