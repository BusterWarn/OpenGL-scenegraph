#include <vr/glUtils.h>

#include <assimp/scene.h>
#include <SOIL.h>
#include <vr/FileSystem.h>
#include <algorithm>
#include <iostream>
#include <map>

typedef std::map<std::string, GLuint*> TextureMap;
TextureMap textureIdMap;	// map image filenames to textureIds
GLuint*		textureIds;

int vr::LoadGLTextures(const aiScene* scene)
{


  /* scan scene's materials for textures */
  for (unsigned int m = 0; m < scene->mNumMaterials; ++m)
  {
    int texIndex = 0;
    aiString path;  // filename

    aiReturn texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
    while (texFound == AI_SUCCESS) {
      //fill map with textures, OpenGL image ids set to 0
      textureIdMap[path.data] = 0;
      // more textures?
      texIndex++;
      texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
    }
  }

  size_t numTextures = textureIdMap.size();


  /* create and fill array with GL texture ids */
  GLuint* textureIds = new GLuint[numTextures];
  glGenTextures((GLsizei)numTextures, textureIds); /* Texture name generation */

                                          /* get iterator */
  TextureMap::iterator itr = textureIdMap.begin();
  int i = 0;
  for (; itr != textureIdMap.end(); ++i, ++itr)
  {
    //save IL image ID
    std::string filename = (*itr).first;  // get filename
    *((*itr).second) = textureIds[i];    // save texture id for filename in map

    int width;
    int height;
    unsigned char* image;

    image = SOIL_load_image(filename.c_str(), &width, &height, 0,
      SOIL_LOAD_RGBA);
    if (!image)
    {
      std::cerr << "Error loading image: " << filename << std::endl;
      return 0;
    }


    /* Create and load textures to OpenGL */
    glBindTexture(GL_TEXTURE_2D, textureIds[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width,
      height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
        image);
    SOIL_free_image_data(image);

  }
  

  delete[] textureIds;

  return 0;
}

// Function that loads a bmp/png file and creates and returns a 2d texture
GLuint vr::createTexture(const std::string& image_path)
{
	GLuint id=0;

  int width;
  int height;
  unsigned char* image;

  image = SOIL_load_image(image_path.c_str(), &width, &height, 0,
    SOIL_LOAD_RGBA);
  if (!image)
  {
    std::cerr << "Error loading image: " << image_path << std::endl;
    return 0;
  }


	// allocate a texture name
	glGenTextures(1, &id);

	// select the current texture
	glBindTexture(GL_TEXTURE_2D, id);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_NEAREST);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
		GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
		GL_REPEAT);

	// build our texture mipmaps
  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height,
		GL_RGBA, GL_UNSIGNED_BYTE, image);
  SOIL_free_image_data(image);

	glBindTexture(GL_TEXTURE_2D, 0);

	return id;
}
