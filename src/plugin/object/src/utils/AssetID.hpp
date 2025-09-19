#pragma once

#include <entt/core/hashed_string.hpp>

namespace Plugin::Object::Utils {

/**
 * AssetID is a type alias for entt::hashed_string, which is used to represent a unique
 * identifier for assets in the engine. It is a hashed string that provides a
 * lightweight and efficient way to identify assets without the need for
 * string comparisons.
 */
using AssetID = entt::hashed_string;

} // namespace Plugin::Object::Utils
