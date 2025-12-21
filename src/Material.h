#ifndef MATERIAL_H
#define MATERIAL_H

#include <Program.h>
#include <Texture.h>
#include <memory>
#include <vector>

namespace OM3D
{
    enum class ProgramEnum
    {
        Default,
        G_Buffer,
        Point_Light_Shading,
    };

    enum class CullMode
    {
        None,
        Backface_Cull,
        FrontFace_Cull,
    };

    enum class BlendMode
    {
        None,
        Alpha,
        Additive,
    };

    enum class DepthTestMode
    {
        None,
        Standard,
        Reversed,
        Equal,
    };

    class Material
    {
    public:
        Material();

        void set_main_program(std::shared_ptr<Program> prog);
        void set_g_buffer_program(std::shared_ptr<Program> prog);
        void set_blend_mode(BlendMode blend);
        DepthTestMode get_depth_test_mode() const;
        void set_depth_test_mode(DepthTestMode depth);
        void set_double_sided(bool double_sided);
        void set_no_depth_contribution(bool no_depth_contribution);
        void set_cull_mode(CullMode cull_mode);
        void set_program_enum(ProgramEnum program_enum);

        void set_texture(u32 slot, std::shared_ptr<Texture> tex);

        bool is_opaque() const;

        // Uniform will be stored inside the material and reset every time its
        // bound
        void set_stored_uniform(u32 name_hash, UniformValue value);

        // Uniform is set immediately and might get overriden by 'set_uniform'
        // called on OTHER materials

        template <typename... Args>
        void set_uniform(Args &&...args) const
        {
            _main_program->set_uniform(FWD(args)...);
            if (_g_buffer_program)
            {
                _g_buffer_program->set_uniform(FWD(args)...);
            }
        }

        void bind(bool g_buffer_pass = false) const;
        Program* get_program() const;

        static Material textured_pbr_material(bool alpha_test = false);

    private:
        std::shared_ptr<Program> _main_program;
        std::shared_ptr<Program> _g_buffer_program;
        std::vector<std::pair<u32, std::shared_ptr<Texture>>> _textures;
        std::vector<std::pair<u32, UniformValue>> _uniforms;

        BlendMode _blend_mode = BlendMode::None;
        DepthTestMode _depth_test_mode = DepthTestMode::Standard;
        CullMode _cull_mode = CullMode::None;
        ProgramEnum _program = ProgramEnum::Default;

        bool _no_depth_contribution = false;
        bool _double_sided = false;
    };

} // namespace OM3D

#endif // MATERIAL_H
