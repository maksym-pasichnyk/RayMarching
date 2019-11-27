#include "graphics.h"
#include "module.h"
#include "window/window.h"
#include <GL/glew.h>

namespace {
    module<rosy::graphics::Graphics> instance;
    module<rosy::window::Window> m_window;

    static void Validate(const rosy::graphics::ShaderProgram shader) {
        GLint isLinked = 0;
        glGetProgramiv(shader.program, GL_LINK_STATUS, (int *)&isLinked);
        if (isLinked == GL_FALSE) {
            GLint maxLength = 0;
            glGetProgramiv(shader.program, GL_INFO_LOG_LENGTH, &maxLength);
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(shader.program, maxLength, &maxLength, &infoLog[0]);
            std::cerr << infoLog.data() << std::endl;
            exit(1);
        }
    }
}

namespace rosy::graphics {
    Graphics::Graphics() {}

    Graphics::~Graphics() {}

    void rosy::graphics::Graphics::present() noexcept {
        m_window->swapBuffers();
    }

    void Graphics::clear() noexcept {
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    ShaderProgram Graphics::newShader(const ShaderType type, const std::initializer_list<const char*> sources) {
        ShaderProgram shader{glCreateShaderProgramv(GLint(type), sources.size(), sources.begin())};
        Validate(shader);
        return shader;
    }

    ShaderProgram newShader(const ShaderType type, const std::string_view source) {
        return Graphics::newShader(type, {source.data()});
    }

    ShaderProgram newShader(const ShaderType type, const std::initializer_list<const char*> sources) {
        return Graphics::newShader(type, sources);
    }
}
