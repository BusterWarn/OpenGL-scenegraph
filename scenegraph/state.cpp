/**
 * State
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - A state for a node in the scenegraph. Contains information
 * such as lights to ignore, texture, material and OpenGL draw settings.
**/

#include <iostream>
#include <algorithm>

#include "headers/state.h"

state::state(GLint program, short polygon_mode,short cull_face_mode):
        m_program(program),
        m_polygon_mode(polygon_mode),
        m_cull_face_mode(cull_face_mode) {
    
    m_material = nullptr;
    m_texture = nullptr;
}

void state::apply() {
    
    // Apply lightsources
    size_t i = 0;
    for (auto l : m_lights) {
        if ((std::find(m_disabled_lights.begin(), m_disabled_lights.end(), i) != m_disabled_lights.end()))
            l->disable();
        else
            l->enable();
        
        l->apply(m_program, i++);
    }

    if (m_material)
        m_material->apply(m_program);

    if (m_cull_face_mode != 0) {
        glEnable(GL_CULL_FACE);
        glCullFace(m_cull_face_mode);
    }

    if (m_polygon_mode != 0)
        glPolygonMode(GL_FRONT_AND_BACK, m_polygon_mode);

}

void state::disable_light(int id) {

    m_disabled_lights.push_back(id);
};

void state::set_texture(std::shared_ptr<texture> texture) {
    m_texture = texture;
    if (m_material)
        m_material->set_has_diffuse_map(true);
}

void state::set_program(GLint program)                              { m_program = program; }
void state::set_lights(std::vector<std::shared_ptr<light>>& lights) { m_lights = lights; }
void state::set_material(std::shared_ptr<material> material)        { m_material = material; }
void state::set_polygon_mode(GLenum polygon_mode)                   { m_polygon_mode = polygon_mode; }
void state::set_cull_face_mode(GLenum cull_face_mode)               { m_cull_face_mode = cull_face_mode; }