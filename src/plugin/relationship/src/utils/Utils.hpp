/**************************************************************************
 * Relationship v0.0.0
 *
 * Relationship is a software package, part of the Engine².
 *
 * This file is part of the EngineSquared project that is under GPL-3.0 License.
 * Copyright © 2024 by @EngineSquared, All rights reserved.
 *
 * EngineSquared is a free software: you can redistribute it and/or modify
 * it under the terms of the GPL-3.0 License as published by the
 * Free Software Foundation. See the GPL-3.0 License for more details.
 *
 * @file Utils.hpp
 * @brief utility functions for the Relationship component
 *
 * This file contains the declaration of the utility functions for the Relationship component.
 *
 * @author @miou-zora
 * @version 0.0.0
 * @date 18-02-2024
 **************************************************************************/

#pragma once

#include "component/Relationship.hpp"

namespace ES::Plugin::Relationship::Utils {
auto SetChildOf(ES::Engine::Core &core, ES::Engine::Entity parent, ES::Engine::Entity child) -> void;
auto IsChildOf(ES::Engine::Core &core, ES::Engine::Entity parent, ES::Engine::Entity child) -> bool;
} // namespace ES::Plugin::Relationship::Utils
