/**
 * Material
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - Material of a geometry in a scenegraph.
 **/

#ifndef __MATERIAL__
#define __MATERIAL__

#include <vector>
#include <memory>

#include <GL/glew.h>
#include <GL/glut.h>

#include <glm/glm.hpp>

/**
 * Material class. Material of a geometry in a scenegraph.
**/
class material {

    public:

        /**
         * Constructor for material.
         */
        material();

        /**
         * Applies the material in the program by updating its OpenGL uniform
         * location values.
         * 
         * @param program The OpenGL program.
         */
        void apply(GLuint program);

        /**
         * Prints the materials values to standard out. Can be Usefull for
         * debugging.
         */
        void print();

        // Getters.
        glm::vec4 get_ambient() const;
        glm::vec4 get_specular() const;
        glm::vec4 get_diffuse() const;
        bool get_has_diffuse_map();

        // Setters.
        void set_ambient(const glm::vec4& color);
        void set_specular(const glm::vec4& color);
        void set_diffuse(const glm::vec4& color);
        void set_shininess(float s);
        void set_has_diffuse_map(bool has_diffuse_map);

    private:

        glm::vec4 m_ambient, m_diffuse, m_specular;

        GLfloat m_shininess;
        bool m_has_diffuse_map;

};

#endif