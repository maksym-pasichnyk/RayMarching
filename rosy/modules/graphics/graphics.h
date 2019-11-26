#pragma once

#include <string>
#include <memory>
#include <set>

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "../window/window.h"
#include "../../module.h"

namespace rosy::image {
    struct Image;
}

namespace rosy::window {
    struct Window;
}

enum class Uniform : GLint {};
enum class Attrib : GLint {};

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
        glProgramUniformMatrix4fv(program, GLint(location), 1, false, (float*) &m);
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

namespace rosy::graphics {
    struct {
        struct {
            Attrib position;
            Attrib uv;
        } in_data;
    } location;

    struct VertexData {
        float x, y, u, v;
    };

    struct Graphics {
        struct Shader {
            ShaderProgram vertex;
            ShaderProgram fragment;
            ShaderProgram pipeline;
        } shader;

        friend struct rosy::window::Window;
    public:
        Graphics();
        ~Graphics();
        void draw();
        void clear() noexcept;
        void present() noexcept;
    private:
        VertexData* m_buffer;

        GLuint m_vbo;
        GLuint m_ibo;
        GLuint m_vao;

        void setSize(const int width, const int height) noexcept;
    };
}