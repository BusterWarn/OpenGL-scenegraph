#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glew.h>
#include <GL/glut.h>
#include <vr/shaderUtils.h>
#include <vr/FileSystem.h>

#include <fstream>

using namespace vr;

GLuint vr::loadShaderFromString(const std::string& source, GLuint shader_type, const std::string& name)
{
  // Create a vertex shader
  GLuint shader_id = glCreateShaderObjectARB(shader_type);
  CheckErrorsGL("glCreateShaderObjectARB");

  // Attach the source to the shader
  const char *cstr = source.c_str();
  glShaderSourceARB(shader_id, 1, &cstr, NULL);
  if (!CheckErrorsGL(std::string("glShaderSourceARB: ") + source))
    return 0;
  
  // Tell the driver to compile the shader source
  glCompileShaderARB(shader_id);
  if (!CheckErrorsGL("glCompileShaderARB"))
    return 0;

  GLint params;

  // Was the compilation successful?
  glGetObjectParameterivARB(shader_id, GL_OBJECT_COMPILE_STATUS_ARB,
    &params);

  if (params == GL_FALSE) {
    char log[256];
    GLsizei length;
    glGetInfoLogARB(shader_id,
      256,
      &length,
      log);
    log[length]='\0';
    std::cerr << "Error compiling shader \'" << name << "\'\n" << log << std::endl;
    return 0;
  }

  return shader_id;
}

GLuint vr::loadGSShader(GLint program, const std::string& source, GLint input, GLint output, GLint vertices)
{

  // Create a vertex shader
  GLuint shader_id = glCreateShaderObjectARB(GL_GEOMETRY_SHADER);
  CheckErrorsGL("glCreateShaderObjectARB");

  // Attach the source to the shader
  const char *cstr = source.c_str();
  glShaderSourceARB(shader_id, 1, &cstr, NULL);
  if (!CheckErrorsGL(std::string("glShaderSourceARB: ") + source))
    return 0;


  // Tell the driver to compile the shader source
  glCompileShaderARB(shader_id);
  if (!CheckErrorsGL("glCompileShaderARB"))
    return 0;

  glAttachShader(program, shader_id);
  glProgramParameteriEXT(program, GL_GEOMETRY_INPUT_TYPE_EXT, input);
  glProgramParameteriEXT(program, GL_GEOMETRY_OUTPUT_TYPE_EXT, output);
  glProgramParameteriEXT(program, GL_GEOMETRY_VERTICES_OUT_EXT, vertices);

  return shader_id;
}

GLuint vr::loadShader(const std::string& path, GLuint shader_type)
{
  std::string filepath = path;
  bool exist = vr::FileSystem::exists(filepath);

  std::string vrPath = vr::FileSystem::getEnv("VR_PATH");
  if (vrPath.empty())
    std::cerr << "The environment variable VR_PATH is not set. It should point to the directory where the vr library is (just above models)" << std::endl;

  if (!exist && !vrPath.empty())
  {
    filepath = std::string(vrPath) + "/" + path;
    exist = vr::FileSystem::exists(filepath);
  }

  if (!exist)
  {
    std::cerr << "The file " << path << " does not exist" << std::endl;
    return 0;
  }

  filepath = vr::FileSystem::convertToNativeFilePath(filepath);

  // Create a vertex shader
  GLuint shader_id = glCreateShaderObjectARB(shader_type);
  CheckErrorsGL(std::string("glCreateShaderObjectARB: ") + filepath);


  // Open the shader text file
  std::ifstream in;
  in.open(filepath.c_str(), std::ios::in);
  if (!in.is_open())
    return 0;

  std::string source;
  std::string line;

  while(!in.eof()) {
    std::getline(in, line);
    source += line;
    source += "\n";
  }

  GLuint id = loadShaderFromString( source, shader_type, path);
  if (!id)
  {
    std::cerr << "Error when compiling shader file: '" << filepath << "\'" << std::endl;
    return 0;
  }

  return id;
}

bool vr::checkLinkStatus( GLuint programID )
{
  GLint success;

  glGetObjectParameterivARB(programID, GL_OBJECT_LINK_STATUS_ARB, &success);
  if (!success) {
    char log[256];
    GLsizei length;
    glGetInfoLogARB(programID,
      255,
      &length,
      log);
    log[length]='\0';
    std::cerr << "Error linking program: " << log << std::endl;
    return false;
  }

  return true;
}

