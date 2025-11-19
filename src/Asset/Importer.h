#ifndef IMPORTER_H
#define IMPORTER_H

struct MeshRawData;

namespace ASSET
{
    MeshRawData load_mesh_raw_data(const std::filesystem::path& path, MeshRawData& _mesh_data);
}

#endif //IMPORTER_H
