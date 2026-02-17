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
