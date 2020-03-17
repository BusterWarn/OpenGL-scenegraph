/**
 * [name]
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - 
 **/

#ifndef _BOUNDING_BOX_H
#define _BOUNDING_BOX_H


// Simple class to store and handle a AABB (Axis aligned bounding box)
class bounding_box {

    public:

        /**
         * Constructor for camera.
         * 
         * @param x 
         * @return
         */
        bounding_box(const glm::vec3& min, const glm::vec3&max) : m_max(max), m_min(min) {}

        bounding_box() : m_max(-1E30f), m_min(1E30f) {}

        bounding_box operator*(const glm::mat4& mat) const {

            bounding_box box;

            box.m_min = mat * glm::vec4(m_min, 1);
            box.m_max = mat * glm::vec4(m_max, 1);
            return box;
        }

        void expand(const glm::vec3& v) {

            m_min = glm::min(m_min, v);
            m_max = glm::max(m_max, v);
        }


        void expand(const bounding_box& other) {

            m_min = glm::min(m_min, other.min());
            m_max = glm::max(m_max, other.max());
        }

        glm::vec3 getCenter() {

            return (m_max - m_min) * 0.5f + m_min;
        }

        float getRadius() {

            return glm::length((m_max - m_min)) * 0.5f;
        }

        const glm::vec3& min() const { return m_min; }

        const glm::vec3& max() const { return m_max; }

    private:

        glm::vec3 m_min, m_max, m_center;
};

#endif