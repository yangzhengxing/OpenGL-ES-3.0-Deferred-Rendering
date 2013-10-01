/*! @file program.c
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#include "program.h"
#include "gl_include.h"
#include "system.h"
#include "vertex.h"

/* Defines
 */

/* Types
 */

/* Constants
 */

/* Variables
 */

/* Internal functions
 */
static GLuint _load_shader(const char* filename, GLenum type)
{
    void*   data = NULL;
    size_t  data_size = 0;
    GLuint  shader = 0;
    GLint   compile_status = 0;
    int     result;
    GLint   shader_size = 0;

    result = (int)load_file_data(filename, &data, &data_size);
    if(result != 0) {
        system_log("Loading shader %s failed", filename);
        return 0;
    }
    assert(result == 0);
    shader_size = (GLint)data_size;

    shader = glCreateShader(type);
    glShaderSource(shader, 1, (const char**)&data, &shader_size);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
    if(compile_status == GL_FALSE) {
        char message[1024] = {0};
        glGetShaderInfoLog(shader, sizeof(message), 0, message);
        system_log("Error compiling %s: %s", filename, message);
        assert(compile_status != GL_FALSE);
        return 0;
    }
    CheckGLError();

    free_file_data(data);

    return shader;
}

/* External functions
 */
Program create_program(const char* vertex_shader_filename,
                       const char* fragment_shader_filename)
{
    GLuint  vertex_shader;
    GLuint  fragment_shader;
    GLuint  program;
    GLint   link_status;

    /* Compile shaders */
    vertex_shader = _load_shader(vertex_shader_filename, GL_VERTEX_SHADER);
    fragment_shader = _load_shader(fragment_shader_filename, GL_FRAGMENT_SHADER);

    /* Create program */
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glBindAttribLocation(program, kPositionSlot,    "a_Position");
    glBindAttribLocation(program, kNormalSlot,      "a_Normal");
    glBindAttribLocation(program, kTangentSlot,     "a_Tangent");
    glBindAttribLocation(program, kBitangentSlot,   "a_Bitangent");
    glBindAttribLocation(program, kTexCoordSlot,    "a_TexCoord");
    CheckGLError();

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    if(link_status == GL_FALSE) {
        char message[1024];
        glGetProgramInfoLog(program, sizeof(message), 0, message);
        system_log(message);
        assert(link_status != GL_FALSE);
    }
    glDetachShader(program, fragment_shader);
    glDetachShader(program, vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteShader(vertex_shader);
    CheckGLError();

    return program;
}

void destroy_program(Program program)
{
    glDeleteProgram(program);
}