/**
 * Update Visitor
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - A visitor that visits a scenegraph and calls on every nodes
 *                update_callback function if they have one.
**/

#include <iostream>
#include <vector>
#include <memory>

#include "headers/update_visitor.h"
#include "headers/update_callback.h"
#include "headers/state.h"
#include "headers/node.h"
#include "headers/group.h"
#include "headers/transform.h"
#include "headers/geometry.h"

update_visitor::update_visitor() {}

void update_visitor::visit(group& g) {

    g.apply_update_callback();

    std::vector<std::shared_ptr<node>>& children = g.get_children();

    for (auto child : children) {
        child->accept(*this);
    }
}

void update_visitor::visit(transform& t) {

    t.apply_update_callback();

    std::vector<std::shared_ptr<node>>& children = t.get_children();

    for (auto child : children) {
        child->accept(*this);
    }
}

void update_visitor::visit(geometry& g) {
    
    g.apply_update_callback();
}
