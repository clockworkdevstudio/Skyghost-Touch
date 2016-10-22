/*

Copyright (c) 2016, Clockwork Dev Studio
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met: 

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#include "SDL_opengles2.h"
#define GLM_FORCE_RADIANS
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include "android/log.h"
#include "multimedia.h"
#include <stdlib.h>
#include <list>

#define GRAPHICS_TYPE_LINE 1
#define GRAPHICS_TYPE_RECT 2
#define GRAPHICS_TYPE_RECT_HOLLOW 3
#define GRAPHICS_TYPE_OVAL 4
#define GRAPHICS_TYPE_OVAL_HOLLOW 5

extern "C"
{

  void load_primitive_program ();
  void load_image_program ();
  GLuint load_shader (GLenum type, const GLchar ** shaderCode, int num_lines);

  SDL_Window *WINDOW;
  SDL_GLContext GL_CONTEXT;
  int BB_GRAPHICS_WIDTH;
  int BB_GRAPHICS_HEIGHT;

  using namespace std;

  int BB_COLOR_RED;
  int BB_COLOR_GREEN;
  int BB_COLOR_BLUE;
  int BB_CLS_COLOR_RED;
  int BB_CLS_COLOR_GREEN;
  int BB_CLS_COLOR_BLUE;
  int BB_CLS_COLOR_ALPHA;
  int BB_ORIGIN_X;
  int BB_ORIGIN_Y;
  int BB_AUTOMIDHANDLE;
  int BB_PRIMITIVE_HANDLE_X;
  int BB_PRIMITIVE_HANDLE_Y;

  TTF_Font *BB_CURRENT_FONT;

  double BB_SCALE;
  double BB_ORIENTATION;
  double BB_ALPHA;
  double BB_SCALE_X;
  double BB_SCALE_Y;
  double BB_PI;

  GLuint PRIMITIVE_PROGRAM;
  GLuint IMAGE_PROGRAM;

  int init_multimedia ()
  {
    SDL_Init (SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    Mix_Init (MIX_INIT_OGG);
    Mix_OpenAudio (22050, MIX_DEFAULT_FORMAT, 2, 1024);
    Mix_AllocateChannels (32);

    IMG_Init (IMG_INIT_PNG);

    TTF_Init ();

    WINDOW =
      SDL_CreateWindow ("Skyghost", SDL_WINDOWPOS_UNDEFINED,
                        SDL_WINDOWPOS_UNDEFINED, 0, 0,
                        SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);
    GL_CONTEXT = SDL_GL_CreateContext (WINDOW);
    SDL_GL_GetDrawableSize (WINDOW, &BB_GRAPHICS_WIDTH, &BB_GRAPHICS_HEIGHT);
    glViewport (0, 0, BB_GRAPHICS_WIDTH, BB_GRAPHICS_HEIGHT);
    glClearColor (0, 0, 0, 1);
    SDL_GL_SetSwapInterval (0);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);

    BB_CLS_COLOR_RED = 0;
    BB_CLS_COLOR_GREEN = 0;
    BB_CLS_COLOR_BLUE = 0;
    BB_CLS_COLOR_ALPHA = 255;

    BB_COLOR_RED = 255;
    BB_COLOR_GREEN = 255;
    BB_COLOR_BLUE = 255;

    BB_ORIGIN_X = 0;
    BB_ORIGIN_Y = 0;

    BB_SCALE_X = 1.0f;
    BB_SCALE_Y = 1.0f;

    BB_AUTOMIDHANDLE = 0;

    BB_PRIMITIVE_HANDLE_X = 0;
    BB_PRIMITIVE_HANDLE_Y = 0;

    BB_ORIENTATION = 0.0f;
    BB_ALPHA = 1.0f;

    BB_PI = 3.14159265358979323;

    load_primitive_program ();
    load_image_program ();
    return 0;
  }
  void load_primitive_program ()
  {

    GLuint linked;
    GLsizei log_length;
    GLchar log[256];
    GLenum error;
    int vertex_shader_size;
    int fragment_shader_size;

    const GLchar *vertex_shader_code[] = { "attribute vec4 vPosition;",
      "void main() {",
      "  gl_Position = vPosition;",
      "}", NULL
    };

    const GLchar *fragment_shader_code[] = { "precision mediump float;",
      "uniform vec4 vColor;",
      "void main() {",
      "  gl_FragColor = vColor;",
      "}", NULL
    };

    vertex_shader_size = 0;
    while (vertex_shader_code[vertex_shader_size])
      vertex_shader_size++;
    fragment_shader_size = 0;
    while (fragment_shader_code[fragment_shader_size])
      fragment_shader_size++;

    GLuint vertex_shader = load_shader (GL_VERTEX_SHADER, vertex_shader_code,
                                        vertex_shader_size);
    GLuint fragment_shader =
      load_shader (GL_FRAGMENT_SHADER, fragment_shader_code,
                   fragment_shader_size);

    PRIMITIVE_PROGRAM = glCreateProgram ();
    glAttachShader (PRIMITIVE_PROGRAM, vertex_shader);
    glAttachShader (PRIMITIVE_PROGRAM, fragment_shader);
    glLinkProgram (PRIMITIVE_PROGRAM);
    glGetProgramiv (PRIMITIVE_PROGRAM, GL_LINK_STATUS, (GLint *) & linked);

    glGetProgramInfoLog (PRIMITIVE_PROGRAM, 255, &log_length, log);

  }

  void load_image_program ()
  {
    GLuint linked;
    GLsizei log_length;
    GLchar log[256];
    GLenum error;
    int vertex_shader_size;
    int fragment_shader_size;

    const GLchar *vertex_shader_code[] = { "attribute vec4 vPosition;",
      "uniform mat4 uMVPMatrix;",
      "varying vec2 textureCoordinates;",
      "void main() {",
      "  gl_Position = vPosition * uMVPMatrix;",
      "  textureCoordinates = vec2(-vPosition) * vec2(0.5) + vec2(0.5);",
      "}", NULL
    };

    const GLchar *fragment_shader_code[] = { "precision mediump float;",
      "uniform sampler2D texture_;",
      "uniform float x1;",
      "uniform float y1;",
      "uniform float x2;",
      "uniform float y2;",
      "uniform float a;",
      "varying vec2 textureCoordinates;",
      "void main() {",
      "  if(1.0 - textureCoordinates.x >= x1 && 1.0 - textureCoordinates.x <= x2 && textureCoordinates.y >= y1 && textureCoordinates.y <= y2)",
      "  {",
      "    vec4 k = texture2D(texture_,vec2(1.0 - textureCoordinates.x,textureCoordinates.y));",
      "    if(k.a != 0.0)",
      "      k.a = a;",
      "    gl_FragColor = k;",
      "  }",
      "  else",
      "  {",
      "    gl_FragColor = vec4(0.0,0.0,0.0,0.0);",
      "  }",
      "}", NULL
    };

    vertex_shader_size = 0;
    while (vertex_shader_code[vertex_shader_size])
      vertex_shader_size++;
    fragment_shader_size = 0;
    while (fragment_shader_code[fragment_shader_size])
      fragment_shader_size++;

    GLuint vertex_shader = load_shader (GL_VERTEX_SHADER, vertex_shader_code,
                                        vertex_shader_size);

    char infoLog[256];
    GLsizei length_;
    glGetShaderInfoLog (vertex_shader, 256, &length_, infoLog);

    GLuint fragment_shader =
      load_shader (GL_FRAGMENT_SHADER, fragment_shader_code,
                   fragment_shader_size);

    IMAGE_PROGRAM = glCreateProgram ();
    glAttachShader (IMAGE_PROGRAM, vertex_shader);
    glAttachShader (IMAGE_PROGRAM, fragment_shader);
    glLinkProgram (IMAGE_PROGRAM);
    glGetProgramiv (IMAGE_PROGRAM, GL_LINK_STATUS, (GLint *) & linked);

    glGetProgramInfoLog (IMAGE_PROGRAM, 255, &log_length, log);

  }

  GLuint load_shader (GLenum type, const GLchar ** shaderCode, int num_lines)
  {

    GLuint shader;
    GLenum error;
    GLint compiled;
    GLsizei log_length;
    GLchar log[256];

    shader = glCreateShader (type);

    glShaderSource (shader, num_lines, shaderCode, NULL);

    glCompileShader (shader);
    glGetShaderiv (shader, GL_COMPILE_STATUS, &compiled);
    glGetShaderInfoLog (shader, 256, &log_length, log);
    return shader;
  }

  unsigned int bb_color (unsigned int red, unsigned int green,
                         unsigned int blue, unsigned int alpha)
  {
    BB_COLOR_RED = red;
    BB_COLOR_GREEN = green;
    BB_COLOR_BLUE = blue;
    BB_ALPHA = double (alpha) / 255.0;
    return 0;
  }

  unsigned int bb_clscolor (unsigned int red, unsigned int green,
                            unsigned int blue, unsigned int alpha)
  {
    BB_CLS_COLOR_RED = red;
    BB_CLS_COLOR_GREEN = green;
    BB_CLS_COLOR_BLUE = blue;
    BB_CLS_COLOR_ALPHA = alpha;
    glClearColor (BB_CLS_COLOR_RED / 255.0f,
                  BB_CLS_COLOR_GREEN / 255.0f,
                  BB_CLS_COLOR_BLUE / 255.0f, BB_CLS_COLOR_ALPHA / 255.0f);
    return 0;
  }

  unsigned int bb_cls ()
  {
    glClear (GL_COLOR_BUFFER_BIT);
    return 0;
  }

  unsigned int bb_line (int x1, int y1, int x2, int y2)
  {
    GLfloat x1_adjusted, y1_adjusted, x2_adjusted, y2_adjusted;
    GLuint matrix_handle;
    GLint position_handle;
    GLuint color_handle;
    GLushort *indices;
    GLuint error;
    GLfloat *vertex_data;
    GLfloat *color_data;
    GLuint vertex_buffer;
    GLuint index_buffer;
    GLuint color_buffer;
    int i, j;

    x1_adjusted = (x1 + BB_ORIGIN_X);
    y1_adjusted = (BB_GRAPHICS_HEIGHT - y1 - BB_ORIGIN_Y);
    x2_adjusted = (x2 + BB_ORIGIN_X) - x1_adjusted;
    y2_adjusted = (BB_GRAPHICS_HEIGHT - y2 - BB_ORIGIN_Y) - y1_adjusted;

    glm::vec4 vertices[2];
    glm::vec4 colors (BB_COLOR_RED / 255.0, BB_COLOR_GREEN / 255.0,
                      BB_COLOR_BLUE / 255.0, BB_ALPHA);

    glm::mat4 matrix;

    glm::mat4 translationMatrix (1.0f, 0.0f, 0.0f,
                                 -(0.5 * BB_GRAPHICS_WIDTH), 0.0f,
                                 1.0f, 0.0f,
                                 -(0.5 * BB_GRAPHICS_HEIGHT), 0.0f,
                                 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    glm::mat4 scaleMatrix (2.0f / BB_GRAPHICS_WIDTH, 0.0f, 0.0f,
                           0.0f, 0.0f, 2.0f / BB_GRAPHICS_HEIGHT,
                           0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 1.0f);

    matrix = translationMatrix * scaleMatrix;

    vertices[0] = glm::vec4 (x1_adjusted, y1_adjusted, 0.0f, 1.0f) * matrix;
    vertices[1] =
      glm::vec4 (x1_adjusted + x2_adjusted,
                 y1_adjusted + y2_adjusted, 0.0f, 1.0f) * matrix;

    int size = 2 * 4 * sizeof (GLfloat);
    indices = (GLushort *) malloc (2 * sizeof (GLushort));

    for (i = 0; i < 2; i++)
      indices[i] = i;

    vertex_data = (GLfloat *) malloc (2 * size);

    for (j = 0; j < 2; j++)
      {
        memcpy (vertex_data + j * sizeof (GLfloat),
                glm::value_ptr (vertices[j]), 4 * sizeof (GLfloat));
      }

    color_data = (GLfloat *) malloc (2 * size);

    for (j = 0; j < 2; j++)
      {
        memcpy (color_data + j * sizeof (GLfloat),
                glm::value_ptr (colors), 4 * sizeof (GLfloat));
      }

    glUseProgram (PRIMITIVE_PROGRAM);

    position_handle = glGetAttribLocation (PRIMITIVE_PROGRAM, "vPosition");
    glEnableVertexAttribArray (position_handle);
    glVertexAttribPointer (position_handle, 4, GL_FLOAT, 0, 16, vertex_data);

    color_handle = glGetUniformLocation (PRIMITIVE_PROGRAM, "vColor");

    glUniform4fv (color_handle, 1, color_data);
    glDrawArrays (GL_LINES, 0, 2);

    glDisableVertexAttribArray (position_handle);

    free (color_data);
    free (vertex_data);

    return 0;

  }

  unsigned int bb_oval (int x, int y, int width, int height,
                        unsigned int filled)
  {
    if (!filled)
      return bb_oval_hollow (x, y, width, height);
    else
      return bb_oval_filled (x, y, width, height);
  }

  unsigned int bb_oval_filled (int x, int y, int width, int height)
  {

    int num_vertices;
    GLuint vertex_buffer;
    GLuint index_buffer;
    GLint matrix_handle;
    GLint position_handle;
    GLuint color_handle;
    GLushort *indices;
    GLfloat *vertex_data;
    GLfloat *color_data;

    int i, j;

    glm::vec3 axis (0.0f, 0.0f, -1.0f);
    glm::mat4 rotation_matrix =
      glm::rotate (((float) BB_ORIENTATION) / 360.0f * 2.0f *
                   ((float) BB_PI), axis);
    glm::mat4 matrix;
    glm::vec4 * vertices;
    glm::vec4 colors (BB_COLOR_RED / 255.0, BB_COLOR_GREEN / 255.0,
                      BB_COLOR_BLUE / 255.0, BB_ALPHA);

    float offset_by_x = (((float) x) / BB_GRAPHICS_WIDTH) * 2.0f;
    float offset_by_y = (((float) y) / BB_GRAPHICS_HEIGHT) * 2.0f;

    float offset_by_handle_x =
      ((0.5f * (width * BB_SCALE_X) -
        BB_PRIMITIVE_HANDLE_X) / (BB_GRAPHICS_WIDTH)) * 2.0f;
    float offset_by_handle_y =
      ((0.5f * (height * BB_SCALE_Y) -
        BB_PRIMITIVE_HANDLE_Y) / (BB_GRAPHICS_HEIGHT)) * 2.0f;

    float offset_by_origin_x =
      (((float) BB_ORIGIN_X) / BB_GRAPHICS_WIDTH) * 2.0f;
    float offset_by_origin_y =
      (((float) BB_ORIGIN_Y) / BB_GRAPHICS_HEIGHT) * 2.0f;

    glm::mat4 translation_matrix (1.0f, 0.0f, 0.0f,
                                  -1.0f + offset_by_x +
                                  offset_by_handle_x +
                                  offset_by_origin_x, 0.0f, 1.0f,
                                  0.0f,
                                  1.0f - offset_by_y -
                                  offset_by_handle_y -
                                  offset_by_origin_y, 0.0f, 0.0f,
                                  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    float major_axis = width > height ? width : height;
    float minor_axis = width > height ? height : width;
    int perimeter =
      0.09 * 2.0f * BB_PI *
      sqrt ((major_axis * major_axis + minor_axis * minor_axis) / 2.0);
    if (perimeter < 3)
      perimeter = 3;
    if (perimeter > 78)
      perimeter = 78;
    float interval = (2.0f * BB_PI) / perimeter;

    glm::mat4 scale_matrix1 (0.5 * float (width) * BB_SCALE_X, 0.0f,
                             0.0f, 0.0f, 0.0f,
                             0.5 * float (height) * BB_SCALE_Y, 0.0f,
                             0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 1.0f);

    glm::mat4 scale_matrix2 ((2.0f / BB_GRAPHICS_WIDTH), 0.0f, 0.0f,
                             0.0f, 0.0f, (2.0f / BB_GRAPHICS_HEIGHT),
                             0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                             0.0f, 0.0f, 0.0f, 1.0f);

    matrix =
      scale_matrix1 * rotation_matrix * scale_matrix2 * translation_matrix;

    num_vertices = perimeter * 3 + 3;
    vertices = new glm::vec4[num_vertices];

    for (i = 0; i < perimeter; i++)
      {
        j = i * 3;
        vertices[j] =
          glm::vec4 (cos (interval * i), sin (interval * i),
                     0.0f, 1.0f) * matrix;
        vertices[j + 1] =
          glm::vec4 (cos (interval * (i + 1)),
                     sin (interval * (i + 1)), 0.0f, 1.0f) * matrix;
        vertices[j + 2] = glm::vec4 (0.0f, 0.0f, 0.0f, 1.0f) * matrix;
      }

    indices = (GLushort *) malloc (num_vertices * sizeof (GLushort));

    for (i = 0; i < num_vertices; i++)
      {
        indices[i] = i;
      }

    vertex_data = (GLfloat *) malloc (num_vertices * 4 * sizeof (GLfloat));

    for (j = 0; j < num_vertices; j++)
      {
        memcpy (vertex_data + j * 4, glm::value_ptr (vertices[j]),
                4 * sizeof (GLfloat));
      }

    color_data = (GLfloat *) malloc (num_vertices * 4 * sizeof (GLfloat));

    for (j = 0; j < num_vertices; j++)
      {
        memcpy (color_data + j * 4, glm::value_ptr (colors),
                4 * sizeof (GLfloat));
      }

    glUseProgram (PRIMITIVE_PROGRAM);

    position_handle = glGetAttribLocation (PRIMITIVE_PROGRAM, "vPosition");
    glEnableVertexAttribArray (position_handle);
    glVertexAttribPointer (position_handle, 4, GL_FLOAT, 0, 16, vertex_data);
    GLuint err;

    err = glGetError ();

    color_handle = glGetUniformLocation (PRIMITIVE_PROGRAM, "vColor");

    glUniform4fv (color_handle, 1, color_data);

    glDrawArrays (GL_TRIANGLES, 0, num_vertices);

    glDisableVertexAttribArray (position_handle);

    free (color_data);
    free (vertex_data);
    return 0;

  }

  unsigned int bb_oval_hollow (int x, int y, int width, int height)
  {
    int num_vertices;
    GLuint vertex_buffer;
    GLuint index_buffer;
    GLint matrix_handle;
    GLint position_handle;
    GLuint color_handle;
    GLushort *indices;
    GLfloat *vertex_data;
    GLfloat *color_data;

    int i, j;

    glm::vec3 axis (0.0f, 0.0f, -1.0f);
    glm::mat4 rotation_matrix =
      glm::rotate (((float) BB_ORIENTATION) / 360.0f * 2.0f *
                   ((float) BB_PI), axis);
    glm::mat4 matrix;
    glm::vec4 * vertices;
    glm::vec4 colors (BB_COLOR_RED / 255.0, BB_COLOR_GREEN / 255.0,
                      BB_COLOR_BLUE / 255.0, BB_ALPHA);

    float offset_by_x = (((float) x) / BB_GRAPHICS_WIDTH) * 2.0f;
    float offset_by_y = (((float) y) / BB_GRAPHICS_HEIGHT) * 2.0f;

    float offset_by_handle_x =
      ((0.5f * (width * BB_SCALE_X) -
        BB_PRIMITIVE_HANDLE_X) / (BB_GRAPHICS_WIDTH)) * 2.0f;
    float offset_by_handle_y =
      ((0.5f * (height * BB_SCALE_Y) -
        BB_PRIMITIVE_HANDLE_Y) / (BB_GRAPHICS_HEIGHT)) * 2.0f;

    float offset_by_origin_x =
      (((float) BB_ORIGIN_X) / BB_GRAPHICS_WIDTH) * 2.0f;
    float offset_by_origin_y =
      (((float) BB_ORIGIN_Y) / BB_GRAPHICS_HEIGHT) * 2.0f;

    glm::mat4 translation_matrix (1.0f, 0.0f, 0.0f,
                                  -1.0f + offset_by_x +
                                  offset_by_handle_x +
                                  offset_by_origin_x, 0.0f, 1.0f,
                                  0.0f,
                                  1.0f - offset_by_y -
                                  offset_by_handle_y -
                                  offset_by_origin_y, 0.0f, 0.0f,
                                  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    float major_axis = width > height ? width : height;
    float minor_axis = width > height ? height : width;
    int perimeter =
      0.09 * 2.0f * BB_PI *
      sqrt ((major_axis * major_axis + minor_axis * minor_axis) / 2.0);
    if (perimeter < 3)
      perimeter = 3;
    if (perimeter > 78)
      perimeter = 78;
    float interval = (2.0f * BB_PI) / perimeter;

    glm::mat4 scale_matrix1 (0.5 * float (width) * BB_SCALE_X, 0.0f,
                             0.0f, 0.0f, 0.0f,
                             0.5 * float (height) * BB_SCALE_Y, 0.0f,
                             0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 1.0f);

    glm::mat4 scale_matrix2 ((2.0f / BB_GRAPHICS_WIDTH), 0.0f, 0.0f,
                             0.0f, 0.0f, (2.0f / BB_GRAPHICS_HEIGHT),
                             0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                             0.0f, 0.0f, 0.0f, 1.0f);

    matrix =
      scale_matrix1 * rotation_matrix * scale_matrix2 * translation_matrix;

    num_vertices = perimeter * 2 + 2;
    vertices = new glm::vec4[num_vertices];

    for (i = 0; i < perimeter; i++)
      {
        j = i * 2;
        vertices[j] =
          glm::vec4 (cos (interval * i), sin (interval * i),
                     0.0f, 1.0f) * matrix;
        vertices[j + 1] =
          glm::vec4 (cos (interval * (i + 1)),
                     sin (interval * (i + 1)), 0.0f, 1.0f) * matrix;
      }

    indices = (GLushort *) malloc (num_vertices * sizeof (GLushort));

    for (i = 0; i < num_vertices; i++)
      {
        indices[i] = i;
      }

    vertex_data = (GLfloat *) malloc (num_vertices * 4 * sizeof (GLfloat));

    for (j = 0; j < num_vertices; j++)
      {
        memcpy (vertex_data + j * 4, glm::value_ptr (vertices[j]),
                4 * sizeof (GLfloat));
      }

    color_data = (GLfloat *) malloc (num_vertices * 4 * sizeof (GLfloat));

    for (j = 0; j < num_vertices; j++)
      {
        memcpy (color_data + j * 4, glm::value_ptr (colors),
                4 * sizeof (GLfloat));
      }

    glUseProgram (PRIMITIVE_PROGRAM);

    position_handle = glGetAttribLocation (PRIMITIVE_PROGRAM, "vPosition");
    glEnableVertexAttribArray (position_handle);
    glVertexAttribPointer (position_handle, 4, GL_FLOAT, 0, 16, vertex_data);
    GLuint err;

    err = glGetError ();

    color_handle = glGetUniformLocation (PRIMITIVE_PROGRAM, "vColor");

    glUniform4fv (color_handle, 1, color_data);

    glDrawArrays (GL_LINES, 0, num_vertices);

    glDisableVertexAttribArray (position_handle);

    free (color_data);
    free (vertex_data);
    free (indices);

    return 0;

  }

  unsigned int bb_drawimage (unsigned int image_handle, int x, int y,
                             unsigned int frame)
  {
    GLuint error;
    GLuint vertex_buffer;
    GLuint index_buffer;
    GLint matrix_handle;
    GLfloat x1, y1, x2, y2, a;
    GLint x1_handle;
    GLint y1_handle;
    GLint x2_handle;
    GLint y2_handle;
    GLint a_handle;
    GLint texture_handle;
    GLint position_handle;
    Image *image = (Image *) image_handle;

    GLfloat vertices[] = { -1.0f, -1.0f, 0.0f, 1.0f,
      1.0f, -1.0f, 0.0f, 1.0f,
      -1.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 1.0f, 0.0f, 1.0f
    };

    GLushort indices[] = { 0, 1, 2, 3 };

    glm::vec3 axis (0.0f, 0.0f, -1.0f);
    glm::mat4 rotation_matrix =
      glm::rotate (((float) BB_ORIENTATION) / 360.0f * 2.0f *
                   ((float) BB_PI), axis);
    glm::mat4 matrix;

    float offset_by_x = (((float) x) / BB_GRAPHICS_WIDTH) * 2.0f;
    float offset_by_y = (((float) y) / BB_GRAPHICS_HEIGHT) * 2.0f;

    float offset_by_handle_x =
      ((0.5f * (image->width * BB_SCALE_X) -
        (image->handle_x * BB_SCALE_X)) / (BB_GRAPHICS_WIDTH)) * 2.0f;
    float offset_by_handle_y =
      ((0.5f * (image->height * BB_SCALE_Y) -
        (image->handle_y * BB_SCALE_Y)) / (BB_GRAPHICS_HEIGHT)) * 2.0f;

    float offset_by_origin_x =
      (((float) BB_ORIGIN_X) / BB_GRAPHICS_WIDTH) * 2.0f;
    float offset_by_origin_y =
      (((float) BB_ORIGIN_Y) / BB_GRAPHICS_HEIGHT) * 2.0f;

    glm::mat4 translation_matrix (1.0f, 0.0f, 0.0f,
                                  -1.0f + offset_by_x +
                                  offset_by_handle_x +
                                  offset_by_origin_x, 0.0f, 1.0f,
                                  0.0f,
                                  1.0f - offset_by_y -
                                  offset_by_handle_y -
                                  offset_by_origin_y, 0.0f, 0.0f,
                                  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    glm::mat4 scale_matrix1 (0.5 * float (image->width) * BB_SCALE_X,
                             0.0f, 0.0f, 0.0f, 0.0f,
                             0.5 * float (image->height) *
                             BB_SCALE_Y, 0.0f, 0.0f, 0.0f, 0.0f,
                             1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    glm::mat4 scale_matrix2 ((2.0f / BB_GRAPHICS_WIDTH), 0.0f, 0.0f,
                             0.0f, 0.0f, (2.0f / BB_GRAPHICS_HEIGHT),
                             0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                             0.0f, 0.0f, 0.0f, 1.0f);

    matrix =
      scale_matrix1 * rotation_matrix * scale_matrix2 * translation_matrix;

    glGenBuffers (1, &vertex_buffer);
    glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices,
                  GL_STATIC_DRAW);

    glGenBuffers (1, &index_buffer);

    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (indices), indices,
                  GL_STATIC_DRAW);
    error = glGetError ();

    glUseProgram (IMAGE_PROGRAM);
    error = glGetError ();

    matrix_handle = glGetUniformLocation (IMAGE_PROGRAM, "uMVPMatrix");
    error = glGetError ();

    texture_handle = glGetUniformLocation (IMAGE_PROGRAM, "texture_");

    x1_handle = glGetUniformLocation (IMAGE_PROGRAM, "x1");
    y1_handle = glGetUniformLocation (IMAGE_PROGRAM, "y1");
    x2_handle = glGetUniformLocation (IMAGE_PROGRAM, "x2");
    y2_handle = glGetUniformLocation (IMAGE_PROGRAM, "y2");
    a_handle = glGetUniformLocation (IMAGE_PROGRAM, "a");
    position_handle = glGetAttribLocation (IMAGE_PROGRAM, "vPosition");

    glUniformMatrix4fv (matrix_handle, 1, 0, glm::value_ptr (matrix));

    x1 = 0.0f;
    y1 = 0.0f;
    x2 = 1.0f;
    y2 = 1.0f;
    a = BB_ALPHA;

    glUniform1f (x1_handle, x1);
    glUniform1f (y1_handle, y1);
    glUniform1f (x2_handle, x2);
    glUniform1f (y2_handle, y2);
    glUniform1f (a_handle, a);

    glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_2D, image->textures[frame]);
    glUniform1i (texture_handle, 0);

    glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer);
    glVertexAttribPointer (position_handle, 4, GL_FLOAT, 0,
                           sizeof (GLfloat) * 4, (void *) 0);
    glEnableVertexAttribArray (position_handle);

    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, index_buffer);

    glDrawArrays (GL_TRIANGLE_STRIP, 0, 4);
    error = glGetError ();

    glDeleteBuffers (1, &vertex_buffer);
    glDeleteBuffers (1, &index_buffer);
    glDisableVertexAttribArray (position_handle);

    return 0;
  }

  unsigned int bb_drawimagerect (unsigned int image_handle, int x, int y,
                                 int rect_x1, int rect_y1, int rect_width,
                                 int rect_height, unsigned int frame)
  {
    GLuint error;
    GLuint vertex_buffer;
    GLuint index_buffer;
    GLint matrix_handle;
    GLfloat x1, y1, x2, y2, a;
    GLint x1_handle;
    GLint y1_handle;
    GLint x2_handle;
    GLint y2_handle;
    GLint a_handle;
    GLint texture_handle;
    GLint position_handle;
    Image *image = (Image *) image_handle;

    GLfloat vertices[] = { -1.0f, -1.0f, 0.0f, 1.0f,
      1.0f, -1.0f, 0.0f, 1.0f,
      -1.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 1.0f, 0.0f, 1.0f
    };

    GLushort indices[] = { 0, 1, 2, 3 };

    glm::vec3 axis (0.0f, 0.0f, -1.0f);
    glm::mat4 rotation_matrix =
      glm::rotate (((float) BB_ORIENTATION) / 360.0f * 2.0f *
                   ((float) BB_PI), axis);
    glm::mat4 matrix;

    float offset_by_x = (((float) x) / BB_GRAPHICS_WIDTH) * 2.0f;
    float offset_by_y = (((float) y) / BB_GRAPHICS_HEIGHT) * 2.0f;

    float offset_by_handle_x =
      ((0.5f * (image->width * BB_SCALE_X) -
        (image->handle_x * BB_SCALE_X)) / (BB_GRAPHICS_WIDTH)) * 2.0f;
    float offset_by_handle_y =
      ((0.5f * (image->height * BB_SCALE_Y) -
        (image->handle_y * BB_SCALE_Y)) / (BB_GRAPHICS_HEIGHT)) * 2.0f;

    float offset_by_origin_x =
      (((float) BB_ORIGIN_X) / BB_GRAPHICS_WIDTH) * 2.0f;
    float offset_by_origin_y =
      (((float) BB_ORIGIN_Y) / BB_GRAPHICS_HEIGHT) * 2.0f;

    glm::mat4 translation_matrix (1.0f, 0.0f, 0.0f,
                                  -1.0f + offset_by_x +
                                  offset_by_handle_x +
                                  offset_by_origin_x, 0.0f, 1.0f,
                                  0.0f,
                                  1.0f - offset_by_y -
                                  offset_by_handle_y -
                                  offset_by_origin_y, 0.0f, 0.0f,
                                  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    glm::mat4 scale_matrix1 (0.5 * float (image->width) * BB_SCALE_X,
                             0.0f, 0.0f, 0.0f, 0.0f,
                             0.5 * float (image->height) *
                             BB_SCALE_Y, 0.0f, 0.0f, 0.0f, 0.0f,
                             1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    glm::mat4 scale_matrix2 ((2.0f / BB_GRAPHICS_WIDTH), 0.0f, 0.0f,
                             0.0f, 0.0f, (2.0f / BB_GRAPHICS_HEIGHT),
                             0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                             0.0f, 0.0f, 0.0f, 1.0f);

    matrix =
      scale_matrix1 * rotation_matrix * scale_matrix2 * translation_matrix;

    glGenBuffers (1, &vertex_buffer);
    glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices,
                  GL_STATIC_DRAW);

    glGenBuffers (1, &index_buffer);

    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (indices), indices,
                  GL_STATIC_DRAW);
    error = glGetError ();

    glUseProgram (IMAGE_PROGRAM);
    error = glGetError ();

    matrix_handle = glGetUniformLocation (IMAGE_PROGRAM, "uMVPMatrix");
    error = glGetError ();

    texture_handle = glGetUniformLocation (IMAGE_PROGRAM, "texture_");

    x1_handle = glGetUniformLocation (IMAGE_PROGRAM, "x1");
    y1_handle = glGetUniformLocation (IMAGE_PROGRAM, "y1");
    x2_handle = glGetUniformLocation (IMAGE_PROGRAM, "x2");
    y2_handle = glGetUniformLocation (IMAGE_PROGRAM, "y2");
    a_handle = glGetUniformLocation (IMAGE_PROGRAM, "a");
    position_handle = glGetAttribLocation (IMAGE_PROGRAM, "vPosition");

    glUniformMatrix4fv (matrix_handle, 1, 0, glm::value_ptr (matrix));

    x1 = float (rect_x1) / image->width;
    y1 = float (rect_y1) / image->height;
    x2 = float (rect_x1 + rect_width) / image->width;
    y2 = float (rect_y1 + rect_height) / image->height;
    a = BB_ALPHA;

    glUniform1f (x1_handle, x1);
    glUniform1f (y1_handle, y1);
    glUniform1f (x2_handle, x2);
    glUniform1f (y2_handle, y2);
    glUniform1f (a_handle, a);

    glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_2D, image->textures[frame]);
    glUniform1i (texture_handle, 0);

    glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer);
    glVertexAttribPointer (position_handle, 4, GL_FLOAT, 0,
                           sizeof (GLfloat) * 4, (void *) 0);
    glEnableVertexAttribArray (position_handle);

    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, index_buffer);

    glDrawArrays (GL_TRIANGLE_STRIP, 0, 4);
    error = glGetError ();

    glDeleteBuffers (1, &vertex_buffer);
    glDeleteBuffers (1, &index_buffer);
    glDisableVertexAttribArray (position_handle);

    return 0;
  }

  unsigned int mask_surface (SDL_Surface * surface, GLuint mask_color,
                             GLuint prev_mask_color)
  {

    int i;

    for (i = 0; i < (surface->w * surface->h); i++)
      {
        if ((((int *) surface->pixels)[i] & 0x00FFFFFF) == mask_color)
          {
            ((int *) surface->pixels)[i] &= 0x00FFFFFF;
          }
        else if ((((int *) surface->pixels)[i] & 0x00FFFFFF) ==
                 prev_mask_color)
          {
            ((int *) surface->pixels)[i] |= 0xFF000000;
          }
      }
    return 0;
  }

  unsigned int bb_loadimage (char const *file_name)
  {

    SDL_Surface *surface = IMG_Load (file_name);

    Image *image;
    GLuint texture;

    glGenTextures (1, &texture);
    glBindTexture (GL_TEXTURE_2D, texture);

    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    image = (Image *) malloc (sizeof (Image));
    image->textures = (GLuint *) malloc (sizeof (GLuint));
    image->textures[0] = texture;
    image->surface = surface;
    image->width = surface->w;
    image->height = surface->h;
    image->width_frames = 1;
    image->height_frames = 1;
    image->mask_color = 0;
    mask_surface (surface, 0, 0);
    image->masks = 0;

    image->mask_width = image->width / (8 * sizeof (unsigned int));
    image->mask_height = image->height;

    if (image->width % sizeof (unsigned int))
      image->mask_width++;

    if (image->height % sizeof (unsigned int))
      image->mask_height++;

    if (BB_AUTOMIDHANDLE)
      {
        image->handle_x = image->width / 2;
        image->handle_y = image->height / 2;
      }
    else
      {
        image->handle_x = 0;
        image->handle_y = 0;
      }

    return (unsigned int) image;
  }

  unsigned int bb_imagewidth (unsigned int image_handle)
  {
    Image *image = (Image *) image_handle;
    return image->width;
  }

  unsigned int bb_imageheight (unsigned int image_handle)
  {
    Image *image = (Image *) image_handle;
    return image->height;
  }

  unsigned int bb_loadfont (char const *font_name, unsigned int height,
                            unsigned int bold, unsigned int italic,
                            unsigned int underlined)
  {
    TTF_Font *font = TTF_OpenFont (font_name, height);
    return (unsigned int) font;
  }

  unsigned int bb_setfont (unsigned int font_handle)
  {
    BB_CURRENT_FONT = (TTF_Font *) font_handle;
    return 0;
  }

  unsigned int bb_text (int x, int y, char *text, unsigned int center_x,
                        unsigned int center_y)
  {
    SDL_Surface *surface;
    SDL_Color color;
    GLuint texture;
    Image *image;

    color.r = BB_COLOR_BLUE;
    color.g = BB_COLOR_GREEN;
    color.b = BB_COLOR_RED;
    color.a = BB_ALPHA;

    surface = TTF_RenderUTF8_Blended (BB_CURRENT_FONT, text, color);
    glGenTextures (1, &texture);
    glBindTexture (GL_TEXTURE_2D, texture);

    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    image = (Image *) malloc (sizeof (Image));
    image->textures = (GLuint *) malloc (sizeof (GLuint));
    image->textures[0] = texture;
    image->surface = surface;
    image->width = surface->w;
    image->height = surface->h;
    image->width_frames = 1;
    image->height_frames = 1;

    if (center_x)
      {
        image->handle_x = image->width / 2;
      }
    else
      {
        image->handle_x = 0;
      }

    if (center_y)
      {
        image->handle_y = image->height / 2;
      }
    else
      {
        image->handle_y = 0;
      }

    bb_drawimage ((unsigned int) image, x, y, 0);

    SDL_FreeSurface (surface);
    glDeleteTextures (1, &texture);
    free (image->textures);
    free (image);
    return 0;
  }

  unsigned int bb_stringwidth (char const *text)
  {
    int width;
    TTF_SizeUTF8 (BB_CURRENT_FONT, text, &width, NULL);
    return width;
  }

  unsigned int bb_stringheight (char const *text)
  {
    int height;
    TTF_SizeUTF8 (BB_CURRENT_FONT, text, NULL, &height);
    return height;
  }

  void bb_setscale (double x, double y)
  {
    BB_SCALE_X = x;
    BB_SCALE_Y = y;
  }

  double bb_getscalex ()
  {
    return BB_SCALE_X;
  }

  double bb_getscaley ()
  {
    return BB_SCALE_Y;
  }

  void bb_setalpha (double a)
  {
    BB_ALPHA = a;
  }

  double bb_getalpha ()
  {
    return BB_ALPHA;
  }

  unsigned int bb_flip (SDL_Window * WINDOW)
  {
    SDL_GL_SwapWindow (WINDOW);
    return 0;
  }

  unsigned int bb_loadsound (char const *name)
  {
    Mix_Chunk *sample;
    Sound *sound;
    sample = (Mix_Chunk *) Mix_LoadWAV (name);

    sound = (Sound *) malloc (sizeof (Sound));
    sound->pitch = 1;
    sound->volume = 0.5f;
    sound->pan = 0.0f;
    sound->loop = 0;
    sound->sample = sample;
    return (unsigned int) sound;
  }

  unsigned int bb_loopsound (unsigned int sound_handle)
  {
    Sound *sound;
    sound = (Sound *) sound_handle;
    sound->loop = -1;
    return 0;
  }

  int bb_playsound (unsigned int sound_handle)
  {
    Sound *sound;
    sound = (Sound *) sound_handle;
    int result;
    result = Mix_PlayChannel (-1, sound->sample, sound->loop);
    return (unsigned int) result;
  }

  unsigned int bb_stopchannel (int channel)
  {
    Mix_HaltChannel (channel);
    return 0;
  }

  unsigned int bb_channelplaying (int channel)
  {
    if (Mix_Playing (channel))
      return 1;
    else
      return 0;
  }

  unsigned int bb_createtimer (unsigned int frequency)
  {
    Timer *timer = (Timer *) malloc (sizeof (Timer));
    timer->frequency = frequency;
    timer->time = SDL_GetTicks ();
    return (unsigned int) timer;
  }

  unsigned int bb_waittimer (unsigned int timer_handle)
  {
    Timer *timer = (Timer *) timer_handle;
    int elapsed;
    int pings;
    int remainder;

    do
      {
        elapsed = SDL_GetTicks () - timer->time;
        if (!(elapsed < (1000.0f / timer->frequency)))
          break;
        else
          SDL_Delay (0);
      }
    while (1);

    pings = elapsed / ((int) (1000.0f / timer->frequency));
    remainder = elapsed % ((int) (1000.0f / timer->frequency));
    timer->time += pings * ((int) (1000.0f / timer->frequency)) - remainder;
    return pings;
  }

  unsigned int bb_timerpings (unsigned int timer_handle)
  {
    Timer *timer = (Timer *) timer_handle;
    int elapsed;
    int pings;
    int remainder;

    elapsed = SDL_GetTicks () - timer->time;
    pings = elapsed / ((int) (1000.0f / timer->frequency));
    remainder = elapsed % ((int) (1000.0f / timer->frequency));
    return pings;
  }

  void bb_resettimer (unsigned int timer_handle)
  {
    Timer *timer = (Timer *) timer_handle;
    int elapsed;
    int pings;
    int remainder;

    elapsed = SDL_GetTicks () - timer->time;
    pings = elapsed / ((int) (1000.0f / timer->frequency));
    remainder = elapsed % ((int) (1000.0f / timer->frequency));
    timer->time += pings * ((int) (1000.0f / timer->frequency));
  }

  void bb_resumetimer (unsigned int timer_handle)
  {
    Timer *timer = (Timer *) timer_handle;
    timer->time += SDL_GetTicks () - timer->time;
  }

  double bb_timerdelta (unsigned int timer_handle)
  {
    Timer *timer = (Timer *) timer_handle;
    int elapsed;
    int pings;
    int remainder;

    elapsed = SDL_GetTicks () - timer->time;
    pings = elapsed / ((int) (1000.0f / timer->frequency));
    remainder = elapsed % ((int) (1000.0f / timer->frequency));
    return double (remainder) / (1000.0f / timer->frequency);
  }

}
