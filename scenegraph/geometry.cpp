/**
 * Geometry
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - A geometry node in a scenegraph.
 **/

#include <iostream>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "headers/SOIL.h"

#include "headers/geometry.h"
#include "headers/state.h"
#include "headers/update_callback_geometry.h"

geometry::geometry():
        m_vbo_vertices(0),
        m_vbo_normals(0),
        m_ibo_elements(0),
        m_vbo_textures(0),
        m_attribute_v_coord(-1),
        m_attribute_v_normal(-1),
        m_attribute_v_tex_coord(-1) {
    
    m_state = std::shared_ptr<state>(new state);
    m_transformation = glm::mat4(1.0f);
}

void geometry::accept(node_visitor &v) {

    v.visit(*this);
}

void geometry::apply_update_callback() {
    
    m_update_callback.update(*this);
}

bounding_box geometry::calculate_bounding_box() {
    
    bounding_box box;
    for (auto v : vertices) {

        glm::vec3 v_transformed = m_transformation * v;
        box.expand(v_transformed);
    }
    return box;
}

bool geometry::init_shaders(GLint program) {

    const char* attribute_name;

    attribute_name = "v_coord";
    m_attribute_v_coord = glGetAttribLocation(program, attribute_name);
    if (m_attribute_v_coord == -1) {
        fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
        return false;
    }

    attribute_name = "v_normal";
    m_attribute_v_normal = glGetAttribLocation(program, attribute_name);
    if (m_attribute_v_normal == -1) {
        fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
        return false;
    }

    attribute_name = "v_text_coord";
    m_attribute_v_tex_coord = glGetAttribLocation(program, attribute_name);
    if (m_attribute_v_tex_coord == -1) {
        fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
        return false;
    }

    const char* uniform_name;
    uniform_name = "m";
    m_uniform_m = glGetUniformLocation(program, uniform_name);
    if (m_uniform_m == -1) {
        fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
        return 0;
    }

    uniform_name = "m_3x3_inv_transp";
    m_uniform_m_3x3_inv_transp = glGetUniformLocation(program, uniform_name);
    if (m_uniform_m_3x3_inv_transp == -1) {
        fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
        return false;
    }

    return true;
}

void geometry::init_texture(GLint program, const char* filename) {

    glActiveTexture(GL_TEXTURE0);
    
    GLuint tex_id;
    glGenTextures(1, &tex_id);

    glBindTexture(GL_TEXTURE_2D, tex_id);
    int width, height;

    unsigned char* data = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    SOIL_free_image_data(data);

    glUniform1i(glGetUniformLocation(program, "material.diffuse_map"), 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void geometry::upload() {
    
    if (this->vertices.size() > 0) {
        glGenBuffers(1, &this->m_vbo_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_vertices);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(this->vertices[0]),
            this->vertices.data(), GL_STATIC_DRAW);
    }

    if (this->normals.size() > 0) {
        glGenBuffers(1, &this->m_vbo_normals);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_normals);
        glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(this->normals[0]),
            this->normals.data(), GL_STATIC_DRAW);
    }

    if (this->elements.size() > 0) {
        glGenBuffers(1, &this->m_ibo_elements);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_ibo_elements);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->elements.size() * sizeof(this->elements[0]),
            this->elements.data(), GL_STATIC_DRAW);
    }

    if (this->texture.size() > 0) {
        glGenBuffers(1, &this->m_vbo_textures);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_textures);
        glBufferData(GL_ARRAY_BUFFER, this->texture.size() * sizeof(this->texture[0]),
            this->texture.data(), GL_STATIC_DRAW);
    }
}


void geometry::draw(GLuint program) {
    
    if (normals.size() == 0) {
        draw_bbox();
        return;
    }   

    if (this->m_vbo_vertices != 0) {
        glEnableVertexAttribArray(m_attribute_v_coord);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_vertices);
        glVertexAttribPointer(
            m_attribute_v_coord,    // attribute
            4,                      // number of elements per vertex, here (x,y,z,w)
            GL_FLOAT,               // the type of each element
            GL_FALSE,               // take our values as-is
            0,                      // no extra data between each position
            0                       // offset of first element
        );
    }

    if (this->m_vbo_normals != 0) {
        glEnableVertexAttribArray(m_attribute_v_normal);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_normals);
        glVertexAttribPointer(
            m_attribute_v_normal,   // attribute
            3,                      // number of elements per vertex, here (x,y,z)
            GL_FLOAT,               // the type of each element
            GL_FALSE,               // take our values as-is
            0,                      // no extra data between each position
            0                       // offset of first element
        );
    }

    if (this->m_vbo_textures != 0) {
        glEnableVertexAttribArray(m_attribute_v_tex_coord);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_textures);
        glVertexAttribPointer(
            m_attribute_v_tex_coord,// attribute
            2,                      // number of elements per vertex, here (x,y,z)
            GL_FLOAT,               // the type of each element
            GL_FALSE,               // take our values as-is
            0,                      // no extra data between each position
            0                       // offset of first element
        );
    }

    /* Apply object's transformation matrix */
    glUniformMatrix4fv(m_uniform_m, 1, GL_FALSE, glm::value_ptr(m_transformation));

    /* Transform normal vectors with transpose of inverse of upper left
    3x3 model matrix (ex-gl_NormalMatrix): */
    glm::mat3 m_3x3_inv_transp = glm::transpose(glm::inverse(glm::mat3(m_transformation)));
    glUniformMatrix3fv(m_uniform_m_3x3_inv_transp, 1, GL_FALSE, glm::value_ptr(m_3x3_inv_transp));

    /* Push each element in buffer_vertices to the vertex shader */
    if (this->m_ibo_elements != 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_ibo_elements);
        int size;
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
        glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
        
    }
    else {
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)this->vertices.size());
    }

    if (this->m_vbo_normals != 0)
        glDisableVertexAttribArray(m_attribute_v_normal);

    if (this->m_vbo_vertices != 0)
        glDisableVertexAttribArray(m_attribute_v_coord);

    if (this->m_vbo_vertices != 0)
        glDisableVertexAttribArray(m_attribute_v_tex_coord);
}

void geometry::draw_bbox() {

    std::cout << "I'm drawing a bbox" << std::endl;
}
void geometry::add_vertice(glm::vec4& vertice) {

    this->vertices.push_back(vertice);
}

void geometry::add_vertice_with_normal(glm::vec4& vertice, glm::vec3& normal) {

    this->vertices.push_back(vertice);
    this->normals.push_back(normal);
}

short geometry::get_type() {

    return NODE_GEOMETRY;
}

unsigned long geometry::get_nr_children() {

    return 1;
}

glm::mat4& geometry::get_transformation() {
    return m_transformation;
}

void geometry::set_transformation(glm::mat4& transformation) {
    m_transformation = transformation;
}