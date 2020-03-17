/**
 * Transform
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - A transform node in a scenegraph.
 **/

#include <iostream>

#include "headers/transform.h"
#include "headers/state.h"

transform::transform() {
    
    m_transformation = std::shared_ptr<glm::mat4>(new glm::mat4(1.0f));
    m_state = std::shared_ptr<state>(new state);
    m_update_callback = nullptr;
}

transform::transform(std::shared_ptr<group> group) {

    std::vector< std::shared_ptr<node> > children = group->get_children();

    for (auto child : children) {
        add_child(child);
    }

    m_state = group->get_state();
    m_transformation = std::shared_ptr<glm::mat4>(new glm::mat4(1.0f));

    m_update_callback = nullptr;
}

void transform::accept(node_visitor& v) {

    v.visit(*this);
}

void transform::apply_update_callback() {
    
    if (m_update_callback)
        m_update_callback->update(*this);
}

bounding_box transform::calculate_bounding_box() {
    
    bounding_box box;
    
    for (auto node : this->get_children()) {

        bounding_box child_box;
        if (node->get_type() == NODE_GEOMETRY) {

            std::shared_ptr<geometry> g = std::dynamic_pointer_cast<geometry>(node);
            for (auto v : g->vertices) {

                glm::vec3 v_transformed = *this->m_transformation * v;
                child_box.expand(v_transformed);
            }
        } else {

            child_box = node->calculate_bounding_box();
            box.expand(child_box);
        }
        box.expand(child_box);
    }

    return box;
}

void transform::multiply_transformation(std::shared_ptr<glm::mat4> transformation) {

    *m_transformation =  *m_transformation * *transformation;
}

void transform::multiply_transformation(glm::mat4& transformation) {

    *m_transformation =  *m_transformation * transformation;
}

short transform::get_type() {

    return NODE_TRANSFORM;
}

std::shared_ptr<glm::mat4> transform::get_transformation() {

    return m_transformation;
}

std::shared_ptr<update_callback_transform> transform::get_update_callback() {
    
    return m_update_callback;
}

void transform::set_transformation(std::shared_ptr<glm::mat4> transformation) {

    m_transformation = transformation;
}

void transform::set_update_callback(std::shared_ptr<update_callback_transform> update_callback) {

    m_update_callback = update_callback;
}