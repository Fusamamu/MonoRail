#ifndef IMPORTER_H
#define IMPORTER_H

namespace MUG
{
    struct MeshRawData;
}


namespace ASSET
{
    MUG::MeshRawData load_mesh_raw_data(const std::filesystem::path& path, MUG::MeshRawData& _mesh_data);
}

#endif //IMPORTER_H
