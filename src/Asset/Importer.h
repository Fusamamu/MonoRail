#ifndef IMPORTER_H
#define IMPORTER_H

#include "../PCH.h"
#include "../Core/Mesh.h"
#include "../Core/Vertex.h"

namespace ASSET
{
    MeshRawData load_mesh_raw_data(const std::filesystem::path& path, MeshRawData& _mesh_data);
}

#endif //IMPORTER_H
