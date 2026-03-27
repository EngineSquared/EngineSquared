#include <cstdint>
#include <iostream>

struct MetaCore {
    void *context;
    std::uint32_t (*GetComponentId)(MetaCore *, const char *);
    void *(*CreateView)();
    void (*DestroyView)(void *);
    void (*AggregateComponentToView)(MetaCore *, void *, std::uint32_t);
    void (*ViewForEach)(MetaCore *, void *, void *, void (*)(std::uint32_t, void *));
    void *(*GetEntityComponentFromView)(MetaCore *, std::uint32_t, std::uint32_t);
    void (*AddSystem)(MetaCore *, void (*)(MetaCore *));
};

struct CppPosition {
    float x;
    float y;
};

extern "C" {

void viewForEachCallback(std::uint32_t entityId, void *userData)
{
    MetaCore *core = static_cast<MetaCore *>(userData);
    std::cout << "Entity ID: " << entityId << ", User Data: " << reinterpret_cast<uintptr_t>(core) << "\n";
    std::uint32_t component_id = core->GetComponentId(core, "CppPosition");
    CppPosition *position = static_cast<CppPosition *>(core->GetEntityComponentFromView(core, component_id, entityId));
    std::cout << "CppPosition: x=" << position->x << ", y=" << position->y << "\n";

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

void system_func(MetaCore *core)
{
    std::cout << "------------------------\n";
    std::cout << "Hello from C++!\n";

    std::uint32_t component_id = core->GetComponentId(core, "CppPosition");
    if (component_id == 0)
    {
        std::cout << "Component 'CppPosition' not found!\n";
        return;
    }
    std::cout << "Component ID for CppPosition: " << component_id << "\n";

    void *view = core->CreateView();

    core->AggregateComponentToView(core, view, component_id);

    core->ViewForEach(core, view, core, &viewForEachCallback);

    core->DestroyView(view);
}

void plugin_bind(MetaCore *core) { core->AddSystem(core, &system_func); }
}
