/**
 * Init Visitor
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - A visitor that initiates a scenegraph.
 **/

#include <iostream>
#include <vector>
#include <memory>

#include "headers/init_visitor.h"
#include "headers/node.h"
#include "headers/group.h"
#include "headers/transform.h"
#include "headers/geometry.h"
#include "headers/texture.h"
#include "headers/state.h"

init_visitor::init_visitor(GLuint program) {

    m_program = program;
}

void init_visitor::visit(group& g) {

    std::shared_ptr<state> state = g.get_state();
    if (state) {
        state->set_lights(m_lights);
        state->set_program(m_program);
    }

    std::vector<std::shared_ptr<node>>& children = g.get_children();

    for (auto child : children) {
        child->accept(*this);
    }
}

void init_visitor::visit(transform& t) {

    std::shared_ptr<state> state = t.get_state();
    if (state) {
        state->set_lights(m_lights);
        state->set_program(m_program);
    }

    std::vector<std::shared_ptr<node>>& children = t.get_children();

    for (auto child : children) {
        child->accept(*this);
    }
}

void init_visitor::visit(geometry& g) {

    std::shared_ptr<state> state = g.get_state();
    if (state) {
        state->set_lights(m_lights);
        state->set_program(m_program);
    }
    
    g.upload(); 
    g.init_shaders(m_program);

    if (state && state->get_material() && state->get_material()->get_has_diffuse_map()) {
        g.init_texture(m_program, state->get_texture()->get_filename().c_str());
    }
}

void init_visitor::set_lights(std::vector<std::shared_ptr<light>>& lights) { m_lights = lights; }