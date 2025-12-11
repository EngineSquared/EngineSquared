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
 * @file ConstraintError.hpp
 * @brief Exception class for constraint-related errors
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-12-04
 **************************************************************************/

#pragma once

#include <stdexcept>

namespace Physics::Exception {

/**
 * @brief Exception thrown when a constraint operation fails
 *
 * This exception is thrown in scenarios such as:
 * - Creating a constraint with invalid body entities
 * - Attempting to constrain a body to itself
 * - Both constraint bodies being static
 * - Invalid constraint parameters (e.g., maxDistance < minDistance)
 * - Jolt physics system not being activated
 */
class ConstraintError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace Physics::Exception
