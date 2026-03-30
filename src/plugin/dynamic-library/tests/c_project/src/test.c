#include <stdint.h>
#include <stdio.h>

typedef struct MetaCore MetaCore;

struct MetaCore {
    void *context;
    uint32_t (*GetComponentId)(MetaCore *, const char *);
    void *(*CreateView)();
    void (*DestroyView)(void *);
    void (*AggregateComponentToView)(MetaCore *, void *, uint32_t);
    void (*ViewForEach)(MetaCore *, void *, void *, void (*)(uint32_t, void *));
    void *(*GetEntityComponentFromView)(MetaCore *, uint32_t, uint32_t);
    void (*AddSystem)(MetaCore *, void (*)(MetaCore *));
};

typedef struct {
    float x;
    float y;
} CPosition;

#if defined(_WIN32)
#    define DYNAMIC_TEST_EXPORT __declspec(dllexport)
#else
#    define DYNAMIC_TEST_EXPORT
#endif

void viewForEachCallback(uint32_t entityId, void *userData)
{
    MetaCore *core = (MetaCore *) (userData);
    printf("Entity ID: %u, User Data: %p\n", entityId, core);
    uint32_t component_id = core->GetComponentId(core, "CPosition");
    CPosition *position = (CPosition *) (core->GetEntityComponentFromView(core, component_id, entityId));
    printf("CPosition: x=%f, y=%f\n", position->x, position->y);

    if (position->x == 69.0f)
    {
        position->x = 0.0f;
    }
    else if (position->x == 67.0f)
    {
        position->x = 2.0f;
    }
    if (position->y == 42.0f)
    {
        position->y = 1.0f;
    }
    else if (position->y == 123456789.0f)
    {
        position->y = 3.0f;
    }
}

DYNAMIC_TEST_EXPORT void system_func(MetaCore *core)
{
    printf("------------------------\n");
    printf("Hello from C!\n");

    uint32_t component_id = core->GetComponentId(core, "CPosition");
    if (component_id == 0)
    {
        printf("Component 'CPosition' not found!\n");
        return;
    }
    printf("Component ID for CPosition: %u\n", component_id);

    void *view = core->CreateView();

    core->AggregateComponentToView(core, view, component_id);

    core->ViewForEach(core, view, core, &viewForEachCallback);

    core->DestroyView(view);
}

DYNAMIC_TEST_EXPORT void plugin_bind(MetaCore *core) { core->AddSystem(core, &system_func); }
