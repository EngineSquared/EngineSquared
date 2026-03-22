/**************************************************************************
 * EngineSquared v0.1.1
 *
 * EngineSquared is a software package, part of the Engine² organization.
 *
 * This file is part of the EngineSquared project that is under MIT License.
 * Copyright © 2025-present by @EngineSquared, All rights reserved.
 *
 * EngineSquared is a free software: you can redistribute it and/or modify
 * it under the terms of the MIT License. See the project's LICENSE file for
 * the full license text and details.
 *
 * @file CharacterControllerError.hpp
 * @brief Exception type for CharacterController errors
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-03-22
 **************************************************************************/

#pragma once

#include <stdexcept>

namespace Physics::Exception {

class CharacterControllerError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace Physics::Exception
