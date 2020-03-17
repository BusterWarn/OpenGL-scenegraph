/**
 * Texture
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - Texture for a geometry in the scenegraph.
**/

#ifndef __TEXTURE__
#define __TEXTURE__

#include <GL/glew.h>

#include <glm/glm.hpp>

/**
 * Class texture. Texture for a geometry in the scenegraph.
**/ 
class texture {

    public:

        /**
         * Constructor for texture.
         * 
         * @param filename Filname of the texturefile.
        **/
        texture(std::string& filename);

        // Setters
        void set_texture(std::string& m_filename);

        // Getters
        std::string get_filename() { return m_filename; }

    private:

        std::string m_filename;
};

#endif