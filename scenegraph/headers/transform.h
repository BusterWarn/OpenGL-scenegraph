/**
 * Transform
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - A transform node in a scenegraph.
 **/

#ifndef __TRANSFORM__
#define __TRANSFORM__

#include <memory>

#include <GL/glew.h>
#include <GL/glut.h>

#include <glm/glm.hpp>

#include "group.h"

#include "update_callback_transform.h"

/**
 * Transform class. Derived from group.
 * 
 * A transform node in a scenegraph.
**/ 
class transform : public group {
    
    public:

        /**
         * Constructor for geometry.
         */
        transform();
        /**
         * Constructor for geometry. Creates it from a group node and copy that
         * nodes data.
         */
        transform(std::shared_ptr<group> group);

        /**
         * Accept a node visitor from the visitor design pattern.
         * 
         * @param v Node visitor to visit the geometry.
         */
        void accept(node_visitor& v) override;

        /**
         * Applies the geometries general update callback method.
         */
        void apply_update_callback() override;

        /**
         * Calculates a bounding box for the transform.
         * 
         * @return The bounding box.
         */
        virtual bounding_box calculate_bounding_box() override;

        /**
         * Multiplies the transformations transformation with a new
         * transformation from left to right:
         * my_transformation = my_transformation * transformation;
         * 
         * @transformation The transformation.
         */
        void multiply_transformation(std::shared_ptr<glm::mat4> transformation);

        /**
         * Multiplies the transformations transformation with a new
         * transformation from left to right:
         * my_transformation = my_transformation * transformation;
         * 
         * @transformation The transformation.
         */
        void multiply_transformation(glm::mat4& transformation);
        
        // Getters
        short get_type() override;
        std::shared_ptr<glm::mat4> get_transformation();
        std::shared_ptr<update_callback_transform> get_update_callback();

        // Setters
        void set_update_callback(std::shared_ptr<update_callback_transform> update_callback);
        void set_transformation(std::shared_ptr<glm::mat4> transformation);

    private:

        std::shared_ptr<glm::mat4> m_transformation;

        std::shared_ptr<update_callback_transform> m_update_callback;
};

#endif