/**
 * Node
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - Node is a node in the scenegraph tree hierarchy.
**/

#ifndef __NODE__
#define __NODE__

#include <memory>

#include "node_visitor.h"
#include "bounding_box.h"
#include "update_callback.h"

class state;

#define NODE_GROUP 0
#define NODE_TRANSFORM 1
#define NODE_GEOMETRY 2

/**
 * Abstract class node. Node is a node in the scenegraph tree hierarchy.
**/
class node {
    public:

        /**
         * Accept a node visitor from the visitor design pattern.
         * 
         * @param v Node visitor to visit the geometry.
         */
        virtual void accept(node_visitor& v) = 0;
        
        /**
         * Applies the geometries general update callback method.
         */
        virtual void apply_update_callback() = 0;

        /**
         * Calculates a bounding box for the node.
         * 
         * @return The bounding box.
         */
        virtual bounding_box calculate_bounding_box() = 0;

        // Getters.
        virtual short get_type() = 0;
        virtual std::string get_name() { return m_name; }
        virtual std::shared_ptr<state> get_state() { return m_state; }
        virtual unsigned long get_nr_children() = 0;

        // Setters.
        virtual void set_name(std::string name) { m_name = name; }
        virtual void set_state(std::shared_ptr<state> statee) { m_state = statee; }

    
    private:

        std::string m_name;

    protected:

        std::shared_ptr<state> m_state = nullptr;
};

#endif