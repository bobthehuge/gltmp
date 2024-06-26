#include <bobgl.h>
#include <log.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define RESX 1920
#define RESY 1080
#define DWINOPT {RESX, RESY, "BobAfkRender", 0, 1}

#define NAME0 "Display"     // MAIN
#define NAMEA "Test"        // BUFFER A
#define NAMEB ""            // BUFFER B
#define NAMEC ""            // BUFFER C
#define NAMED ""            // BUFFER D

#define VERTX "vert.glsl"

#define FRAG0 "frag"NAME0".glsl"
#define FRAGA "frag"NAMEA".glsl"
#define FRAGB "frag"NAMEB".glsl"
#define FRAGC "frag"NAMEC".glsl"
#define FRAGD "frag"NAMED".glsl"

struct timespec t_org;

GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

struct Framebuffer
{
    GLuint loc;
    GLuint attach0;
    GLuint attach1;
};

struct Shader
{
    GLuint prog;
    char *frag_path;
    GLuint frag;
    GLuint vert;
};

struct Shader disp = { 0, FRAG0, 0, 0 };
struct Shader life = { 0, FRAGA, 0, 0 };

struct Framebuffer life_fbo = { 0, 0, 0 };

GLuint display_tex;

GLuint vao;
GLuint vbo;
GLuint ebo;
GLuint prog;

GLuint curr;
GLuint next;

float diff = 0.0;
unsigned long frame_count = 0;

float diff_timespec(const struct timespec* t1, const struct timespec* t0)
{
    float second = difftime(t1->tv_sec, t0->tv_sec);
    return second + ((float)t1->tv_nsec - (float)t0->tv_nsec) / 1e9;
}

void reload_shaders_attribs(GLuint prog)
{
    /* GLuint position_loc = glGetAttribLocation(prog, "Position"); */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0);
    glEnableVertexAttribArray(0);

    /* mouseUniform = glGetUniformLocation(prog, "iMouse"); */

    /* glUniform1i(glGetUniformLocation(prog, "iChannel0"), 0); */
    /* glUniform1i(glGetUniformLocation(prog, "iChannel1"), 1); */
    /* glUniform1i(glGetUniformLocation(prog, "iChannel2"), 2); */
    /* glUniform1i(glGetUniformLocation(prog, "iChannel3"), 3); */

    glUniform2f(glGetUniformLocation(prog, "iResolution"), RESX, RESY);
    glUniform1f(glGetUniformLocation(prog, "iTime"), diff);
    glUniform1ui(glGetUniformLocation(prog, "iFrame"), frame_count);
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

void attach_tex(struct Framebuffer *fbo, GLuint *attach, GLenum attach_type)
{
    GLuint tex = 0;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, RESX, RESY, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo->loc);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, attach_type, GL_TEXTURE_2D, tex, 0);
    glDrawBuffers(2, drawBuffers);

    /* glBindTexture(GL_TEXTURE_2D, 0); */
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);

    *attach = tex;
}

void attach_rbo(struct Framebuffer *buffer, GLuint *attach, GLenum attach_type)
{
    GLuint rbuf = 0;

    glBindFramebuffer(GL_FRAMEBUFFER, buffer->loc);

    glGenRenderbuffers(1, &rbuf);
    glBindRenderbuffer(GL_RENDERBUFFER, rbuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, RESX, RESY);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attach_type, GL_RENDERBUFFER, rbuf);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    *attach = rbuf;
}

void create_fbo(struct Framebuffer *buffer)
{
    glGenFramebuffers(1, &buffer->loc);

    /* glBindFramebuffer(GL_FRAMEBUFFER, 0); */

}

void delete_texfbo(struct Framebuffer *buffer)
{
    glDeleteTextures(1, &buffer->attach0);
    glDeleteTextures(1, &buffer->attach1);
    glDeleteFramebuffers(1, &buffer->loc);
}

void delete_renderfbo(struct Framebuffer *buffer)
{
    glDeleteRenderbuffers(1, &buffer->attach0);
    glDeleteRenderbuffers(1, &buffer->attach1);
    glDeleteFramebuffers(1, &buffer->loc);
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
            reload_shader(&disp);
            reload_shader(&life);
            clock_gettime(CLOCK_REALTIME, &t_org);
        }
    }
}

void update_iMouse(BGL_Window* window, double xpos, double ypos)
{
    (void) window;
    glUniform2f(glGetUniformLocation(life.prog, "iMouse"), xpos, ypos);
    glUniform2f(glGetUniformLocation(disp.prog, "iMouse"), xpos, ypos);
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

    float vertices[] = BGL_WIN_RECT;
    int indices[] = BGL_RECT_IDX;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    create_shader(&disp);
    create_shader(&life);

    create_fbo(&life_fbo);
    /* attach_rbo(&life_fbo, &life_fbo.attach0, GL_COLOR_ATTACHMENT0); */
    /* attach_rbo(&life_fbo, &life_fbo.attach1, GL_COLOR_ATTACHMENT1); */
    attach_tex(&life_fbo, &life_fbo.attach1, GL_COLOR_ATTACHMENT1);
    attach_tex(&life_fbo, &life_fbo.attach0, GL_COLOR_ATTACHMENT0);

    /* glBindFramebuffer(GL_FRAMEBUFFER, life_fbo.loc); */
    /* glDrawBuffer(GL_COLOR_ATTACHMENT0); */
    /* glBindFramebuffer(GL_FRAMEBUFFER, 0); */

    glBindFramebuffer(GL_FRAMEBUFFER, life_fbo.loc);
    glViewport(0, 0, RESX, RESY);
    /* glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, channel0->tex, 0); */
    glDrawBuffers(2, drawBuffers);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);

    prog = life.prog;

    /* curr = bufferA->fbo.tex; */
    /* next = channel0->tex; */
    /* glUseProgram(prog); */

    glfwSetCursorPosCallback(win, update_iMouse);
    glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);

    clock_gettime(CLOCK_REALTIME, &t_org);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    /* glClearDepth(10.0); */
    /* glfwSwapInterval(1); */
}

void bgl_on_update(void)
{
    /* BGL_Window* win = glfwGetCurrentContext(); */

    /* glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); */

//////////////////////////////

    prog = life.prog;

    glUseProgram(prog);

    glBindFramebuffer(GL_FRAMEBUFFER, life_fbo.loc);
    glViewport(0, 0, RESX, RESY);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, life_fbo.attach0, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, life_fbo.attach1, 0);
    /* glDrawBuffer(GL_COLOR_ATTACHMENT0); */
    glDrawBuffers(2, drawBuffers);

    reload_shaders_attribs(prog);

    /* int status = frame_count % 2 == 0 ? 0 : 1; */
    /* glDrawBuffers(1, drawBuffers + status); */

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, life_fbo.attach0);
    glUniform1i(glGetUniformLocation(prog, "iChannel0"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, life_fbo.attach1);
    glUniform1i(glGetUniformLocation(prog, "iChannel1"), 1);

    glActiveTexture(GL_TEXTURE0);

    /* /1* glClearColor(0.0, 0.0, 0.0, 1.0); *1/ */
    /* /1* glClear(GL_COLOR_BUFFER_BIT); *1/ */
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

//////////////////////////////

    prog = disp.prog;
    /* prog = life.prog; */

    glUseProgram(prog);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, RESX, RESY);
    reload_shaders_attribs(prog);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, life_fbo.attach0);
    glUniform1i(glGetUniformLocation(prog, "iChannel0"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, life_fbo.attach1);
    glUniform1i(glGetUniformLocation(prog, "iChannel1"), 1);

    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

//////////////////////////////

    sleep(1);

    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    diff = diff_timespec(&now, &t_org);

    frame_count++;
}

void bgl_on_exit(void)
{
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &ebo);

    delete_texfbo(&life_fbo);
    glDeleteTextures(1, &display_tex);
    delete_shader(&disp);
    delete_shader(&life);
}

int main(void)
{
    srand(time(NULL));
    bgl_init(bgl_on_load);
    bgl_loop(bgl_on_update);
    bgl_exit(bgl_on_exit);

    return EXIT_SUCCESS;
}
