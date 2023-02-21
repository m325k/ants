#include <gl/GL.h>

#define internal static
#define global_variable static

#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_COMPUTE_SHADER                 0x91B9
#define GL_VALIDATE_STATUS                0x8B83
#define GL_LINK_STATUS                    0x8B82
#define GL_WRITE_ONLY                     0x88B9
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_TEXTURE0                       0x84C0
#define GL_RGBA32F                        0x8814
#define GL_SHADER_IMAGE_ACCESS_BARRIER_BIT 0x00000020
#define GL_SHADER_STORAGE_BARRIER_BIT     0x00002000
#define GL_SHADER_STORAGE_BUFFER          0x90D2
#define GL_DYNAMIC_READ                   0x88E9
#define GL_DYNAMIC_COPY                   0x88EA
#define GL_READ_ONLY                      0x88B8
#define GL_STATIC_DRAW                    0x88E4

typedef char GLchar;
typedef size_t GLsizeiptr;

typedef void gl_attach_shader(GLuint program, GLuint shader);
typedef void gl_compile_shader(GLuint shader);
typedef GLuint gl_create_program(void);
typedef GLuint gl_create_shader(GLenum type);
typedef void gl_link_program(GLuint program);
typedef void gl_shader_source(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
typedef void gl_use_program(GLuint program);
typedef void gl_validate_program(GLuint program);
typedef void gl_get_programiv(GLuint program, GLenum pname, GLint* params);
typedef void gl_get_shader_info_log(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
typedef void gl_get_program_info_log(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
typedef void gl_uniform_1f(GLint location, GLfloat v0);
typedef GLint gl_get_uniform_location(GLuint program, const GLchar* name);
typedef void gl_bind_image_texture(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);
typedef void gl_active_texture(GLenum texture);
typedef void gl_dispatch_compute(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
typedef void gl_memory_barrier(GLbitfield barriers);
typedef void gl_gen_buffers(GLsizei n, GLuint* buffers);
typedef void gl_bind_buffer(GLenum target, GLuint buffer);
typedef void gl_bind_buffer_base(GLenum target, GLuint index, GLuint buffer);
typedef void gl_buffer_data(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
typedef void gl_clear_tex_image(GLuint texture, GLint level, GLenum format, GLenum type, const void* data);
typedef void* gl_map_buffer(GLenum target, GLenum access);

global_variable gl_attach_shader* glAttachShader;
global_variable gl_compile_shader* glCompileShader;
global_variable gl_create_program* glCreateProgram;
global_variable gl_create_shader* glCreateShader;
global_variable gl_link_program* glLinkProgram;
global_variable gl_shader_source* glShaderSource;
global_variable gl_use_program* glUseProgram;
global_variable gl_validate_program* glValidateProgram;
global_variable gl_get_programiv* glGetProgramiv;
global_variable gl_get_shader_info_log* glGetShaderInfoLog;
global_variable gl_get_program_info_log* glGetProgramInfoLog;
global_variable gl_uniform_1f* glUniform1f;
global_variable gl_get_uniform_location* glGetUniformLocation;
global_variable gl_bind_image_texture* glBindImageTexture;
global_variable gl_active_texture* glActiveTexture;
global_variable gl_dispatch_compute* glDispatchCompute;
global_variable gl_memory_barrier* glMemoryBarrier;
global_variable gl_gen_buffers* glGenBuffers;
global_variable gl_bind_buffer* glBindBuffer;
global_variable gl_bind_buffer_base* glBindBufferBase;
global_variable gl_buffer_data* glBufferData;
global_variable gl_clear_tex_image* glClearTexImage;
global_variable gl_map_buffer* glMapBuffer;

internal void InitOpenGL(HWND window) {
    HDC windowDC = GetDC(window);

    PIXELFORMATDESCRIPTOR desiredPixelFormat = {};
    desiredPixelFormat.nSize = sizeof(desiredPixelFormat);
    desiredPixelFormat.nVersion = 1;
    desiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    desiredPixelFormat.cColorBits = 32;
    desiredPixelFormat.cAlphaBits = 8;
    desiredPixelFormat.iLayerType = PFD_MAIN_PLANE;

    int suggestedIndex = ChoosePixelFormat(windowDC, &desiredPixelFormat);
    PIXELFORMATDESCRIPTOR suggestedPixelFormat = {};
    DescribePixelFormat(windowDC, suggestedIndex, sizeof(suggestedPixelFormat), &suggestedPixelFormat);
    SetPixelFormat(windowDC, suggestedIndex, &suggestedPixelFormat);


    HGLRC OpenGLRC = wglCreateContext(windowDC);
    if (wglMakeCurrent(windowDC, OpenGLRC)) {
        glAttachShader = (gl_attach_shader*)wglGetProcAddress("glAttachShader");
        glCompileShader = (gl_compile_shader*)wglGetProcAddress("glCompileShader");
        glCreateProgram = (gl_create_program*)wglGetProcAddress("glCreateProgram");
        glCreateShader = (gl_create_shader*)wglGetProcAddress("glCreateShader");
        glLinkProgram = (gl_link_program*)wglGetProcAddress("glLinkProgram");
        glShaderSource = (gl_shader_source*)wglGetProcAddress("glShaderSource");
        glUseProgram = (gl_use_program*)wglGetProcAddress("glUseProgram");
        glValidateProgram = (gl_validate_program*)wglGetProcAddress("glValidateProgram");
        glGetProgramiv = (gl_get_programiv*)wglGetProcAddress("glGetProgramiv");
        glGetShaderInfoLog = (gl_get_shader_info_log*)wglGetProcAddress("glGetShaderInfoLog");
        glGetProgramInfoLog = (gl_get_program_info_log*)wglGetProcAddress("glGetProgramInfoLog");
        glUniform1f = (gl_uniform_1f*)wglGetProcAddress("glUniform1f");
        glGetUniformLocation = (gl_get_uniform_location*)wglGetProcAddress("glGetUniformLocation");
        glActiveTexture = (gl_active_texture*)wglGetProcAddress("glActiveTexture");
        glBindImageTexture = (gl_bind_image_texture*)wglGetProcAddress("glBindImageTexture");
        glDispatchCompute = (gl_dispatch_compute*)wglGetProcAddress("glDispatchCompute");
        glMemoryBarrier = (gl_memory_barrier*)wglGetProcAddress("glMemoryBarrier");
        glGenBuffers = (gl_gen_buffers*)wglGetProcAddress("glGenBuffers");
        glBindBuffer = (gl_bind_buffer*)wglGetProcAddress("glBindBuffer");
        glBindBufferBase = (gl_bind_buffer_base*)wglGetProcAddress("glBindBufferBase");
        glBufferData = (gl_buffer_data*)wglGetProcAddress("glBufferData");
        glClearTexImage = (gl_clear_tex_image*)wglGetProcAddress("glClearTexImage");
        glMapBuffer = (gl_map_buffer*)wglGetProcAddress("glMapBuffer");

        char* version = (char*)glGetString(GL_VERSION);
        char buffer[256];
        wsprintf(buffer, "OpenGL version: %s\n", version);
        OutputDebugString(buffer);
    }
    else {
        //FAIL
    }
    ReleaseDC(window, windowDC);
}


GLuint OpenGLCreateShaderProgram(char* HeaderCode, char* VertexCode, char* FragmentCode) {
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLchar* vertexShaderCode[] = {
        HeaderCode,
        VertexCode
    };
    glShaderSource(vertexShaderID, 2, vertexShaderCode, 0);
    glCompileShader(vertexShaderID);

    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    GLchar* fragmentShaderCode[] = {
        HeaderCode,
        FragmentCode
    };
    glShaderSource(fragmentShaderID, 2, fragmentShaderCode, 0);
    glCompileShader(fragmentShaderID);

    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    glValidateProgram(programID);
    GLint valid = false;
    glGetProgramiv(programID, GL_LINK_STATUS, &valid);

    if (!valid) {
        GLsizei length;
        char vertexShaderErrors[4096];
        char fragmentShaderErrors[4096];
        char programShaderErrors[4096];
        glGetShaderInfoLog(vertexShaderID, sizeof(vertexShaderErrors), &length, vertexShaderErrors);
        glGetShaderInfoLog(fragmentShaderID, sizeof(fragmentShaderErrors), &length, fragmentShaderErrors);
        glGetProgramInfoLog(programID, sizeof(programShaderErrors), &length, programShaderErrors);

        OutputDebugString("VERTEX/FRAGMENT SHADER ERROR!\n");
        OutputDebugString(vertexShaderErrors);
        OutputDebugString("\n");
        OutputDebugString(fragmentShaderErrors);
        OutputDebugString("\n");
        OutputDebugString(programShaderErrors);
    }
    return programID;
}

GLuint OpenGLCreateComputeShader(char* computeShaderCode) {
    GLuint computeShaderID = glCreateShader(GL_COMPUTE_SHADER);
    GLchar* codeArray[] = {
        computeShaderCode
    };
    glShaderSource(computeShaderID, 1, codeArray, 0);
    glCompileShader(computeShaderID);

    GLuint programID = glCreateProgram();
    glAttachShader(programID, computeShaderID);
    glLinkProgram(programID); //maybe dont need this

    glValidateProgram(programID);

    GLint valid = false;
    glGetProgramiv(programID, GL_LINK_STATUS, &valid);

    if (!valid) {
        GLsizei length;
        char computeShaderErrors[4096];
        char programShaderErrors[4096];
        glGetShaderInfoLog(computeShaderID, sizeof(computeShaderErrors), &length, computeShaderErrors);
        glGetProgramInfoLog(programID, sizeof(programShaderErrors), &length, programShaderErrors);

        OutputDebugString("COMPUTE SHADER ERROR!\n");
        OutputDebugString(computeShaderErrors);
        OutputDebugString("\n");
        OutputDebugString(programShaderErrors);
    }

    return programID;
}