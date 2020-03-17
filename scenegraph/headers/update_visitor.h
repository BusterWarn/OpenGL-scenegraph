/**
 * Update Visitor
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - A visitor that visits a scenegraph and calls on every nodes
 *                update_callback function if they have one.
**/

#ifndef __UPDATE_VISITOR__
#define __UPDATE_VISITOR__

#include <stack>

#include <GL/glew.h>
#include <GL/glut.h>

#include <glm/glm.hpp>

#include "node_visitor.h"

/**
 * Class update_visitor. A visitor that visits a scenegraph and calls on every
 * nodes update_callback function if they have one.
**/
class update_visitor : public node_visitor {
    
    public:

        /**
         * Constructor for update visitor.
         */
        update_visitor();

        /**
         * Visits a group node. Calls its update_callback function if it has
         * one.
         * 
         * @param g The group node.
         */
        virtual void visit(transform& t) override;

        /**
         * Visits a transform node. Calls its update_callback function if it has
         * one.
         * 
         * @param t The transform node.
         */
        virtual void visit(group& g) override;

        /**
         * Visits a transform node. Calls its update_callback function if it has
         * one.
         * 
         * @param g The geometry node.
         */
        virtual void visit(geometry& g) override;
};

#endif