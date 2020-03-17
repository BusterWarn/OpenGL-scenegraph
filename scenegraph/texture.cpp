/**
 * Texture
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - Texture for a geometry in the scenegraph.
**/

#include <iostream>

#include "headers/texture.h"

texture::texture(std::string& filename): m_filename(filename) {

}

void texture::set_texture(std::string& filename) { m_filename = filename; }