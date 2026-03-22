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
 * @file OBJLoaderError.hpp
 * @brief OBJLoaderError class declaration.
 *
 * This class is used to represent errors that occur while loading OBJ files.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-10-03
 **************************************************************************/

#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

namespace Object {

/**
 * @brief OBJLoaderError is an exception class that should be thrown when an error
 * occurs while manipulating OBJLoader objects.
 *
 * @example "Catching an exception"
 * @code
 * try {
 * } catch (OBJLoaderError &e) {
 *   std::cerr << e.what() << std::endl;
 * }
 * @endcode
 *
 * @example "Throwing an exception"
 * @code
 * throw OBJLoaderError("Failed to do something");
 * @endcode
 */
class OBJLoaderError : public std::exception {
  public:
    explicit OBJLoaderError(const std::string &message) : _msg("OBJLoader error: " + message){};

    const char *what() const throw() override { return this->_msg.c_str(); };

  private:
    std::string _msg;
};

} // namespace Object
