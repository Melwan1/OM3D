#include "SceneObject.h"

namespace OM3D
{

    SceneObject::SceneObject(std::shared_ptr<StaticMesh> mesh,
                             std::shared_ptr<Material> material)
        : _mesh(std::move(mesh))
        , _material(std::move(material))
    {}

    void SceneObject::render(const Camera &camera, const Frustum &frustum,
                             const bool backface_culling,
                             PassType pass_type) const
    {
        if (!_material || !_mesh)
        {
            return;
        }

        _material->set_uniform(HASH("model"), transform());
        DepthTestMode original_depth_test_mode =
            _material->get_depth_test_mode();

        if (pass_type != PassType::DEPTH_PREPASS
            && pass_type != PassType::POINT_LIGHT_G_BUFFER
            && pass_type != PassType::MAIN_NO_DEPTH)
        {
            _material->set_depth_test_mode(DepthTestMode::Equal);
        }

        if (backface_culling)
        {
            _material->set_cull_mode(CullMode::Backface_Cull);
        }

        _material->bind(pass_type == PassType::G_BUFFER);
        _material->set_depth_test_mode(original_depth_test_mode);

        _mesh->draw(camera, frustum, scale(), translation());
    }

    const Material &SceneObject::material() const
    {
        DEBUG_ASSERT(_material);
        return *_material;
    }

    void SceneObject::set_transform(const glm::mat4 &tr)
    {
        _transform = tr;
    }

    const glm::mat4 &SceneObject::transform() const
    {
        return _transform;
    }

    void SceneObject::set_scale(const glm::vec3 &scale)
    {
        _scale = scale;
    }

    const glm::vec3 &SceneObject::scale() const
    {
        return _scale;
    }

    void SceneObject::set_translation(const glm::vec3 &translation)
    {
        _translation = translation;
    }

    const glm::vec3 &SceneObject::translation() const
    {
        return _translation;
    }

    void SceneObject::set_rotation(const glm::mat3 &rotation)
    {
        _rotation = rotation;
    }

    const glm::mat3 &SceneObject::rotation() const
    {
        return _rotation;
    }

    const StaticMesh &SceneObject::get_static_mesh() const
    {
        return *_mesh;
    }

    Material *SceneObject::get_material()
    {
        return _material.get();
    }

} // namespace OM3D
