#include "graphics.h"
#include "../../module.h"

#include "../window/window.h"
#include "camera.h"

#include <filesystem>
#include <fstream>

namespace {
    module<rosy::graphics::Camera> m_camera;
    module<rosy::window::Window> m_window;

    void ValidateProgram(GLuint program) {
        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
        if (isLinked == GL_FALSE) {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
            std::cerr << infoLog.data() << std::endl;
            exit(1);
        }
    }
}

extern "C" const char _binary_rosy_core_glsl_start;
extern "C" const char _binary_rosy_core_glsl_end;

extern "C" const char _binary_main_glsl_start;
extern "C" const char _binary_main_glsl_end;

const std::string core_glsl(&_binary_rosy_core_glsl_start, &_binary_rosy_core_glsl_end - &_binary_rosy_core_glsl_start);
const std::string main_glsl(&_binary_main_glsl_start, &_binary_main_glsl_end - &_binary_main_glsl_start);

namespace rosy::graphics {
    Graphics::Graphics() {
        static constexpr std::array indices {
                0, 1, 3,
                1, 2, 3
        };

        std::array fragment_shaders = {
            R"(
                #version 400
                #define FRAGMENT_SHADER
            )",
            core_glsl.data(),
            "\n",
            main_glsl.data()
        };

        std::array vertex_shaders = {
            R"(
                #version 400
                #define VERTEX_SHADER
            )",
            core_glsl.data(),
            "\n",
            main_glsl.data()
        };

        shader.vertex = {glCreateShaderProgramv(GL_VERTEX_SHADER, vertex_shaders.size(), vertex_shaders.data())};
        ValidateProgram(shader.vertex.program);

        shader.fragment = {glCreateShaderProgramv(GL_FRAGMENT_SHADER, fragment_shaders.size(), fragment_shaders.data())};
        ValidateProgram(shader.fragment.program);

        glCreateProgramPipelines(1, &shader.pipeline.program);
        shader.pipeline.useProgramStages(GL_VERTEX_SHADER_BIT, shader.vertex);
        shader.pipeline.useProgramStages(GL_FRAGMENT_SHADER_BIT, shader.fragment);
        glBindProgramPipeline(shader.pipeline.program);
        ValidateProgram(shader.pipeline.program);

        location.in_data.position = shader.pipeline.getAttribLocation("in_data.position");
        location.in_data.uv = shader.pipeline.getAttribLocation("in_data.uv");

        glCreateBuffers(1, &m_vbo);
        glNamedBufferStorage(m_vbo, sizeof(VertexData) * 4, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

        glCreateBuffers(1, &m_ibo);
        glNamedBufferData(m_ibo, sizeof(indices), indices.data(), GL_STATIC_DRAW);

        glCreateVertexArrays(1, &m_vao);
        glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, sizeof(VertexData));
        glVertexArrayElementBuffer(m_vao, m_ibo);

        glEnableVertexArrayAttrib(m_vao, GLint(location.in_data.position));
        glEnableVertexArrayAttrib(m_vao, GLint(location.in_data.uv));

        glVertexArrayAttribFormat(m_vao, GLint(location.in_data.position), 2, GL_FLOAT, GL_FALSE, offsetof(VertexData, x));
        glVertexArrayAttribFormat(m_vao, GLint(location.in_data.uv), 2, GL_FLOAT, GL_FALSE, offsetof(VertexData, u));

        glVertexArrayAttribBinding(m_vao, GLint(location.in_data.position), 0);
        glVertexArrayAttribBinding(m_vao, GLint(location.in_data.uv), 0);

        m_buffer = reinterpret_cast<VertexData*>(glMapNamedBufferRange(m_vbo, 0, sizeof(VertexData) * 4, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT));

        static constexpr std::array vertices = {
                VertexData{-1.0f, -1.0f, 0, 0},
                VertexData{1.0f, -1.0f, 1, 0},
                VertexData{1.0f, 1.0f, 1, 1},
                VertexData{-1.0f, 1.0f, 0, 1}
        };

        memcpy(m_buffer, vertices.data(), sizeof(vertices));
    }

    Graphics::~Graphics() {
        glUnmapNamedBuffer(m_vbo);
    }

    void Graphics::draw() {
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void rosy::graphics::Graphics::present() noexcept {
        m_window->swapBuffers();
    }

    void Graphics::setSize(const int width, const int height) noexcept {
        m_camera->setSize(width, height);

        m_buffer[1].u = float(width);
        m_buffer[2].u = float(width);
        m_buffer[2].v = float(height);
        m_buffer[3].v = float(height);

        shader.fragment.setInt("iResolution", width, height);
    }

    void Graphics::clear() noexcept {
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}
