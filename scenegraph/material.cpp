/**
 * Material
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - Material of a geometry in a scenegraph.
 **/

#include <iostream>
#include <string>
#include <vector>

#include "headers/material.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


material::material() : m_shininess(5), m_has_diffuse_map(false) {

    m_ambient = glm::vec4(0.1, 0.1, 0.1, 1.0);
    m_diffuse = glm::vec4(1.0, 1.0, 1.0, 1.0);
    m_specular = glm::vec4(0.3, 0.3, 0.3, 1.0);
}

void material::apply(GLuint program) {

    GLint loc = 0;
    loc = glGetUniformLocation(program, "material.ambient");
    glUniform4fv(loc, 1, glm::value_ptr(m_ambient));

    loc = glGetUniformLocation(program, "material.specular");
    glUniform4fv(loc, 1, glm::value_ptr(m_specular));

    loc = glGetUniformLocation(program, "material.diffuse");
    glUniform4fv(loc, 1, glm::value_ptr(m_diffuse));

    loc = glGetUniformLocation(program, "material.shininess");
    glUniform1f(loc, m_shininess);

    loc = glGetUniformLocation(program, "material.has_diffuse_map");
    glUniform1i(loc, m_has_diffuse_map);

}

void material::print() {

    std::cout << "Printing material information" << std::endl;
    std::cout << "\tambient:    " << m_ambient.x << "-" << m_ambient.y << "-" << m_ambient.z << std::endl;
    std::cout << "\tspecular:   " << m_specular.x << "-" << m_specular.y << "-" << m_specular.z << std::endl;
    std::cout << "\tshininess:  " << m_diffuse.x << "-" << m_diffuse.y << "-" << m_diffuse.z << std::endl;
    std::cout << "\tshininess:  " << m_shininess << std::endl;
    std::cout << "\ttexture:    " << m_has_diffuse_map << std::endl;
}

glm::vec4 material::get_ambient() const { return m_ambient; }
glm::vec4 material::get_specular() const { return m_specular; }
glm::vec4 material::get_diffuse() const { return m_diffuse; }
bool material::get_has_diffuse_map() { return m_has_diffuse_map; }

void material::set_ambient(const glm::vec4& color)  { m_ambient = color; }
void material::set_specular(const glm::vec4& color)  { m_specular = color; }
void material::set_diffuse(const glm::vec4& color)  { m_diffuse = color; }
void material::set_shininess(float s)  { m_shininess = s; }
void material::set_has_diffuse_map(bool has_diffuse_map) { m_has_diffuse_map = has_diffuse_map; }