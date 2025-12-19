#include "Material.h"

#include <algorithm>
#include <glad/gl.h>

namespace OM3D
{

    Material::Material()
    {}

    void Material::set_main_program(std::shared_ptr<Program> prog)
    {
        _main_program = std::move(prog);
    }

    void Material::set_g_buffer_program(std::shared_ptr<Program> prog)
    {
        _g_buffer_program = std::move(prog);
    }

    void Material::set_blend_mode(BlendMode blend)
    {
        _blend_mode = blend;
    }

    DepthTestMode Material::get_depth_test_mode() const
    {
        return _depth_test_mode;
    }

    void Material::set_depth_test_mode(DepthTestMode depth)
    {
        _depth_test_mode = depth;
    }

    void Material::set_double_sided(bool double_sided)
    {
        _double_sided = double_sided;
    }

    void Material::set_program_enum(ProgramEnum program_enum)
    {
        _program = program_enum;
    }

    void Material::set_no_depth_contribution(bool no_depth_contribution)
    {
        _no_depth_contribution = no_depth_contribution;
    }

    void Material::set_cull_mode(CullMode cull_mode)
    {
        _cull_mode = cull_mode;
    }


    void Material::set_texture(u32 slot, std::shared_ptr<Texture> tex)
    {
        if (const auto it =
                std::find_if(_textures.begin(), _textures.end(),
                             [&](const auto &t) { return t.first == slot; });
            it != _textures.end())
        {
            it->second = std::move(tex);
        }
        else
        {
            _textures.emplace_back(slot, std::move(tex));
        }
    }

    bool Material::is_opaque() const
    {
        return _blend_mode == BlendMode::None;
    }

    void Material::set_stored_uniform(u32 name_hash, UniformValue value)
    {
        for (auto &[h, v] : _uniforms)
        {
            if (h == name_hash)
            {
                v = value;
                return;
            }
        }
        _uniforms.emplace_back(name_hash, std::move(value));
    }

    void Material::bind(bool g_buffer_pass) const
    {
        switch (_blend_mode)
        {
        case BlendMode::None:
            glDisable(GL_BLEND);

            if (_cull_mode != CullMode::None)
            {
                // Enable back_face culling when the object is not transparent
                glEnable(GL_CULL_FACE);
                if (_cull_mode == CullMode::FrontFace_Cull)
                {
                    glCullFace(GL_FRONT);
                }
                else
                {
                    glCullFace(GL_BACK);
                }
                glFrontFace(GL_CCW);
            } else
            {
                glDisable(GL_CULL_FACE);
            }
            break;

        case BlendMode::Alpha:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Disable back_face culling when the object is transparent
            glDisable(GL_CULL_FACE);
            break;

        case BlendMode::Additive:
            // TODO: idk additive
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ZERO);
            // maybe: glBlendFunc(GL_ONE, GL_ONE);

            // Disable back_face culling when the object is transparent
            // glDisable(GL_CULL_FACE);
            break;
        }

        switch (_depth_test_mode)
        {
        case DepthTestMode::None:
            glDisable(GL_DEPTH_TEST);
            break;

        case DepthTestMode::Equal:
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_EQUAL);
            break;

        case DepthTestMode::Standard:
            glEnable(GL_DEPTH_TEST);
            // We are using standard-Z
            glDepthFunc(GL_GEQUAL);
            break;

        case DepthTestMode::Reversed:
            glEnable(GL_DEPTH_TEST);
            // We are using reverse-Z
            glDepthFunc(GL_LEQUAL);
            break;
        }

        if (_no_depth_contribution)
        {
            glDepthMask(GL_FALSE);
        }
        else
        {
            glDepthMask(GL_TRUE);
        }

        for (const auto &texture : _textures)
        {
            texture.second->bind(texture.first);
        }

        for (const auto &[h, v] : _uniforms)
        {
            if (g_buffer_pass)
            {
                _g_buffer_program->set_uniform(h, v);
            }
            else
            {
                _main_program->set_uniform(h, v);
            }
        }


        if (g_buffer_pass)
        {
            _g_buffer_program->bind();
        }
        else
        {
            _main_program->bind();
        }
    }

    Material Material::textured_pbr_material(bool alpha_test)
    {
        Material material;
        std::vector<std::string> defines;
        if (alpha_test)
        {
            defines.emplace_back("ALPHA_TEST");
        }

        material._main_program =
            Program::from_files("lit.frag", "basic.vert", defines);

        std::vector<std::string> defines_with_g_buffer;
        defines_with_g_buffer.insert(defines_with_g_buffer.end(),
                                     defines.begin(), defines.end());
        defines_with_g_buffer.emplace_back("G_BUFFER_RENDER");
        material._g_buffer_program = Program::from_files(
            "lit.frag", "basic.vert", defines_with_g_buffer);

        material.set_texture(0u, default_white_texture());
        material.set_texture(1u, default_normal_texture());
        material.set_texture(2u, default_metal_rough_texture());
        material.set_texture(3u, default_white_texture());

        return material;
    }

} // namespace OM3D
