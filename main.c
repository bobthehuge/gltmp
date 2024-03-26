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
#define NAMEA "Life"        // BUFFER A
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

GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};

struct Framebuffer
{
    GLuint loc;
    GLuint tex;
};

struct Buffer
{
    GLuint prog;
    char *frag_path;
    GLuint frag;
    GLuint vert;
    struct Framebuffer fbo;
};

struct Buffer buffers[] = {
    { 0, FRAG0, 0, 0, { 0, 0 } },
    { 0, FRAGA, 0, 0, { 0, 0 } },
    { 0, FRAGB, 0, 0, { 0, 0 } },
    { 0, FRAGC, 0, 0, { 0, 0 } },
    { 0, FRAGD, 0, 0, { 0, 0 } },
};

struct Buffer *buffer0 = buffers;
struct Buffer *bufferA = buffers + 1;

struct Framebuffer channels[] = {
    { 0, 0 },
    { 0, 0 },
};

struct Framebuffer *channel0 = channels;
struct Framebuffer *channel1 = channels + 1;

/* GLuint channels[] = {0, 0, 0, 0}; */
/* GLuint *channel0 = channels; */
/* GLuint *channel1 = channels + 1; */

GLuint vao;
GLuint vbo;
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

void create_shader(struct Buffer *buffer)
{
    GLuint prog = glCreateProgram();
    GLuint vert = bgl_new_shader(GL_VERTEX_SHADER, VERTX);
    GLuint frag = bgl_new_shader(GL_FRAGMENT_SHADER, buffer->frag_path);

    glAttachShader(prog, vert);
    glAttachShader(prog, frag);

    glDeleteShader(vert);
    glDeleteShader(frag);

    glLinkProgram(prog);

    /* glUseProgram(prog); */


    buffer->prog = prog;
    buffer->vert = vert;
    buffer->frag = frag;
}

void delete_shader(struct Buffer *buffer)
{
    glDetachShader(buffer->prog, buffer->vert);
    glDetachShader(buffer->prog, buffer->frag);
    glDeleteProgram(buffer->prog);
}

void reload_shader(struct Buffer *buffer)
{
    delete_shader(buffer);
    create_shader(buffer);
}

void create_fbo(struct Framebuffer *buffer)
{
    GLuint fbo = 0;
    GLuint tex = 0;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, RESX, RESY, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
    glDrawBuffers(1, drawBuffers);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);

    buffer->loc = fbo;
    buffer->tex = tex;
}

void delete_fbo(struct Framebuffer *buffer)
{
    glDeleteFramebuffers(1, &buffer->loc);
    glDeleteTextures(1, &buffer->tex);
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
            reload_shader(buffer0);
            reload_shader(bufferA);
            clock_gettime(CLOCK_REALTIME, &t_org);
        }
    }
}

void update_iMouse(BGL_Window* window, double xpos, double ypos)
{
    (void) window;
    glUniform2f(glGetUniformLocation(buffer0->prog, "iMouse"), xpos, ypos);
    glUniform2f(glGetUniformLocation(bufferA->prog, "iMouse"), xpos, ypos);
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

///////////////               FBOs              ///////////////

    create_fbo(&buffer0->fbo);
    create_shader(buffer0);
    create_fbo(&bufferA->fbo);
    create_shader(bufferA);

///////////////             CHANNELS            ///////////////

    create_fbo(channel0);
    /* create_fbo(channel1); */

    /* glBindFramebuffer(GL_FRAMEBUFFER, bufferA->fbo.loc); */
    /* glViewport(0, 0, RESX, RESY); */
    /* glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, channel0->tex, 0); */
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);

    prog = bufferA->prog;

    curr = bufferA->fbo.tex;
    next = channel0->tex;
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

    GLuint prog = bufferA->prog;

    glUseProgram(prog);

    glBindFramebuffer(GL_FRAMEBUFFER, bufferA->fbo.loc);
    glViewport(0, 0, RESX, RESY);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, curr, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    reload_shaders_attribs(prog);

    int status = frame_count % 2 == 0 ? 0 : 1;
    /* glDrawBuffers(1, drawBuffers + status); */
    /* glDrawBuffers(2, drawBuffers); */
    glDrawBuffers(1, drawBuffers);

    glActiveTexture(GL_TEXTURE0 + status);
    glBindTexture(GL_TEXTURE_2D, next);
    glUniform1i(glGetUniformLocation(prog, "iChannel0"), status);
    glUniform1f(glGetUniformLocation(prog, "iTime"), diff);
    glUniform1ui(glGetUniformLocation(prog, "iFrame"), frame_count);

    /* glClearColor(0.0, 0.0, 0.0, 1.0); */
    /* glClear(GL_COLOR_BUFFER_BIT); */
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

//////////////////////////////

    prog = buffer0->prog;

    glUseProgram(prog);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, RESX, RESY);

    reload_shaders_attribs(prog);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, curr);
    glUniform1i(glGetUniformLocation(prog, "iChannel0"), 0);
    glUniform1f(glGetUniformLocation(prog, "iTime"), diff);
    glUniform1ui(glGetUniformLocation(prog, "iFrame"), frame_count);

    /* glClearColor(0.0, 0.0, 0.0, 1.0); */
    /* glClear(GL_COLOR_BUFFER_BIT); */
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

//////////////////////////////

    curr ^= next;
    next ^= curr;
    curr ^= next;

    sleep(1);

    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    diff = diff_timespec(&now, &t_org);

    frame_count++;
}

void bgl_on_exit(void)
{
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &vbo);

    /* glDeleteTextures(1, channel0); */
    /* glDeleteTextures(1, channel1); */

    delete_shader(buffer0);
    delete_fbo(&buffer0->fbo);

    delete_shader(bufferA);
    delete_fbo(&bufferA->fbo);

    delete_fbo(channel0);
    delete_fbo(channel1);
}

int main(void)
{
    srand(time(NULL));
    bgl_init(bgl_on_load);
    bgl_loop(bgl_on_update);
    bgl_exit(bgl_on_exit);

    return EXIT_SUCCESS;
}
