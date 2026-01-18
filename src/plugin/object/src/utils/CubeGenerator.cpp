#include "CubeGenerator.hpp"
#include "BoxGenerator.hpp"
#include "Object.pch.hpp"

namespace Object::Utils {

Component::Mesh GenerateCubeMesh(float size)
{
    return GenerateBoxMesh(size, size, size);
}

} // namespace Object::Utils
