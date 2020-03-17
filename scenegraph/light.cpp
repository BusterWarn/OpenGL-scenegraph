/**
 * Light
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - A light in a scenegraph. Will illuminate it's surroundings.
 **/

#include <iostream>
#include <sstream>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "headers/light.h"
#include "headers/state.h"
#include "headers/loader.h"

light::light() {}

void light::enable() { enabled = true; }
void light::disable() { enabled = false; }

std::shared_ptr<geometry> light::create_geometry() {

    m_geometry = std::dynamic_pointer_cast<geometry>(
        load_3d_model_file("models/sphere.obj")->get_children().front()
    );
    std::shared_ptr<material> m = m_geometry->get_state()->get_material();
    m->set_diffuse(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
    m->set_specular(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
    m->set_ambient(this->diffuse * 10.0f + this->specular * 10.0f);

    return m_geometry;
}

std::shared_ptr<geometry>& light::get_geometry() {

    return m_geometry; 
}

void light::apply(GLuint program, size_t idx) {

    if (m_geometry != nullptr)
        this->position = m_geometry->get_transformation() * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    std::stringstream str;
    str << "lights[" << idx << "].";
    std::string prefix = str.str();

    GLint loc = -1;
    std::string uniform_name;

    uniform_name = prefix + "enabled";
    loc = glGetUniformLocation(program, uniform_name.c_str());
    if (loc == -1) {
        std::cerr << "Could not bind uniform << " << uniform_name << std::endl;
        return;
    }
    glUniform1i(loc, enabled);
    
    #define APPLY_UNIFORM4V(PROGRAM, NAME, VALUE) \
    uniform_name = prefix + std::string(NAME);  \
    loc = glGetUniformLocation(program, uniform_name.c_str()); \
    if (loc == -1) { \
        std::cerr << "Could not bind uniform << " << uniform_name << std::endl; \
        return; \
    } \
    glUniform4fv(loc, 1, glm::value_ptr(VALUE));

    APPLY_UNIFORM4V(program, "diffuse", this->diffuse);
    APPLY_UNIFORM4V(program, "specular", this->specular);
    APPLY_UNIFORM4V(program, "position", this->position);
}