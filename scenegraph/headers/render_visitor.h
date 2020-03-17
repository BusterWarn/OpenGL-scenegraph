/**
 * Init Visitor
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - A visitor that renders a scenegraph.
 **/

#ifndef __RENDER_VISITOR__
#define __RENDER_VISITOR__

#include <stack>
#include <memory>

#include <GL/glew.h>
#include <GL/glut.h>

#include <glm/glm.hpp>

#include "node_visitor.h"

/**
 * render_visitor class. Derived from node_visitor.
 * 
 * A visitor that renders a scenegraph. Will traverse the scenegraph tree 
 * hiearchy as the node_visitor and do three things mainly:
 * 
 * 1. Apply states to all nodes it visits.
 * 
 * 2. Uses a transforamtion stack and tracks each transformation it visits. When
 * visiting a transformation node it will add that transformation to its stack.
 * 
 * 3. Draw geometries once visiting them.
**/ 
class render_visitor : public node_visitor {
    
    public:

        /**
         * Constructor for render_visitor.
         * 
         * @param program The OpenGL program the the scenegraph will use.
         */
        render_visitor(GLuint program);

        /**
         * Visits a group node. Applies its state and visits its children.
         * 
         * @param g The group node.
         */
        virtual void visit(group& g) override;

        /**
         * Visits a transform node. Applies its state, Adds its transormation to
         * the transformation stack and visits the transformation nodes
         * children. When done visting, pops the stack.
         * 
         * @param t The transform node.
         */
        virtual void visit(transform& t) override;

        /**
         * Visits a geometry node. Applies its sts, update its transformation
         * and calls its draw function.
         * 
         * @param g The geometry node.
         */
        virtual void visit(geometry& g) override;

        /**
         * Sets the top transformation on the transformation stack. 
         * 
         * @param g The new transformation.
         */
        void set_top_transformation(glm::mat4 transformation);

        // Getters
        glm::mat4 get_transformation();
    
    private:

        GLuint m_program;

        std::stack<glm::mat4> transformations;
};

#endif