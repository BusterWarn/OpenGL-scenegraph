/**
 * Init Visitor
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - A visitor that initiates a scenegraph.
**/

#ifndef __INIT_VISITOR__
#define __INIT_VISITOR__

#include <GL/glew.h>
#include <GL/glut.h>

#include <glm/glm.hpp>

#include "node_visitor.h"
#include "light.h"

/**
 * init_visitor class. Derived from node_visitor.
 * 
 * A visitor that initiates a scenegraph. Will traverse the scenegraph hiearchy
 * as the node_visitor and initating the nodes.
 * 
 * Initate geometries by setting their OpenGL program and calling their
 * initiation functions. Also initates their textures if they have any.
 * 
 * Initates states by adding lights to them (is needed to ignore lights) and 
 * the OpenGL program.
**/ 
class init_visitor : public node_visitor {
    
    public:

        /**
         * Constructor for init_visitor.
         * 
         * @param program The OpenGL program the the scenegraph will use.
         */
        init_visitor(GLuint program);

        /**
         * Visits a group node. Initates its state and visits its children.
         * 
         * @param g The group node.
         */
        virtual void visit(group& g) override;

        /**
         * Visits a transform node. Initates its state and visits its children.
         * 
         * @param t The transform node.
         */
        virtual void visit(transform& t) override;

        /**
         * Visits a transform node. Initates its state and call its initation
         * functions.
         * 
         * @param g The geometry node.
         */
        virtual void visit(geometry& g) override;

        // Setters
        void set_lights(std::vector<std::shared_ptr<light>>& lights);
    
    private:

        GLuint m_program;

        std::vector<std::shared_ptr<light> > m_lights;
};

#endif