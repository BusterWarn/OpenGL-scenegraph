/**
 * Update callback.
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - A function that is tied to a node in a scenegraph. The node
 *                can apply its update_callback to that node.
**/

#ifndef __UPDATE_CALLBACK__
#define __UPDATE_CALLBACK__

class group;
class transform;
class geometry;

/**
 * Abstract class update_callback. A function that is tied to a node in a
 * scenegraph. The node can apply its update_callback to that node.
**/ 
class update_callback {

    public:

        /**
         * Updates a group node.
         * 
         * @param g The group node.
         */
        virtual void update(group& g) = 0;

        /**
         * Updates a transform node.
         * 
         * @param t The group node.
         */
        virtual void update(transform& t) = 0;

        /**
         * Updates a geometry node.
         * 
         * @param g The geometry node.
         */
        virtual void update(geometry& g) = 0;

        // Getters.
        bool get_update() { return m_update; };

        // Setters.
        void set_update(bool update) { m_update = update; };

    private:

        bool m_update;
};
#endif