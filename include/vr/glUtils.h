#ifndef __GL_UTILS_H__
#define __GL_UTILS_H__

#include <GL/glew.h>
#include <string>
struct aiScene;


namespace vr
{
  int LoadGLTextures(const aiScene* scene);

	/// Create a texture with some sample default settings and return a texture id. You will have to delete it!!
	GLuint createTexture(const std::string& image_path);
}

#endif
