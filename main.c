#include <bobgl.h>
#include <log.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

GLuint vert;
GLuint frag;
GLuint prog;
GLuint vao;
GLuint vbo;

void reload_shaders_attribs(void)
{
    GLint posAttrib = glGetAttribLocation(prog, "iPosition");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0);
}

void reload_shaders(void)
{
    glDetachShader(prog, vert);
    glDetachShader(prog, frag);
    glDeleteProgram(prog);

    prog = glCreateProgram();
    vert = bgl_new_shader(GL_VERTEX_SHADER, "vert.glsl");
    frag = bgl_new_shader(GL_FRAGMENT_SHADER, "frag.glsl");

    glAttachShader(prog, vert);
    glAttachShader(prog, frag);

    glDeleteShader(vert);
    glDeleteShader(frag);

    glLinkProgram(prog);
    reload_shaders_attribs();
    glUseProgram(prog);
}

void my_keycalls(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void) scancode;
    (void) action;
    (void) mods;

    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_ESCAPE)
        {
            exit(0);
        }
        if (key == GLFW_KEY_Q)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        if (key == GLFW_KEY_F5)
        {
            reload_shaders();
        }
    }
}

void on_load(void)
{
    float vertices[] = BGL_RECT_VERT;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    reload_shaders();

    BGL_Window* window = glfwGetCurrentContext();
    glfwSetKeyCallback(window, my_keycalls);
}

void on_update(void)
{
    BGL_Window* window = glfwGetCurrentContext();
    glBindVertexArray(vao);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(window);
    sleep(0.5);
}

void on_exit(void)
{
    glDetachShader(prog, vert);
    glDetachShader(prog, frag);
    glDeleteProgram(prog);
}

int main(void)
{
    bgl_init(on_load);
    bgl_loop(on_update);
    bgl_exit(on_exit);

    return EXIT_SUCCESS;
}
