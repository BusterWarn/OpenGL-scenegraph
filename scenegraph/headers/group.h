/**
 * Group
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - A group node in a scenegraph.
 **/

#ifndef __GROUP__
#define __GROUP__

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "node.h"

#include "update_callback_group.h"

/**
 * Group class. Derived from node.
 * 
 * A group node in scenegraph. The group functions as a collection of nodes
 * for some purpose. I.e. they could all be part of a larger object.
**/
class group : public node {
    
    public:

        /**
         * Constructor for group.
         */
        group();
        virtual void accept(node_visitor& v) override;

        /**
         * Applies the geometries general update callback method.
         */
        virtual void apply_update_callback() override;

        /**
         * Calculates a bounding box around the group and it's children.
         * 
         * @return A new bounding box.
         */
        virtual bounding_box calculate_bounding_box() override;

        /**
         * Adds a child to the group.
         * 
         * @param n - The node child. This could be a group, a transform or a
         * geometry.
         */
        void add_child(std::shared_ptr<node> n);
        
        // Getters
        virtual short get_type() override;
        virtual unsigned long get_nr_children() override;
        std::vector<std::shared_ptr<node> >& get_children();
    
    private:

        std::vector< std::shared_ptr<node> > m_children;

        update_callback_group m_update_callback;
};

#endif