/**
 * Camera
 * 
 * @author      - Buster Hultgren Wärn
 * @date        - 2020-03-17
 * @description - Camera in OpenGL scenegraph.
**/

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "headers/camera.h"

camera::camera(int width, int height) {

    m_screen_size[0] = width;
    m_screen_size[1] = height;

    m_near_far = glm::vec2(0.1, 100);

    // Default view
    world2camera = glm::lookAt(
        glm::vec3(0.0, 0.0, 4.0),   // eye
        glm::vec3(0.0, 0.0, 0.0),   // direction
        glm::vec3(0.0, 1.0, 0.0));  // up
}

bool camera::init(GLuint program) {

    const char* uniform_name;
    uniform_name = "v";
    m_uniform_v = glGetUniformLocation(program, uniform_name);
    if (m_uniform_v == -1) {
        std::cerr << "Could not bind uniform " << uniform_name << std::endl;
        return false;
    }
    uniform_name = "p";
    m_uniform_p = glGetUniformLocation(program, uniform_name);
    if (m_uniform_p == -1) {
        std::cerr << "Could not bind uniform " << uniform_name << std::endl;
        return false;
    }

    uniform_name = "v_inv";
    m_uniform_v_inv = glGetUniformLocation(program, uniform_name);
    if (m_uniform_v_inv == -1) {
        std::cerr << "Could not bind uniform " << uniform_name << std::endl;
        return false;
    }

    return true;
}

void camera::apply(GLuint program) {

    glm::mat4 camera2screen = glm::perspective(
        45.0f, 1.0f*m_screen_size[0] / m_screen_size[1],
        m_near_far[0],
        m_near_far[1]);

    glUniformMatrix4fv(m_uniform_v, 1, GL_FALSE, glm::value_ptr(this->world2camera));
    glUniformMatrix4fv(m_uniform_p, 1, GL_FALSE, glm::value_ptr(camera2screen));
    glm::mat4 v_inv = glm::inverse(this->world2camera);
    glUniformMatrix4fv(m_uniform_v_inv, 1, GL_FALSE, glm::value_ptr(v_inv));
}

void camera::rotate_x(float rot_x) {

    glm::vec3 y_axis_world = glm::mat3(this->world2camera) * glm::vec3(0.0, 1.0, 0.0);
    this->world2camera = glm::rotate(glm::mat4(1.0), glm::radians(rot_x), y_axis_world) * this->world2camera;
}

void camera::rotate_y(float rot_y) {
    
    this->world2camera = glm::rotate(glm::mat4(1.0), glm::radians(rot_y), glm::vec3(1.0, 0.0, 0.0)) * this->world2camera;
}

/**
* Get a normalized vector from the center of the virtual ball O to a
* point P on the virtual ball surface, such that P is aligned on
* screen's (X,Y) coordinates.  If (X,Y) is too far away from the
* sphere, return the nearest point on the virtual ball surface.
*/
glm::vec3 camera::get_arcball_vector(int x, int y) {
  
    glm::vec3 P = glm::vec3(
        1.0*x / m_screen_size[0] * 2 - 1.0,
        1.0*y / m_screen_size[1] * 2 - 1.0,
        0);
    P.y = -P.y;
    float OP_squared = P.x * P.x + P.y * P.y;
    if (OP_squared <= 1 * 1)
        P.z = sqrt(1 * 1 - OP_squared);  // Pythagoras
    else
        P = glm::normalize(P);  // nearest point
    return P;
}

glm::uvec2 camera::get_screen_size() {

    return m_screen_size;
}

void camera::set_screen_size(const glm::uvec2& size) {

    m_screen_size = size;
}

void camera::set_near_far(const glm::vec2& near_far) {
    
    m_near_far = near_far;
}