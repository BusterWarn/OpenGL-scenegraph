
#ifndef _GLERRORUTIL_H
#define _GLERRORUTIL_H
#include <GL/glew.h>
#include <iostream>

namespace vr
{
  void printCompilationError(GLuint object);
}


// A simple OpenGL error checking routine. This compiles away
// to a no-op inline method if the NDEBUG preprocessor symbol
// is defined during compilation.
//
// - The first parameter (optional) is a string that can be 
//   used to indicate the location where the error check occurs.
//
// - The second parameter determines the destination of the error
//   message. It defaults to cerr, but could also be a file.
#ifndef NDEBUG
bool CheckErrorsGL( const std::string& location = "", 
				    std::ostream& ostr = std::cerr );
#else
inline bool CheckErrorsGL( const std::string& = "", 
						   std::ostream& /*ostr*/ = std::cerr ) 
{ return true; }
#endif 

#define DO_GL_AND_CHECK(A) A; CheckErrorsGL(#A)


#define CHECK_FRAMEBUFFER_STATUS() \
{\
  GLenum status; \
  status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT); \
  /*fprintf(stderr, "%x\n", status); */\
  switch(status) { \
  case GL_FRAMEBUFFER_COMPLETE_EXT: \
  /*fprintf(stderr,"framebuffer complete!\n"); */\
  break; \
  case GL_FRAMEBUFFER_UNSUPPORTED_EXT: \
  fprintf(stderr,"framebuffer GL_FRAMEBUFFER_UNSUPPORTED_EXT\n");\
  /* you gotta choose different formats */ \
  assert(0); \
  break; \
  case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT: \
  fprintf(stderr,"framebuffer INCOMPLETE_ATTACHMENT\n");\
  break; \
  case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT: \
  fprintf(stderr,"framebuffer FRAMEBUFFER_MISSING_ATTACHMENT\n");\
  break; \
  case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: \
  fprintf(stderr,"framebuffer FRAMEBUFFER_DIMENSIONS\n");\
  break; \
  case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: \
  fprintf(stderr,"framebuffer INCOMPLETE_FORMATS\n");\
  break; \
  case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT: \
  fprintf(stderr,"framebuffer INCOMPLETE_DRAW_BUFFER\n");\
  break; \
  case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT: \
  fprintf(stderr,"framebuffer INCOMPLETE_READ_BUFFER\n");\
  break; \
  case GL_FRAMEBUFFER_BINDING_EXT: \
  fprintf(stderr,"framebuffer BINDING_EXT\n");\
  break; \
  /* 
case GL_FRAMEBUFFER_STATUS_ERROR_EXT: \
fprintf(stderr,"framebuffer STATUS_ERROR\n");\
break; \
*/ \
  default: \
           /* programming error; will fail on all hardware */ \
           assert(0); \
}\
}



#endif
