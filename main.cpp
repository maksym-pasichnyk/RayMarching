#include <rosy.h>

#include <graphics/graphics.h>
#include <graphics/shader.h>
#include <window/window.h>
#include <audio/audio.h>

#include <camera.h>

#include <cstring>

namespace {
    inline constexpr float HALF_FOV_60_TAN = 0.577350269f;

    using rosy::graphics::ShaderType;
    using rosy::graphics::ShaderProgram;
    using rosy::graphics::Attrib;
    using rosy::audio::Source;
    using rosy::audio::SourceType;

    struct VertexData {
        float x, y, u, v;
    } *m_buffer;

    ShaderProgram vertex;
    ShaderProgram fragment;
    ShaderProgram pipeline;

    rosy::timer::point timeStart;

    GLuint m_vbo, m_ibo, m_vao;

    Camera camera{};

    // todo: resources
    extern "C" const char _binary_core_glsl_start;
    extern "C" const char _binary_core_glsl_end;

    extern "C" const char _binary_main_glsl_start;
    extern "C" const char _binary_main_glsl_end;

    extern "C" const char _binary_fractal_glsl_start;
    extern "C" const char _binary_fractal_glsl_end;

    std::unique_ptr<Source> sound;
}


void rosy::load() {
    sound = rosy::audio::newSource("../supernatural.wav", SourceType::Stream);
    sound->setLoop(true);
    sound->play();

    const std::string core_glsl(&_binary_core_glsl_start, &_binary_core_glsl_end - &_binary_core_glsl_start);
    const std::string main_glsl(&_binary_main_glsl_start, &_binary_main_glsl_end - &_binary_main_glsl_start);
    const std::string fractal_glsl(&_binary_fractal_glsl_start, &_binary_fractal_glsl_end - &_binary_fractal_glsl_start);

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
            fractal_glsl.data()
    });

    fragment = rosy::graphics::newShader(ShaderType::Fragment, {
            R"(
                #version 400
                #define FRAGMENT_SHADER
            )",
            core_glsl.data(),
            "\n",
            fractal_glsl.data()
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

    camera.transform.position = {0, 0, 3};

    fragment.set("_WorldSpaceCameraPos", camera.transform.position.get());
    fragment.set("_LocalToWorldMatrix", camera.transform.localToWorldMatrix());
    fragment.set("_CameraToWorldMatrix", camera.cameraToWorldMatrix());

    resize(rosy::window::getWidth(), rosy::window::getHeight());

    rosy::window::setRelativeMouse(true);

    timeStart = rosy::timer::getTime();
}

void rosy::unload() {
    glUnmapNamedBuffer(m_vbo);
}

void rosy::resize(int width, int height) {
    glViewport(0, 0, width, height);

    const float yScale = 1.0f / float(height);
    const float aspect = float(width) * yScale;

    m_buffer[0].u = -aspect * HALF_FOV_60_TAN + yScale;
    m_buffer[0].v = -HALF_FOV_60_TAN + yScale;

    m_buffer[1].u = aspect * HALF_FOV_60_TAN + yScale;
    m_buffer[1].v = -HALF_FOV_60_TAN + yScale;

    m_buffer[2].u = aspect * HALF_FOV_60_TAN + yScale;
    m_buffer[2].v = HALF_FOV_60_TAN + yScale;

    m_buffer[3].u = -aspect * HALF_FOV_60_TAN + yScale;
    m_buffer[3].v = HALF_FOV_60_TAN + yScale;

    fragment.setInt("_Resolution", width, height);
}

void rosy::update(rosy::timer::duration dt) {
    uint8_t state = camera.update(dt);

    if (state & Camera::UpdateState::updatePos) {
        fragment.set("_WorldSpaceCameraPos", camera.transform.position.get());
    }

    if (state & Camera::UpdateState::updateMat) {
        fragment.set("_LocalToWorldMatrix", camera.transform.localToWorldMatrix());
        fragment.set("_CameraToWorldMatrix", camera.cameraToWorldMatrix());
    }

    auto iTime = std::chrono::duration<double>(rosy::timer::getTime() - timeStart);
    fragment.setFloat("_Time", iTime.count());
}

void rosy::draw() {
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}