#ifndef ANIMATION_H
#define ANIMATION_H

#include "PCH.h"
#include "Mesh.h"

// ----------------------------
// Inline Assimp → GLM helpers
// ----------------------------
inline glm::vec3 ai_to_glm_vec(const aiVector3D& v)
{
    return { v.x, v.y, v.z };
}

inline glm::quat ai_to_glm_quat(const aiQuaternion& q)
{
    return { q.w, q.x, q.y, q.z };
}

inline glm::mat4 ai_to_glm_mat4(const aiMatrix4x4& _from)
{
    glm::mat4 _to;
    _to[0][0] = _from.a1; _to[1][0] = _from.a2; _to[2][0] = _from.a3; _to[3][0] = _from.a4;
    _to[0][1] = _from.b1; _to[1][1] = _from.b2; _to[2][1] = _from.b3; _to[3][1] = _from.b4;
    _to[0][2] = _from.c1; _to[1][2] = _from.c2; _to[2][2] = _from.c3; _to[3][2] = _from.c4;
    _to[0][3] = _from.d1; _to[1][3] = _from.d2; _to[2][3] = _from.d3; _to[3][3] = _from.d4;
    return _to;
}


// ----------------------------
// Keyframe data structs
// ----------------------------
struct KeyPosition
{
    glm::vec3 position{};
    float time_stamp{};
};

struct KeyRotation
{
    glm::quat orientation{};
    float time_stamp{};
};

struct KeyScale
{
    glm::vec3 scale{};
    float time_stamp{};
};

// ----------------------------
// Bone: stores keyframes + interpolation logic
// ----------------------------
class Bone {
public:
    int id;
    std::string name;

    Bone(const std::string& _name, int _id, const aiNodeAnim* _ai_node_anim);
    Bone() = default;
    ~Bone() = default;

    void update(float _animation_time);

    glm::mat4 get_local_transform() const { return m_local_transform; }
private:
    std::vector<KeyPosition> m_positions;
    std::vector<KeyRotation> m_rotations;
    std::vector<KeyScale>    m_scales;

    int m_num_positions{};
    int m_num_rotations{};
    int m_num_scalings{};

    glm::mat4 m_local_transform{1.0f};

    int get_position_index(float animation_time) const;
    int get_rotation_index(float animation_time) const;
    int get_scale_index   (float animation_time) const;

    float get_scale_factor(float last_time, float next_time, float animation_time) const;

    glm::mat4 interpolate_position(float _animation_time) const;
    glm::mat4 interpolate_rotation(float _animation_time) const;
    glm::mat4 interpolate_scaling (float _animation_time) const;
};

// ----------------------------
// Animation container (stub)
// ----------------------------
struct AssimpNodeData
{
    glm::mat4   transformation;
    std::string name;

    int children_count;
    std::vector<AssimpNodeData> children;
};

class Animation
{
public:

    Animation(const std::string& _animation_path, SkeletonMesh* _skeleton_mesh)
    {
        Assimp::Importer _importer;
        const aiScene* _scene = _importer.ReadFile(_animation_path, aiProcess_Triangulate);

        assert(_scene && _scene->mRootNode);

        unsigned int numAnimations = _scene->mNumAnimations;
        std::cout << "Number of animations: " << numAnimations << "\n";

        for (unsigned int i = 0; i < numAnimations; i++)
        {
            aiAnimation* anim = _scene->mAnimations[i];
            std::cout << "Animation " << i << ": " << anim->mName.C_Str()
                      << " duration: " << anim->mDuration
                      << " ticks per second: " << anim->mTicksPerSecond << "\n";
        }

        auto _animation = _scene->mAnimations[0];

        m_duration         = _animation->mDuration;
        m_ticks_per_second = _animation->mTicksPerSecond;

        read_hierarchy_data(m_root_node, _scene->mRootNode);
        read_missing_bones (_animation, *_skeleton_mesh);
    }

    Animation()  = default;
    ~Animation() = default;

    Bone* find_bone(const std::string& _name)
    {
        auto _iter = std::find_if(m_bones.begin(), m_bones.end(),
            [&](const Bone& _bone){
                return _bone.name == _name;
            }
        );

        if (_iter == m_bones.end())
            return nullptr;

        return &(*_iter);
    }

    float get_ticks_per_second()                             { return m_ticks_per_second; }
    float get_duration()                                     { return m_duration        ; }
    const AssimpNodeData& get_root_node()                    { return m_root_node       ; }
    const std::map<std::string, BoneInfo>& get_bone_id_map() { return m_bone_info_map   ; }

private:

    void read_hierarchy_data(AssimpNodeData& _dest, const aiNode* _src)
    {
        assert(_src);

        _dest.name           = _src->mName.data;
        _dest.transformation = ai_to_glm_mat4(_src->mTransformation);
        _dest.children_count = _src->mNumChildren;

        for (int _i = 0; _i < _src->mNumChildren; _i++)
        {
            AssimpNodeData _new_data;
            read_hierarchy_data(_new_data, _src->mChildren[_i]);
            _dest.children.push_back(_new_data);
        }
    }

    void read_missing_bones(const aiAnimation* _ai_animation, SkeletonMesh& _skeleton_mesh)
    {
        int&  _bone_count    = _skeleton_mesh.bone_count;
        auto& _bone_info_map = _skeleton_mesh.bone_mapping;

        // reading channels (bones engaged in animation and their keyframes)
        for (int _i = 0; _i < _ai_animation->mNumChannels; _i++)
        {
            aiNodeAnim* _channel = _ai_animation->mChannels[_i];

            std::string _bone_name = _channel->mNodeName.data;

            if (_bone_info_map.find(_bone_name) == _bone_info_map.end())
            {
                std::cout << "Bone " << _bone_name << " not found\n";
                _bone_info_map[_bone_name].id = _bone_count;
                _bone_count++;
            }

            Bone _new_bone
            (
                _bone_name,
                _bone_info_map[_bone_name].id,
                _channel
            );

            m_bones.push_back(_new_bone);
        }

        m_bone_info_map = _bone_info_map;
    }

    float m_duration        {};
    int   m_ticks_per_second{};

    AssimpNodeData m_root_node;

    std::vector<Bone> m_bones;
    std::map<std::string, BoneInfo> m_bone_info_map;
};

class Animator
{
public:
    Animator(Animation* _animation)
    {
        m_current_time      = 0.0f;
        m_current_animation = _animation;

        m_final_bone_matrices.reserve(100);

        for (int _i = 0; _i < 100; _i++)
            m_final_bone_matrices.push_back(glm::mat4(1.0f));
    }

    Animator() = default;
    ~Animator() = default;

    std::vector<glm::mat4> get_final_bone_matrices()
    {
        return m_final_bone_matrices;
    }

    void play_animation(Animation* _animation)
    {
        m_current_animation = _animation;
        m_current_time      = 0.0f;
    }

    void update_animation(float _dt)
    {
        m_delta_time = _dt;

        if (m_current_animation)
        {
            m_current_time += m_current_animation->get_ticks_per_second() * _dt;
            m_current_time = fmod(m_current_time, m_current_animation->get_duration());

            calculate_bone_transform(&m_current_animation->get_root_node(), glm::mat4(1.0f));
        }
    }

    void calculate_bone_transform(const AssimpNodeData* _node, glm::mat4 _parent_transform)
    {
        std::string _node_name      = _node->name;
        glm::mat4   _node_transform = _node->transformation;

        Bone* _bone = m_current_animation->find_bone(_node_name);

        if (_bone)
        {
            _bone->update(m_current_time);
            _node_transform = _bone->get_local_transform();
        }

        glm::mat4 _global_transform = _parent_transform * _node_transform;

        auto _bone_info_map = m_current_animation->get_bone_id_map();
        if (_bone_info_map.find(_node_name) != _bone_info_map.end())
        {
            int _index = _bone_info_map[_node_name].id;
            glm::mat4 _offset = _bone_info_map[_node_name].offset_matrix;
            m_final_bone_matrices[_index] = _global_transform * _offset;
        }

        for (int _i = 0; _i < _node->children_count; _i++)
            calculate_bone_transform(&_node->children[_i], _global_transform);
    }

private:
    std::vector<glm::mat4> m_final_bone_matrices;
    Animation* m_current_animation;

    float m_current_time;
    float m_delta_time;
};


#endif
