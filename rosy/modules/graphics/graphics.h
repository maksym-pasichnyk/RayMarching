#pragma once

#include "shader.h"
#include <vector>

namespace rosy::graphics {
    struct Graphics {
    public:
        Graphics();
        ~Graphics();
        void clear() noexcept;
        void present() noexcept;
        static ShaderProgram newShader(const ShaderType type, const std::initializer_list<const char*> sources);
    };

    ShaderProgram newShader(const ShaderType type, const std::string_view source);
    ShaderProgram newShader(const ShaderType type, const std::initializer_list<const char*> sources);
}