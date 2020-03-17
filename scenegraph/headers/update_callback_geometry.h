/**
 * Update callback geometry.
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - A function that is tied to a node in a scenegraph. The node
 *                can apply its update_callback to that node.
 * @warning     - NOT YET IMPLEMENTED.
**/

#ifndef __UPDATE_CALLBACK_GEOMETRY__
#define __UPDATE_CALLBACK_GEOMETRY__

#include "update_callback.h"

/**
 * Class update_callback_geometry. A function that is tied to a node in a
 * scenegraph. The node can apply its update_callback to that node.
**/ 
class update_callback_geometry : public update_callback {

    public:

        /**
         * Constructor for update_callback_geometry.
         */
        update_callback_geometry() {}

        /**
         * Updates a group node.
         * 
         * @param g The group node.
         */
        void update(transform& t) override {}

        /**
         * Updates a transform node.
         * 
         * @param t The group node.
         */
        void update(group& g) override {}

        /**
         * Updates a geometry node.
         * 
         * @param g The geometry node.
         */
        void update(geometry& g) override {}
};
#endif