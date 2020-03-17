/**
 * Node visitor
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - Abstract node visitor following the visitor design pattern.
 *                Node visitor will visit the scenegraph hiearchy.
 **/

#ifndef __NODE_VISITOR__
#define __NODE_VISITOR__

class node;
class group;
class transform;
class geometry;

/**
 * Node visitor abstract class. Abstract node visitor following the visitor
 * design pattern. Node visitor will visit the scenegraph hiearchy.
**/
class node_visitor {
    
    public:

        /**
         * Visits a group node.
         * 
         * @param g The group node.
         */
        virtual void visit(group& g) = 0;

        /**
         * Visits a transform node.
         * 
         * @param t The transform node.
         */
        virtual void visit(transform& t) = 0;
        
        /**
         * Visits a geometry node.
         * 
         * @param g The geometry node.
         */
        virtual void visit(geometry& g) = 0;
};

#endif