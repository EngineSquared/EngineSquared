#include <gtest/gtest.h>

#include "Registry.hpp"
#include "Entity.hpp"

using namespace ES::Engine;

TEST(Registry, CreateEntity) {
	Registry reg;

	entt::entity raw_entity = reg.CreateEntity();

}

TEST(Registry, Systems) {
	Registry reg;

	int x = 42;

	reg.RegisterSystem([&](Registry & registry) {
		x = 69;
	});

	reg.RegisterSystem([&](Registry& registry) {
		x = 144;
	});

	ASSERT_EQ(x, 42);

	reg.RunSystems();

	ASSERT_EQ(x, 144);
}

TEST(Registry, Resources) {
	Registry reg;

	struct Res {
		int x;
	};

	Res res;
	res.x = 42;

	reg.RegisterSystem([&](Registry& registry) {
		registry.GetResource<Res>().x = 69;
	});

	ASSERT_EQ(res.x, 42);

	reg.RunSystems();

	ASSERT_EQ(res.x, 69);
}