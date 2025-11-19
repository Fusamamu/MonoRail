#include "Animation.h"

Bone::Bone(const std::string& _name, int _id, const aiNodeAnim* _ai_node_anim)
    : name(_name), id(_id)
{
    // Positions
    m_num_positions = _ai_node_anim->mNumPositionKeys;
    m_positions.reserve(m_num_positions);
    for (int i = 0; i < m_num_positions; ++i)
    {
        KeyPosition key{};
        key.position   = ai_to_glm_vec(_ai_node_anim->mPositionKeys[i].mValue);
        key.time_stamp = static_cast<float>(_ai_node_anim->mPositionKeys[i].mTime);
        m_positions.push_back(key);
    }

    // Rotations
    m_num_rotations = _ai_node_anim->mNumRotationKeys;
    m_rotations.reserve(m_num_rotations);
    for (int i = 0; i < m_num_rotations; ++i)
    {
        KeyRotation key{};
        key.orientation = ai_to_glm_quat(_ai_node_anim->mRotationKeys[i].mValue);
        key.time_stamp  = static_cast<float>(_ai_node_anim->mRotationKeys[i].mTime);
        m_rotations.push_back(key);
    }

    // Scales
    m_num_scalings = _ai_node_anim->mNumScalingKeys;
    m_scales.reserve(m_num_scalings);
    for (int i = 0; i < m_num_scalings; ++i)
    {
        KeyScale key{};
        key.scale      = ai_to_glm_vec(_ai_node_anim->mScalingKeys[i].mValue);
        key.time_stamp = static_cast<float>(_ai_node_anim->mScalingKeys[i].mTime);
        m_scales.push_back(key);
    }
}

void Bone::update(float _animation_time)
{
    glm::mat4 translation = interpolate_position(_animation_time);
    glm::mat4 rotation    = interpolate_rotation(_animation_time);
    glm::mat4 scale       = interpolate_scaling (_animation_time);

    m_local_transform     = translation * rotation * scale;
}

int Bone::get_position_index(float animation_time) const
{
    for (int i = 0; i < m_num_positions - 1; ++i)
        if (animation_time < m_positions[i + 1].time_stamp) return i;
    assert(0);
}

int Bone::get_rotation_index(float animation_time) const
{
    for (int i = 0; i < m_num_rotations - 1; ++i)
        if (animation_time < m_rotations[i + 1].time_stamp) return i;
    assert(0);
}

int Bone::get_scale_index(float animation_time) const
{
    for (int i = 0; i < m_num_scalings - 1; ++i)
        if (animation_time < m_scales[i + 1].time_stamp) return i;
    assert(0);
}

float Bone::get_scale_factor(float last_time, float next_time, float animation_time) const
{
    float delta = next_time - last_time;
    if (delta == 0.0f)
        return 0.0f;
    return (animation_time - last_time) / delta;
}

glm::mat4 Bone::interpolate_position(float _animation_time) const
{
    if (m_num_positions == 1)
        return glm::translate(glm::mat4(1.0f), m_positions[0].position);

    int _p0 = get_position_index(_animation_time);
    int _p1 = _p0 + 1;

    float _factor = get_scale_factor(
        m_positions[_p0].time_stamp,
        m_positions[_p1].time_stamp,
        _animation_time
    );

    glm::vec3 _pos = glm::mix(
        m_positions[_p0].position,
        m_positions[_p1].position,
        _factor);

    return glm::translate(glm::mat4(1.0f), _pos);
}

glm::mat4 Bone::interpolate_rotation(float _animation_time) const
{
    if (m_num_rotations == 1)
        return glm::mat4_cast(glm::normalize(m_rotations[0].orientation));

    int _p0 = get_rotation_index(_animation_time);
    int _p1 = _p0 + 1;

    float _factor = get_scale_factor(
        m_rotations[_p0].time_stamp,
        m_rotations[_p1].time_stamp,
        _animation_time
    );

    glm::quat _rot = glm::slerp(
        m_rotations[_p0].orientation,
        m_rotations[_p1].orientation,
        _factor
    );

    return glm::mat4_cast(glm::normalize(_rot));
}

glm::mat4 Bone::interpolate_scaling(float _animation_time) const
{
    if (m_num_scalings == 1)
        return glm::scale(glm::mat4(1.0f), m_scales[0].scale);

    int _p0 = get_scale_index(_animation_time);
    int _p1 = _p0 + 1;

    float _factor = get_scale_factor(
        m_scales[_p0].time_stamp,
        m_scales[_p1].time_stamp,
        _animation_time
    );

    glm::vec3 _scale = glm::mix(
        m_scales[_p0].scale,
        m_scales[_p1].scale,
        _factor
    );

    return glm::scale(glm::mat4(1.0f), _scale);
}

