#pragma once

#include <unordered_map>
#include <set>

#include "Registry.hpp"
#include "Entity.hpp"

namespace ES::Plugin::Collision::Resource {
	class CollisionStorage {
	public:
		CollisionStorage() = default;
		~CollisionStorage() = default;

		void AddCollisionPair(ES::Engine::Entity::entity_id_type entityA, ES::Engine::Entity::entity_id_type entityB);
		void RemoveCollisionPair(ES::Engine::Entity::entity_id_type entityA, ES::Engine::Entity::entity_id_type entityB);
		void ClearAllCollisionPair();
		void ClearAll();

		bool IsCollidingPair(ES::Engine::Entity::entity_id_type entityA, ES::Engine::Entity::entity_id_type entityB) const;
	
		static void ResetCollision(ES::Engine::Registry& registry)
		{
			registry.GetResource<CollisionStorage>().ClearAllCollisionPair();
		}
	private:
		std::unordered_map<ES::Engine::Entity::entity_id_type, std::set<ES::Engine::Entity::entity_id_type>> _entityCollisionsPairs;
	};
}
