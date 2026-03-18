#include "resource/BodyEntityMap.hpp"

namespace Physics::Resource {

void BodyEntityMap::Add(const Engine::EntityId &entity, const JPH::BodyID &bodyID)
{
    auto aitr = this->_bodyToEntity.insert_or_assign(bodyID, nullptr);
    auto bp = &((*aitr.first).first);
    auto bitr = this->_entityToBody.insert_or_assign(entity, bp).first;
    auto ap = &((*bitr).first);
    (*aitr.first).second = ap;
}
void BodyEntityMap::Remove(const Engine::EntityId &entity)
{
    auto it = this->_entityToBody.find(entity);
    if (it != this->_entityToBody.end())
    {
        this->_bodyToEntity.erase(*(it->second));
        this->_entityToBody.erase(it);
    }
}
void BodyEntityMap::Remove(const JPH::BodyID &bodyID)
{
    auto it = this->_bodyToEntity.find(bodyID);
    if (it != this->_bodyToEntity.end())
    {
        this->_entityToBody.erase(*(it->second));
        this->_bodyToEntity.erase(it);
    }
}
std::uint32_t BodyEntityMap::Size() const { return static_cast<std::uint32_t>(this->_entityToBody.size()); }
const JPH::BodyID &BodyEntityMap::Get(const Engine::EntityId &entity) const { return *this->_entityToBody.at(entity); }
const Engine::EntityId &BodyEntityMap::Get(const JPH::BodyID &bodyID) const { return *this->_bodyToEntity.at(bodyID); }
bool BodyEntityMap::Contains(const Engine::EntityId &entity) const { return this->_entityToBody.contains(entity); }
bool BodyEntityMap::Contains(const JPH::BodyID &bodyID) const { return this->_bodyToEntity.contains(bodyID); }

} // namespace Physics::Resource
