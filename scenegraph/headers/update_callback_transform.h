/**
 * Update callback.
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - A function that is tied to a transform node in a scenegraph.
 *                The transform node can apply its update_callback to that node.
**/

#ifndef __UPDATE_CALLBACK_TRANSFORM__
#define __UPDATE_CALLBACK_TRANSFORM__

#include <memory>

#include <GL/glew.h>

#include <glm/glm.hpp>

#include "update_callback.h"

class group;
class transform;
class geometry;
class light;

/**
 * A function that is tied to a transform node in a scenegraph. The transform
 * node can apply its update_callback to that node.
 * 
 * update_callback_transform will update a transform by multiplying its
 * transformation by i.e. rotating it. This way spinning objects can be
 * achieved. 
**/
class update_callback_transform : public update_callback {

    public:

        /**
         * Constructor for update_callback_transform.
         */
        update_callback_transform();

        /**
         * Constructor for update_callback_transform. Constructs it with a
         * specific transformation.
         * 
         * @param transformation The transformation.
         */
        update_callback_transform(glm::mat4& transfromation);

        /**
         * Updates a transform node by multiplying its transformation with this
         * update_callback_transformations transformation.
         * 
         * @param t The group node.
         */
        void update(transform& t) override;

        /**
         * Updates a group node.
         * 
         * @param g The group node.
         */
        void update(group& g) override {}

        /**
         * Updates a geometry node.
         * 
         * @param g The geometry node.
         */
        void update(geometry& g) override {}

        // Getters
        glm::mat4 get_transfromation();
        glm::mat4 get_light_transformation();
        std::shared_ptr<light> get_light();

        // Setters
        void set_transfromation(glm::mat4 transfromation);
        void set_light_transformation(glm::mat4 transfromation);

    private:

        glm::mat4 m_transformation;
        glm::mat4 m_light_transformation;
};
#endif