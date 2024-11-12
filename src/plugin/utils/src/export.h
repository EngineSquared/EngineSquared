/**************************************************************************
 * VkWrapper v0.0.0
 *
 * VkWrapper is a software package, part of the Engine².
 *
 * This file is part of the VkWrapper project that is under GPL-3.0 License.
 * Copyright © 2024 by @EngineSquared, All rights reserved.
 *
 * VkWrapper is a free software: you can redistribute it and/or modify
 * it under the terms of the GPL-3.0 License as published by the
 * Free Software Foundation. See the GPL-3.0 License for more details.
 *
 * @file export.h
 * @brief Compile-Time exportation of the project path.
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-10-15
 **************************************************************************/

// clang-format off
#ifndef EXPORT_H_
    #define EXPORT_H_

#ifdef __cplusplus
#include <filesystem>

#define PROJECT_SOURCE_DIR std::filesystem::current_path().string() + "/"
#else
#include <stdlib.h>
#include <string.h>

#define PROJECT_SOURCE_DIR getenv("PWD")
#endif

#endif /* !EXPORT_H_ */
// clang-format on
