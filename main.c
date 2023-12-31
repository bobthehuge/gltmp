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

#define RESX 1920
#define RESY 1080
#define DWINOPT {RESX, RESY, "BobAfkRender", 0, 0}

#define NAME "Julia"

#define VERT_SHADER "vert.glsl"
#define FRAG_SHADER "frag"NAME".glsl"

struct timespec t_org;
GLuint timeUniform;
GLuint resolutionUniform;
GLuint mouseUniform;

float diff_timespec(const struct timespec* t1, const struct timespec* t0)
{
    float second = difftime(t1->tv_sec, t0->tv_sec);
    return second + ((float)t1->tv_nsec - (float)t0->tv_nsec) / 1e9;
}

void reload_shaders_attribs(void)
{
    GLint posAttrib = glGetAttribLocation(prog, "Position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0);

    timeUniform = glGetUniformLocation(prog, "iTime");
    resolutionUniform = glGetUniformLocation(prog, "iResolution");
    mouseUniform = glGetUniformLocation(prog, "iMouse");
    glUniform2f(resolutionUniform, RESX, RESY);
}

void reload_shaders(void)
{
    glDetachShader(prog, vert);
    glDetachShader(prog, frag);
    glDeleteProgram(prog);

    prog = glCreateProgram();
    vert = bgl_new_shader(GL_VERTEX_SHADER, VERT_SHADER);
    frag = bgl_new_shader(GL_FRAGMENT_SHADER, FRAG_SHADER);

    glAttachShader(prog, vert);
    glAttachShader(prog, frag);

    glDeleteShader(vert);
    glDeleteShader(frag);

    glLinkProgram(prog);
    glUseProgram(prog);
    reload_shaders_attribs();
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
            clock_gettime(CLOCK_REALTIME, &t_org);
        }
    }
}

void bgl_on_load(void)
{

    BGL_Window* win = glfwGetCurrentContext();
    glfwDestroyWindow(win);
    struct BGL_Winopt winopt = DWINOPT;
    win = bgl_create_window(&winopt);
    glfwSetKeyCallback(win, my_keycalls);
    glfwMakeContextCurrent(win);

    float vertices[] = BGL_RECT_VERT;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    reload_shaders();

    clock_gettime(CLOCK_REALTIME, &t_org);
}

void update_iMouse(BGL_Window* window, double xpos, double ypos)
{
    (void) window;
    glUniform2f(mouseUniform, xpos, ypos);
}

void bgl_on_update(void)
{
    BGL_Window* win = glfwGetCurrentContext();
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    float diff = diff_timespec(&now, &t_org);
    glUniform1f(timeUniform, diff);

    glfwSetCursorPosCallback(win, update_iMouse);
}

void bgl_on_exit(void)
{
    glDetachShader(prog, vert);
    glDetachShader(prog, frag);
    glDeleteProgram(prog);
}

int main(void)
{
    bgl_init(bgl_on_load);
    bgl_loop(bgl_on_update);
    bgl_exit(bgl_on_exit);

    return EXIT_SUCCESS;
}
