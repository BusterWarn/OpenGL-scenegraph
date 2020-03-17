
#include <GL/glew.h>

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS 1
#include <windows.h>
#endif
#include <GL/glut.h>

#include <vr/DrawText.h>
#include <vr/FileSystem.h>
#include <vr/shaderUtils.h>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vr/glErrorUtil.h>


extern "C" {
#include <ft2build.h>
#include FT_FREETYPE_H
}

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#ifdef HAVE_FREETYPE

namespace
{
	typedef std::vector<std::string> StringVector;
	std::string findInPath(StringVector& paths, const std::string& filename)
	{
		for (StringVector::const_iterator it = paths.begin(); it != paths.end(); ++it)
		{
			std::string path = vr::FileSystem::convertToNativeFilePath(*it + "/" + filename);
			bool exist = vr::FileSystem::exists(path);
			if (exist)
				return path;

		}
		return "";
	}

	class Initializer
	{
	public:

		Initializer()
		{
			FT_Library ft;

			if (FT_Init_FreeType(&ft)) {
				std::cerr << "Error: Could not init freetype library" << std::endl;
				return;
			}

			StringVector fontPaths;

#if defined(WIN32)
			fontPaths.push_back(".");
			fontPaths.push_back("c:/winnt/fonts");
			fontPaths.push_back("c:/windows/fonts");

			char *ptr = getenv("windir");
			if (ptr)
			{
				std::string winFontPath = ptr;
				winFontPath += "\\fonts";
				fontPaths.push_back(winFontPath);
			}
#elif defined(__APPLE__)
			fontPaths.push_back(".");
			fontPaths.push_back("/usr/share/fonts/ttf");
			fontPaths.push_back("/usr/share/fonts/ttf/western");
			fontPaths.push_back("/usr/share/fonts/ttf/decoratives");
			fontPaths.push_back("/Library/Fonts:/System/Library/Fonts");
#else
			fontPaths.push_back(".");
			fontPaths.push_back("/usr/share/fonts/ttf");
			fontPaths.push_back("/usr/share/fonts/ttf/western");
			fontPaths.push_back("/usr/share/fonts/ttf/decoratives");
			fontPaths.push_back("/usr/share/fonts/truetype/freefont");
#endif


			std::string font = "Arial.ttf";
			std::string fontPath = findInPath(fontPaths, font);
			if (fontPath.empty())
			{
				font = "FreeSans.ttf";
				fontPath = findInPath(fontPaths, font);
			}
			int status = FT_New_Face(ft, fontPath.c_str(), 0, &m_face);
			if (status==FT_Err_Unknown_File_Format) {
				std::cerr << "Format of font file not supported (" << status << ") " << font << std::endl;
				return;
			}
			if (status) {
				std::cerr << "Could not open font (" << status << ") " << font << std::endl;
				return;
			}

			FT_Set_Pixel_Sizes(m_face, 0, 30);
		}


		FT_Face& getFace() { return m_face; }
		FT_GlyphSlot getGlyph() { return m_face->glyph; }
		FT_Face m_face;
	};
	Initializer init;

}

#endif

static bool s_initialized = false;
static GLuint s_textureID = 0;
static GLuint s_vbo;
static GLuint v_shader = 0;
static GLuint f_shader = 0;
static GLuint program = 0;
static GLint texture0Uniform = -1;
static GLint colorUniform = -1;
static GLint attribute_coord = -1;
static GLuint box_vbo = 0;
static glm::vec4 s_color = glm::vec4(1, 1, 0, 1);

void render_text(const char *text, float x, float y, float sx, float sy) {
	const char *p;
#ifdef HAVE_FREETYPE

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDisable(GL_CULL_FACE);

	glActiveTexture(GL_TEXTURE0);
	if (!s_initialized)
	{
		s_initialized = true;

		glGenTextures(1, &s_textureID);
		//glUniform1i(uniform_tex, 0);	


		// Create a vertex shader from a string
		v_shader = vr::loadShaderFromString(
			STRINGIFY(

			\#version 120 \n
			attribute vec4 coord; \n
			varying vec2 texcoord; \n
			uniform sampler2D tex; \n
			\n
		void main(void) { \n
		gl_Position = vec4(coord.xy, 0, 1); \n
		texcoord = coord.zw; \n
		}
		)
			, GL_VERTEX_SHADER);

		// Create a fragment shader from a string
		f_shader = vr::loadShaderFromString(
			STRINGIFY(

			\#version 120 \n
			varying vec2 texcoord; \n
			uniform sampler2D tex; \n
			uniform vec4 color; \n
			\n
			\n
			void main(void) {
			\n
				gl_FragColor = vec4(1, 1, 1, texture2D(tex, texcoord).r) * color;  \n
		}
		)

			, GL_FRAGMENT_SHADER);

		// Create a program object where the both shaders are attached.
		// A Shader Program consists of 1 or two shaders.
		program = glCreateProgramObjectARB();
		glAttachObjectARB(program, v_shader);
		glAttachObjectARB(program, f_shader);

		// Try to link the program (with the two attached shaders)
		DO_GL_AND_CHECK(glLinkProgramARB(program));

		// Check if it was successful
		if (!vr::checkLinkStatus(program))
			return;
		CheckErrorsGL("BEGIN :GetLocation");

		// Locate the two uniforms in the linked program. We will assign values to them later on
		DO_GL_AND_CHECK(colorUniform = glGetUniformLocation(program, "color"));
		DO_GL_AND_CHECK(texture0Uniform = glGetUniformLocation(program, "tex"));
		DO_GL_AND_CHECK(attribute_coord = glGetAttribLocation(program, "coord"));

		CheckErrorsGL("END :GetLocation");

		CheckErrorsGL("BEGIN :glGenBuffers");

		glGenBuffers(1, &box_vbo);
		CheckErrorsGL("END :glGenBuffers");

	}
	DO_GL_AND_CHECK(glUseProgramObjectARB(program));

	CheckErrorsGL("BEGIN :setupTexture");
	glBindTexture(GL_TEXTURE_2D, s_textureID);
	CheckErrorsGL("END :setupTexture");
	glUniform1i(texture0Uniform, 0);
	CheckErrorsGL("END :glUniform1i");
	glUniform4fv(colorUniform, 1, glm::value_ptr(s_color));
	CheckErrorsGL("END :glUniform4fv");

	CheckErrorsGL("BEGIN :TextureParameters");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	CheckErrorsGL("END :TextureParameters");


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glEnableVertexAttribArray(attribute_coord);
	glBindBuffer(GL_ARRAY_BUFFER, box_vbo);
	glVertexAttribPointer(
		attribute_coord, // attribute
		4,                 // number of elements per vertex, here (r,g,b)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
		);

	for (p = text; *p; p++) {
		if (FT_Load_Char(init.getFace(), *p, FT_LOAD_RENDER))
			continue;

	CheckErrorsGL("BEGIN :Testing");
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			init.getGlyph()->bitmap.width,
			init.getGlyph()->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			init.getGlyph()->bitmap.buffer
			);
	CheckErrorsGL("END :Testing");

		float x2 = x + init.getGlyph()->bitmap_left * sx;
		float y2 = -y - init.getGlyph()->bitmap_top * sy;
		float w = init.getGlyph()->bitmap.width * sx;
		float h = init.getGlyph()->bitmap.rows * sy;

		GLfloat box[4][4] = {
			{ x2, -y2, 0, 0 },
			{ x2 + w, -y2, 1, 0 },
			{ x2, -y2 - h, 0, 1 },
			{ x2 + w, -y2 - h, 1, 1 },
		};

		CheckErrorsGL("BEGIN :DrawVertexBuffer");

	
		glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
		CheckErrorsGL("END :DrawVertexBuffer");


		x += (init.getGlyph()->advance.x >> 6) * sx;
		y += (init.getGlyph()->advance.y >> 6) * sy;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableVertexAttribArray(attribute_coord);

	DO_GL_AND_CHECK(glUseProgramObjectARB(0));

	glPopAttrib();
#endif
}

void vr::Text::setColor(const glm::vec4& color)
{
	s_color = color;
}


void vr::Text::setFontSize(int size)
{
	#ifdef HAVE_FREETYPE
	FT_Set_Pixel_Sizes(init.getFace(), 0, size);
	#endif
}

void vr::Text::drawText(float x, float y, const char *s)
{
	float sx = 2.0f / glutGet(GLUT_WINDOW_WIDTH);
	float sy = 2.0f / glutGet(GLUT_WINDOW_HEIGHT);

	render_text(s, -1 + x * sx, 1 - y * sy, sx, sy);
}
