#ifndef CUB_RENDER_OPEN_GL_H
#define CUB_RENDER_OPEN_GL_H

#include "types/open_gl.h"

/* Try to get the store the location of the uniform called 'uniform_name'
 * in the shader identified by 'shader_program'. It raises an error if
 * the uniform could not be found. 'fname' is the name of the invoking
 * function */
void try_get_uniform_location(GLuint shader_program, GLint* location,
                              const char* uniform_name, const char* fname);

void setup_render_buffer_object(RenderBufferObject* buffer_obj);

void free_render_buffer_object(RenderBufferObject* buffer_obj);

#endif
