/**
 * Geometry
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - A geometry node in a scenegraph.
 **/

#ifndef __GEOMETRY__
#define __GEOMETRY__

#include <vector>
#include <memory>

#include <GL/glew.h>
#include <GL/glut.h>

#include <glm/glm.hpp>

#include "node.h"
#include "material.h"
#include "update_callback_geometry.h"

/**
 * Geometry class. Derived from node.
 * 
 * A geometry in a scenegraph.
**/ 
class geometry : public node {

    public:

        std::vector<glm::vec4> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texture;
        std::vector<GLushort> elements;

        /**
         * Constructor for geometry.
         */
        geometry();

        /**
         * Accept a node visitor from the visitor design pattern.
         * 
         * @param v Node visitor to visit the geometry.
         */
        virtual void accept(node_visitor& v) override;

        /**
         * Applies the geometries general update callback method.
         */
        virtual void apply_update_callback();

        /**
         * Calculates a bounding box for the geometry,
         * 
         * @return The bounding box.
         */
        virtual bounding_box calculate_bounding_box() override;

        /**
         * Initiates the geometries uniform location in shader.
         * 
         * @param program OpenGL program where geometry should be rendered.
         */
        bool init_shaders(GLint program);
        
        /**
         * Initiates geometries textures if it should have any.
         * 
         * @param program OpenGL program where geometry should be rendered.
         */
        void init_texture(GLint program, const char* filename);
   
        /**
         * Uploads the geometries buffers to OpenGL pipeline.
         */
        void upload();

        /**
         * Draws the geometry in the program by enabling its buffers in OpenGL
         * pipeline.
         * 
         * @param program OpenGL program where geometry should be rendered.
         */
        void draw(GLuint program);

        /**
         * NOT YET IMPLEMENTED!
         * Draws the geometries bounding box.
         */
        void draw_bbox();

        /**
         * Adds a vertice to the geometry.
         * 
         * @param vertice The vertice.
         */
        void add_vertice(glm::vec4& vertice);

        /**
         * Adds a vertice together with a vertice normal.
         * 
         * @param vertice The vertice.
         * @param normal The vertice normal.
         */
        void add_vertice_with_normal(glm::vec4& vertice, glm::vec3& normal);
        
        // Getters
        virtual short get_type() override;
        virtual unsigned long get_nr_children() override;
        glm::mat4& get_transformation();

        // Setters
        void set_transformation(glm::mat4& transformation);

    private:

        GLuint m_vbo_vertices, m_vbo_normals, m_ibo_elements, m_vbo_textures;
        GLint m_attribute_v_coord;
        GLint m_attribute_v_normal;
        GLint m_attribute_v_tex_coord;

        GLint m_uniform_m;
        GLint m_uniform_m_3x3_inv_transp;

        glm::mat4 m_transformation;

        update_callback_geometry m_update_callback;
};

#endif