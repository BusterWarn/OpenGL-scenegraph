/**
 * State
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - A state for a node in the scenegraph. Contains information
 * such as lights to ignore, texture, material and OpenGL draw settings.
**/

#ifndef __STATE__
#define __STATE__

#include <memory>
#include <vector>

#include <GL/glew.h>

#include "light.h"
#include "material.h"
#include "texture.h"

/**
 * State class. A state for a node in the scenegraph. Contains information such
 * as lights to ignore, texture, material and OpenGL draw settings.
**/ 
class state {

    public:
        
        /**
         * Constructor for state.
         * 
         * @param program OpenGL program of its node.
         * @param polygon_mode OpenGL polygon mode. Can be any of GL_FRONT,
         *      GL_BACK and GL_FRONT_AND_BACK.
         * @param cull_face_mode OpenGL cull face mode. Can be any of GL_LINE,
         *      GL_POINT and GL_FILL.
        **/
        state(GLint program = 0, short polygon_mode = 0,
            short cull_face_mode = 0);

        /**
         * Aplies the state to the OpenGL program by calling OpenGL functions
         * and setting uniform values.
        **/
        void apply();

        /**
         * Disables a light for this state.
         * 
         * @param id Id of the light in the shader.
        **/
        void disable_light(int id);


        // Getters
        std::shared_ptr<texture> get_texture() { return m_texture; };
        std::shared_ptr<material> get_material() { return m_material; };

        // Setters
        void set_program(GLint program);
        void set_lights(std::vector<std::shared_ptr<light>>& lights);
        void set_active_lights(std::vector<bool>& active_lights);
        void set_material(std::shared_ptr<material> material);
        void set_texture(std::shared_ptr<texture> texture);
        void set_polygon_mode(GLenum polygon_mode);
        void set_cull_face_mode(GLenum cull_face_mode);

    private:
        
        GLint m_program;

        std::vector<std::shared_ptr<light>> m_lights;
        std::vector<int> m_disabled_lights;
        
        std::shared_ptr<material> m_material;
        std::shared_ptr<texture> m_texture;

        GLenum m_polygon_mode;
        GLenum m_cull_face_mode;
};



#endif