/**
 * Group
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - A group node in a scenegraph.
 **/

#include <iostream>

#include "headers/group.h"
#include "headers/state.h"

group::group() {

    m_state = std::shared_ptr<state>(new state(0, GL_FILL, GL_BACK));
}

void group::accept(node_visitor& v) {

    v.visit(*this);
}

void group::apply_update_callback() {
    
    m_update_callback.update(*this);
}

bounding_box group::calculate_bounding_box() {
    
    bounding_box box;
    
    for (auto c : m_children) {

        bounding_box child_box = c->calculate_bounding_box();
        box.expand(child_box);
    }

    return box;
}

void group::add_child(std::shared_ptr<node> n) {

    m_children.push_back(n);
}

short group::get_type() {

    return NODE_GROUP;
}

unsigned long group::get_nr_children() {

    unsigned long nr_children = 0;
    for (auto c : m_children) {
        nr_children += c->get_nr_children();
    }
    return nr_children;
}

std::vector<std::shared_ptr<node> >& group::get_children() {

    return m_children;
}