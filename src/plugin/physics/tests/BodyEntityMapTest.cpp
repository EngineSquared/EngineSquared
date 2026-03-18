#include <gtest/gtest.h>

#include "resource/BodyEntityMap.hpp"

TEST(PhysicsPlugin, BodyEntityMapAddition)
{
    Physics::Resource::BodyEntityMap map;

    EXPECT_EQ(map.Size(), 0);

    map.Add(Engine::EntityId{1}, JPH::BodyID{10});
    map.Add(Engine::EntityId{2}, JPH::BodyID{20});
    map.Add(Engine::EntityId{3}, JPH::BodyID{30});

    EXPECT_EQ(map.Size(), 3);
}

TEST(PhysicsPlugin, BodyEntityMapRemoval)
{
    Physics::Resource::BodyEntityMap map;

    EXPECT_EQ(map.Size(), 0);

    map.Add(Engine::EntityId{1}, JPH::BodyID{10});
    map.Add(Engine::EntityId{2}, JPH::BodyID{20});
    map.Add(Engine::EntityId{3}, JPH::BodyID{30});

    EXPECT_EQ(map.Size(), 3);

    map.Remove(Engine::EntityId{1});

    EXPECT_EQ(map.Size(), 2);

    map.Remove(JPH::BodyID{20});

    EXPECT_EQ(map.Size(), 1);

    map.Remove(Engine::EntityId{3});

    EXPECT_EQ(map.Size(), 0);
}

TEST(PhysicsPlugin, BodyEntityMapRetrieval)
{
    std::array<std::pair<unsigned int, uint32_t>, 3> entities{
        std::make_pair(1, 10),
        std::make_pair(2, 20),
        std::make_pair(3, 30),
    };

    Physics::Resource::BodyEntityMap map;

    for (auto &[entityId, bodyId] : entities)
    {
        map.Add(Engine::EntityId{entityId}, JPH::BodyID{bodyId});
    }

    for (const auto &[entityId, bodyId] : entities)
    {
        EXPECT_EQ(map.Get(Engine::EntityId{entityId}), JPH::BodyID{bodyId});
        EXPECT_EQ(Engine::EntityId{entityId}, map.Get(JPH::BodyID{bodyId}));
    }
}

TEST(PhysicsPlugin, BodyEntityMapErroneousRetrieval)
{
    Physics::Resource::BodyEntityMap map;

    Engine::EntityId validEntity{1};
    Engine::EntityId invalidEntity{2};

    map.Add(validEntity, JPH::BodyID{10});

    EXPECT_EQ(map.Contains(invalidEntity), false);
    EXPECT_EQ(map.Contains(validEntity), true);
    EXPECT_EQ(map.Size(), 1);
}
