#pragma once

namespace OM3D
{

    static const char *debug_mode_items[]{
        "None", "Depth", "Albedo", "Normals", "Roughness", "Metalness"
    };

    enum DebugMode
    {
        DEBUG_NONE = 0,
        DEBUG_DEPTH = 1,
        DEBUG_ALBEDO = 2,
        DEBUG_NORMAL = 3,
        DEBUG_ROUGHNESS = 4,
        DEBUG_METALNESS = 5,
    };

    inline std::string debug_mode_to_string(const DebugMode mode)
    {
        switch (mode)
        {
        case DEBUG_NONE:
            return "DEBUG_NONE";
        case DEBUG_DEPTH:
            return "DEBUG_DEPTH";
        case DEBUG_ALBEDO:
            return "DEBUG_ALBEDO";
        case DEBUG_NORMAL:
            return "DEBUG_NORMAL";
        case DEBUG_ROUGHNESS:
            return "DEBUG_ROUGHNESS";
        case DEBUG_METALNESS:
            return "DEBUG_METALNESS";
        }
        return "";
    }

} // namespace OM3D
