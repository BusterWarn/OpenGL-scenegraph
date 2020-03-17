/**
 * Light
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - A light in a scenegraph. Will illuminate it's surroundings.
**/

#ifndef __LIGHT__
#define __LIGHT__

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "geometry.h"
#include "transform.h"

/**
 * Light class. A light in a scenegraph that illuminates its surroundings.
**/
class light {

    public:
        
        glm::vec4 position;
        glm::vec4 diffuse;
        glm::vec4 specular;
        
        /**
         * Constructor for light.
         */
        light();

        /**
         * Enables the particular light in the OpenGL program.
         */
        void enable();

        /**
         * Disables the particular light in the OpenGL program.
         */
        void disable();

        /**
         * Creates a geometry of a sphere that is connected to the light.
         * 
         * @return The geometry.
         */
        std::shared_ptr<geometry> create_geometry();

        /**
         * Applies the light in the program by updating its OpenGL uniform
         * location values.
         * 
         * @param program The OpenGL program.
         * @parm idx Which light is being applied. The Fragment shader keeps an
         * array of lights and depending on the idx that light will be enabled.
         */
        void apply(GLuint program, size_t idx);

        // Getters.
        std::shared_ptr<geometry>& get_geometry();

    private:
        
        friend class state;

        bool enabled;

        std::shared_ptr<geometry> m_geometry;
};

#endif