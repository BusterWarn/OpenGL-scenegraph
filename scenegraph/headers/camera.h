/**
 * Camera
 * 
 * @author      - Buster Hultgren Wärn
 * @date        - 2020-03-17
 * @description - Camera in OpenGL scenegraph.
**/

#ifndef __CAMERA__
#define __CAMERA__

#include <GL/glew.h>

#include <glm/glm.hpp>

class camera {

    public:
    
        glm::mat4 world2camera;

        /**
         * Constructor for camera.
         * 
         * @param width Width in pixels of camera.
         * @param height Height in pixels of camera.
         */
        camera(int width = 1280, int height = 720);

        /**
         * Initiates the camera.
         * 
         * @param program The OpenGL program the camera will be used in.
         */
        bool init(GLuint program);

        /**
         * Applies the camera to the program.
         * 
         * @param The OpenGL program the camera will be used in.
         */
        void apply(GLuint program);

        /**
         * Rotates the camera in x direction.
         * 
         * @param rot_x Radians the camera should be rotated in.
         */
        void rotate_x(float rot_x);

        /**
         * Rotates the camera in y direction.
         * 
         * @param rot_y Radians the camera should be rotated in.
         */
        void rotate_y(float rot_y);

        // Getters
        glm::vec3 get_arcball_vector(int x, int y);
        glm::uvec2 get_screen_size();

        // Setters
        void set_screen_size(const glm::uvec2& size);
        void set_near_far(const glm::vec2& near_far);

    
    private:

        GLint m_uniform_v;
        GLint m_uniform_p;
        GLint m_uniform_v_inv;
        glm::uvec2 m_screen_size;
        glm::vec2 m_near_far;
};

#endif