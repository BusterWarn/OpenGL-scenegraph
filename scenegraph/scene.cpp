/**
 * Scene
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - The main scene in a scenegraph.
**/

#include <iostream>

#include <vr/shaderUtils.h>

#include "headers/scene.h"

scene* scene::m_instance = nullptr;


scene::scene() : m_uniform_numberOfLights(-1) {
    
    m_root = std::shared_ptr<transform>(new transform);
    m_camera = std::shared_ptr<camera>(new camera);
    u_visitor = std::shared_ptr<update_visitor>(new update_visitor);
    m_program = glCreateProgram();
}

scene::~scene() {
    m_instance = nullptr;
}

std::shared_ptr<scene> scene::get_instance() {

    if (!m_instance)
        m_instance = new scene();
    
    return std::shared_ptr<scene>(m_instance);
}

bool scene::init_shaders(const std::string& vshader_filename, const std::string& fshader_filename) {
    
    /* Compile and link shaders */
    GLint link_ok = GL_FALSE;
    GLint validate_ok = GL_FALSE;
    GLuint vs, fs;
    if ((vs = vr::loadShader(vshader_filename, GL_VERTEX_SHADER)) == 0) return false;
    if ((fs = vr::loadShader(fshader_filename, GL_FRAGMENT_SHADER)) == 0) return false;

    glAttachShader(m_program, vs);
    glAttachShader(m_program, fs);
    glLinkProgram(m_program);
    glGetProgramiv(m_program, GL_LINK_STATUS, &link_ok);

    if (!link_ok) {
        fprintf(stderr, "glLinkProgram:");
        vr::printCompilationError(m_program);
        return false;
    }
    glValidateProgram(m_program);
    glGetProgramiv(m_program, GL_VALIDATE_STATUS, &validate_ok);

    if (!validate_ok) {
        fprintf(stderr, "glValidateProgram:");
        vr::printCompilationError(m_program);
        return false;
    }

    r_visitor = std::shared_ptr<render_visitor> (new render_visitor(m_program));
    i_visitor = std::shared_ptr<init_visitor> (new init_visitor(m_program));
    i_visitor->set_lights(m_lights);

    // Update number of lights
    if (m_uniform_numberOfLights == -1) {
        
        const char *uniform_name = "numberOfLights";
        m_uniform_numberOfLights = glGetUniformLocation(m_program, uniform_name);
        if (m_uniform_numberOfLights == -1)
            std::cerr << "Could not bind uniform " << uniform_name << std::endl;
    }

    m_camera->init(m_program);
    m_root->accept(*i_visitor);

    return true;
}

void scene::apply_camera() {
    
    m_camera->apply(m_program);
}

void scene::use_program() {
    
    glUseProgram(m_program);
}

bounding_box scene::calculate_bounding_box() {
    
    bounding_box box;
    box.expand(m_root->calculate_bounding_box());

    return box;
}

void scene::draw() {
    
    glUseProgram(m_program);
    glUniform1i(m_uniform_numberOfLights, (GLint)m_lights.size());

    m_root->accept(*u_visitor);
    m_root->accept(*r_visitor);
}

void scene::transform_root(glm::mat4& transformation) {

    m_root->multiply_transformation(transformation);
}

void scene::add_node(std::shared_ptr<node> node) {

    m_root->add_child(node);
}

void scene::add_light(std::shared_ptr<light> light) {

    m_lights.push_back(light);
}

std::shared_ptr<node> scene::get_node(size_t i) {

    return m_root;
}

unsigned long scene::get_nr_geometries() {
    
    return m_root->get_nr_children();
}

GLuint scene::get_program() {

    return m_program;
}

std::shared_ptr<camera> scene::get_camera() {
    
    return m_camera;
}

std::vector<std::shared_ptr<light> > scene::get_lights() {

    return m_lights;
}

std::shared_ptr<transform> scene::get_root() {
    
    return m_root;
}


std::shared_ptr<render_visitor> scene::get_render_visitor() {
    
    return r_visitor;
}