#include <bobgl.h>
#include <log.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/* GLuint vertx; */
/* GLuint frag0; */
/* GLuint frag1; */

/* GLuint prog0; */
/* GLuint prog1; */

GLuint vao;
GLuint vbo;
GLuint fbo;

#define RESX 1920
#define RESY 1080
#define DWINOPT {RESX, RESY, "BobAfkRender", 0, 0}

#define NAME0 "Display"     // MAIN
#define NAME1 "Life"        // BUFFER A
#define NAME2 ""            // BUFFER B
#define NAME3 ""            // BUFFER C
#define NAME4 ""            // BUFFER D

#define VERTX "vert.glsl"

#define FRAG0 "frag"NAME0".glsl"
#define FRAG1 "frag"NAME1".glsl"
#define FRAG2 "frag"NAME2".glsl"
#define FRAG3 "frag"NAME3".glsl"
#define FRAG4 "frag"NAME4".glsl"

struct timespec t_org;
GLuint timeUniform;
GLuint resolutionUniform;
GLuint mouseUniform;

GLuint channel0;
GLuint channel1;
GLuint channel2;
GLuint channel3;

GLuint curr;
GLuint next;

struct Shader
{
    GLuint prog;
    char *frag_path;
    GLuint vert;
    GLuint frag;
};

struct Shader shader0 = {
    .prog = 0,
    .frag_path = FRAG1,
    .vert = 0,
    .frag = 0,
};

struct Shader shader1 = {
    .prog = 0,
    .frag_path = FRAG0,
    .vert = 0,
    .frag = 0,
};

float diff_timespec(const struct timespec* t1, const struct timespec* t0)
{
    float second = difftime(t1->tv_sec, t0->tv_sec);
    return second + ((float)t1->tv_nsec - (float)t0->tv_nsec) / 1e9;
}

void reload_shaders_attribs(GLuint prog)
{
    GLuint position_loc = glGetAttribLocation(prog, "Position");
    glEnableVertexAttribArray(position_loc);
    glVertexAttribPointer(position_loc, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0);

    /* mouseUniform = glGetUniformLocation(prog, "iMouse"); */

    /* glUniform1i(glGetUniformLocation(prog, "iChannel0"), 0); */
    /* glUniform1i(glGetUniformLocation(prog, "iChannel1"), 1); */
    /* glUniform1i(glGetUniformLocation(prog, "iChannel2"), 2); */
    /* glUniform1i(glGetUniformLocation(prog, "iChannel3"), 3); */

    glUniform2f(glGetUniformLocation(prog, "iResolution"), RESX, RESY);
}

void create_shader(struct Shader *shader)
{
    GLuint prog = glCreateProgram();
    GLuint vert = bgl_new_shader(GL_VERTEX_SHADER, VERTX);
    GLuint frag = bgl_new_shader(GL_FRAGMENT_SHADER, shader->frag_path);

    glAttachShader(prog, vert);
    glAttachShader(prog, frag);

    glDeleteShader(vert);
    glDeleteShader(frag);

    glLinkProgram(prog);

    /* glUseProgram(prog); */

    reload_shaders_attribs(prog);

    shader->prog = prog;
    shader->vert = vert;
    shader->frag = frag;
}

void delete_shader(struct Shader *shader)
{
    glDetachShader(shader->prog, shader->vert);
    glDetachShader(shader->prog, shader->frag);
    glDeleteProgram(shader->prog);
}

void reload_shader(struct Shader *shader)
{
    delete_shader(shader);
    create_shader(shader);
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
            reload_shader(&shader0);
            reload_shader(&shader1);
            clock_gettime(CLOCK_REALTIME, &t_org);
        }
    }
}

void update_iMouse(BGL_Window* window, double xpos, double ypos)
{
    (void) window;
    glUniform2f(glGetUniformLocation(shader0.prog, "iMouse"), xpos, ypos);
    glUniform2f(glGetUniformLocation(shader1.prog, "iMouse"), xpos, ypos);
}

void framebuffer_size_callback(BGL_Window *window, int width, int height)
{
    (void) window;
    glViewport(0, 0, width, height);
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

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &channel0);
    glBindTexture(GL_TEXTURE_2D, channel0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    char *data = calloc(RESX * RESY * 3, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, RESX, RESY, 0, GL_BGR, GL_UNSIGNED_BYTE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, channel0, 0);
    /* glGenerateMipmap(GL_TEXTURE_2D); */

    glGenTextures(1, &channel1);
    glBindTexture(GL_TEXTURE_2D, channel1);

    for(size_t i = 0; i < RESX * RESY * 3; i+=3)
    {
        char x = rand() % 2 * 255;
        data[i] = x;
        data[i+1] = x;
        data[i+2] = x;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, RESX, RESY, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    /* glGenerateMipmap(GL_TEXTURE_2D); */

    glBindTexture(GL_TEXTURE_2D, 0);

    free(data);

    create_shader(&shader0);
    create_shader(&shader1);

    /* glUseProgram(shader0.prog); */
    /* glUseProgram(shader1.prog); */

    glViewport(0, 0, RESX, RESY);

    glfwSetCursorPosCallback(win, update_iMouse);
    glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);

    curr = channel0;
    curr = channel1;

    clock_gettime(CLOCK_REALTIME, &t_org);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(10.0);
    glfwSwapInterval(1);
}

void bgl_on_update(void)
{
    /* BGL_Window* win = glfwGetCurrentContext(); */

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader0.prog);

    GLint uindex = glGetUniformLocation(shader0.prog, "iChannel0");
    if (uindex >= 0)
    {
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, curr);
        glUniform1i(uindex, 0);
    }

    uindex = glGetUniformLocation(shader0.prog, "iChannel1");
    if (uindex >= 0)
    {
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, next);
        glUniform1i(uindex, 1);
    }

    /* glBindFramebuffer(GL_FRAMEBUFFER, 0); */

    /* glBindVertexArray(vao); */
    /* glDrawArrays(GL_TRIANGLES, 0, 6); */

    curr ^= next;
    next ^= curr;
    curr ^= next;

    glUseProgram(shader1.prog);

    uindex = glGetUniformLocation(shader1.prog, "iChannel0");
    if (uindex >= 0)
    {
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, curr);
        glUniform1i(uindex, 0);
    }

    uindex = glGetUniformLocation(shader1.prog, "iChannel1");
    if (uindex >= 0)
    {
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, next);
        glUniform1i(uindex, 1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    float diff = diff_timespec(&now, &t_org);

    glUniform1f(glGetUniformLocation(shader0.prog, "iTime"), diff);
    glUniform1f(glGetUniformLocation(shader1.prog, "iTime"), diff);

    sleep(1);
}

void bgl_on_exit(void)
{
    glDeleteTextures(1, &channel0);
    glDeleteTextures(1, &channel1);
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteFramebuffers(1, &fbo);

    delete_shader(&shader0);
    delete_shader(&shader1);
}

int main(void)
{
    srand(time(NULL));
    bgl_init(bgl_on_load);
    bgl_loop(bgl_on_update);
    bgl_exit(bgl_on_exit);

    return EXIT_SUCCESS;
}
