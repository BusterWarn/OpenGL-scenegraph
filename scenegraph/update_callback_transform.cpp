/**
 * Update callback.
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - A function that is tied to a transform node in a scenegraph.
 *                The transform node can apply its update_callback to that node.
**/

#include <iostream>

#include "headers/update_callback_transform.h"
#include "headers/group.h"
#include "headers/transform.h"
#include "headers/geometry.h"
#include "headers/light.h"

update_callback_transform::update_callback_transform() {
    
    set_update(false);
    m_transformation = glm::mat4(1.0f);
    m_light_transformation = glm::mat4(1.0f);
}

update_callback_transform::update_callback_transform(glm::mat4& transfromation) {

    set_update(true);
    m_transformation = transfromation;
    m_light_transformation = glm::mat4(1.0f);
}

void update_callback_transform::update(transform& t) {
    
    if (!get_update()) return;
    t.multiply_transformation(m_transformation);
}

glm::mat4 update_callback_transform::get_transfromation() { return m_transformation; }
glm::mat4 update_callback_transform::get_light_transformation() { return m_light_transformation; }


void update_callback_transform::set_transfromation(glm::mat4 transfromation) {
    m_transformation = transfromation;
}

void update_callback_transform::set_light_transformation(glm::mat4 transfromation) {
    m_light_transformation = transfromation;
}