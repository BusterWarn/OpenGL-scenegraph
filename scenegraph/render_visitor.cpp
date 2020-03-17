/**
 * Init Visitor
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - A visitor that renders a scenegraph.
 **/

#include <iostream>
#include <vector>

#include "headers/render_visitor.h"
#include "headers/state.h"
#include "headers/node.h"
#include "headers/group.h"
#include "headers/transform.h"
#include "headers/geometry.h"

render_visitor::render_visitor(GLuint program) {

    m_program = program;
    transformations.push(glm::mat4(1.0));
}

void render_visitor::visit(group& g) {

    std::shared_ptr<state> state = g.get_state();
    if (state) {
        state->apply();
    }
    
    std::vector<std::shared_ptr<node> >& children = g.get_children();

    for (auto child : children) {
        child->accept(*this);
    }
}

void render_visitor::visit(transform& t) {

    std::shared_ptr<state> state = t.get_state();
    if (state) {
        state->apply();
    }

    glm::mat4 new_transformation = this->transformations.top() * *t.get_transformation();
    this->transformations.push(new_transformation);
    
    std::vector<std::shared_ptr<node> >& children = t.get_children();

    for (auto child : children) {
        child->accept(*this);
    }
    transformations.pop();
}

void render_visitor::visit(geometry& g) {

    std::shared_ptr<state> state = g.get_state();
    if (state)
        state->apply();
    
    g.set_transformation(transformations.top());
    g.draw(m_program);
}

void render_visitor::set_top_transformation(glm::mat4 transformation) {

    if (!this->transformations.empty())
        this->transformations.pop();
    
    this->transformations.push(transformation);
}

glm::mat4 render_visitor::get_transformation() {

    return this->transformations.top();
}