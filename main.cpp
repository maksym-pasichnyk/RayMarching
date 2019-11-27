#include <rosy.h>
#include <rosy.h>
#include <module.h>
#include <graphics/shader.h>
#include <graphics/graphics.h>
#include <window/window.h>
#include <camera.h>

#include <cstring>

namespace {
    using rosy::graphics::ShaderType;
    using rosy::graphics::ShaderProgram;
    using rosy::graphics::Attrib;

    struct VertexData {
        float x, y, u, v;
    } *m_buffer;

    ShaderProgram vertex;
    ShaderProgram fragment;
    ShaderProgram pipeline;

    rosy::timer::point timeStart;

    GLuint m_vbo, m_ibo, m_vao;

    Camera camera{};

    extern "C" const char _binary_core_glsl_start;
    extern "C" const char _binary_core_glsl_end;

    extern "C" const char _binary_main_glsl_start;
    extern "C" const char _binary_main_glsl_end;
}

void rosy::load() {
    const std::string core_glsl(&_binary_core_glsl_start, &_binary_core_glsl_end - &_binary_core_glsl_start);
    const std::string main_glsl(&_binary_main_glsl_start, &_binary_main_glsl_end - &_binary_main_glsl_start);

    static constexpr std::array indices {
            0, 1, 3,
            1, 2, 3
    };

    vertex = rosy::graphics::newShader(ShaderType::Vertex, {
            R"(
                #version 400
                #define VERTEX_SHADER
            )",
            core_glsl.data(),
            "\n",
            main_glsl.data()
    });

    fragment = rosy::graphics::newShader(ShaderType::Fragment, {
            R"(
                #version 400
                #define FRAGMENT_SHADER
            )",
            core_glsl.data(),
            "\n",
            main_glsl.data()
    });

    glCreateProgramPipelines(1, &pipeline.program);
    pipeline.useProgramStages(GL_VERTEX_SHADER_BIT, vertex);
    pipeline.useProgramStages(GL_FRAGMENT_SHADER_BIT, fragment);
    glBindProgramPipeline(pipeline.program);
    
    Attrib position = pipeline.getAttribLocation("in_data.position");
    Attrib uv = pipeline.getAttribLocation("in_data.uv");

    glCreateBuffers(1, &m_vbo);
    glNamedBufferStorage(m_vbo, sizeof(VertexData) * 4, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

    glCreateBuffers(1, &m_ibo);
    glNamedBufferData(m_ibo, sizeof(indices), indices.data(), GL_STATIC_DRAW);

    glCreateVertexArrays(1, &m_vao);
    glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, sizeof(VertexData));
    glVertexArrayElementBuffer(m_vao, m_ibo);

    glEnableVertexArrayAttrib(m_vao, GLint(position));
    glEnableVertexArrayAttrib(m_vao, GLint(uv));

    glVertexArrayAttribFormat(m_vao, GLint(position), 2, GL_FLOAT, GL_FALSE, offsetof(VertexData, x));
    glVertexArrayAttribFormat(m_vao, GLint(uv), 2, GL_FLOAT, GL_FALSE, offsetof(VertexData, u));

    glVertexArrayAttribBinding(m_vao, GLint(position), 0);
    glVertexArrayAttribBinding(m_vao, GLint(uv), 0);

    m_buffer = reinterpret_cast<VertexData*>(glMapNamedBufferRange(m_vbo, 0, sizeof(VertexData) * 4, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT));

    static constexpr std::array vertices = {
            VertexData{-1.0f, -1.0f, 0, 0},
            VertexData{1.0f, -1.0f, 1, 0},
            VertexData{1.0f, 1.0f, 1, 1},
            VertexData{-1.0f, 1.0f, 0, 1}
    };

    std::memcpy(m_buffer, vertices.data(), sizeof(vertices));

    camera.transform.position = {0, 1, 10};

    fragment.set("camera.position", camera.transform.position.get());
    fragment.set("camera.localToWorldMatrix", camera.transform.localToWorldMatrix());
    fragment.set("camera.cameraToWorldMatrix", camera.cameraToWorldMatrix());

    resize(rosy::window::getWidth(), rosy::window::getHeight());

    rosy::window::setRelativeMouse(true);

    timeStart = rosy::timer::getTime();
}

void rosy::unload() {
    glUnmapNamedBuffer(m_vbo);
}

void rosy::resize(int width, int height) {
    glViewport(0, 0, width, height);

    m_buffer[1].u = float(width);
    m_buffer[2].u = float(width);
    m_buffer[2].v = float(height);
    m_buffer[3].v = float(height);

    fragment.setInt("iResolution", width, height);
}

void rosy::update(rosy::timer::duration dt) {
    uint8_t state = camera.update(dt);

    if (state & Camera::UpdateState::updatePos) {
        fragment.set("camera.position", camera.transform.position.get());
    }

    if (state & Camera::UpdateState::updateMat) {
        fragment.set("camera.localToWorldMatrix", camera.transform.localToWorldMatrix());
        fragment.set("camera.cameraToWorldMatrix", camera.cameraToWorldMatrix());
    }

    auto iTime = std::chrono::duration<double>(rosy::timer::getTime() - timeStart);
    fragment.setFloat("iTime", iTime.count());
}

void rosy::draw() {
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}