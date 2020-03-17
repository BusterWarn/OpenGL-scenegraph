/**
 * Scene
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - The main scene in a scenegraph.
**/

#ifndef __SCENE__
#define __SCENE__

#include <vector>
#include <memory>

#include "camera.h"
#include "group.h"
#include "transform.h"
#include "light.h"
#include "bounding_box.h"

#include "render_visitor.h"
#include "init_visitor.h"
#include "update_visitor.h"

/**
 * Scene class. Implemented as a singleton. The main scene in a scenegraph.
**/ 
class scene {

    public:
        
        /**
         * Gets the global instance of the scenegraph.
         * 
         * @return The scenegraph.
         */
        static std::shared_ptr<scene> get_instance();
        
        /**
         * Deconstructor for scene. Sets instance to nullptr.
         */
        ~scene();

        /**
         * Initiates the scenegraphs shaders.
         * 
         * @param vshader_filename Filename of vertex shader
         * @param fshader_filename Filename of fragment shader
         */
        bool init_shaders(const std::string& vshader_filename, const std::string& fshader_filename);

        /**
         * Applies the camera by calling its apply function.
         */
        void apply_camera();

        /**
         * Calls glUseProgram on the scense OpenGL program.
         */
        void use_program();

        /**
         * Calculates a bounding box for the whole scene.
         * 
         * @return The bounding box.
         */
        bounding_box calculate_bounding_box();

        /**
         * Draws the entire scenegraph by using its update_visitor and
         * render_visitor.
         */
        void draw();

        /**
         * Changes the root node transformations transformation.
         * 
         * @param transformation The new transformation.
         */
        void transform_root(glm::mat4& transformation);

        /**
         * Adds a node to the scenegraphs root node.
         * 
         * @param node The new node to be added.
         */
        void add_node(std::shared_ptr<node> node);

        /**
         * Adds a light to the current vector of lights.
         * 
         * @param light The new light.
         */
        void add_light(std::shared_ptr<light> light);

        /**
         * Gets one of the root nodes node.
         * 
         * @param i Index of the nodes position in the roots children vector.
         */
        std::shared_ptr<node> get_node(size_t i);
        
        // Getters
        unsigned long get_nr_geometries();
        GLuint get_program();
        std::shared_ptr<camera> get_camera();
        std::vector<std::shared_ptr<light> > get_lights();
        std::shared_ptr<transform> get_root();
        std::shared_ptr<render_visitor> get_render_visitor();
    
    private:
        
        static scene* m_instance;
        scene();
        
        std::shared_ptr<transform> m_root;
        std::shared_ptr<camera> m_camera;
        std::vector<std::shared_ptr<light> > m_lights;

        std::shared_ptr<init_visitor> i_visitor = nullptr;
        std::shared_ptr<render_visitor> r_visitor = nullptr;
        std::shared_ptr<update_visitor> u_visitor = nullptr;
        
        GLuint m_program;
        GLint m_uniform_numberOfLights;


};

#endif