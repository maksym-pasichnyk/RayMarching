#pragma once

#include <string_view>

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace rosy::graphics {
    enum class Uniform : GLint {};
    enum class Attrib : GLint {};

    enum class ShaderType {
        Fragment = GL_FRAGMENT_SHADER,
        Vertex   = GL_VERTEX_SHADER,
        Compute  = GL_COMPUTE_SHADER,
        Geometry = GL_GEOMETRY_SHADER,
    };

    struct ShaderProgram {
        GLuint program;

        inline Uniform getUniformLocation(const std::string_view name) noexcept {
            return Uniform(glGetUniformLocation(program, name.data()));
        }

        inline Attrib getAttribLocation(const std::string_view name) noexcept {
            return Attrib(glGetAttribLocation(program, name.data()));
        }

        void useProgramStages(const GLbitfield stages, const ShaderProgram shader) noexcept {
            glUseProgramStages(program, stages, shader.program);
        }

        inline void setInt(const Uniform location, const GLint x, const GLint y) noexcept {
            glProgramUniform2i(program, GLint(location), x, y);
        }

        inline void setFloat(const Uniform location, const GLfloat x) noexcept {
            glProgramUniform1f(program, GLint(location), x);
        }

        inline void set(const Uniform location, const glm::vec3 xyz) noexcept {
            glProgramUniform3f(program, GLint(location), xyz.x, xyz.y, xyz.z);
        }

        inline void set(const Uniform location, const glm::mat4 m) noexcept {
            glProgramUniformMatrix4fv(program, GLint(location), 1, false, (float *) &m);
        }

        void setInt(const std::string_view name, const GLint x, const GLint y) noexcept {
            setInt(getUniformLocation(name), x, y);
        }

        void setFloat(const std::string_view name, const GLfloat x) noexcept {
            setFloat(getUniformLocation(name), x);
        }

        void set(const std::string_view name, const glm::vec3 xyz) noexcept {
            set(getUniformLocation(name), xyz);
        }

        void set(const std::string_view name, const glm::mat4 m) noexcept {
            set(getUniformLocation(name), m);
        }
    };
}