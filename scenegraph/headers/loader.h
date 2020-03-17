/**
 * Loaders
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - Loads object files and XML files into node objects for the 
 *                scenegraph. Object files will become geometries collected by
 *                a group node and XML files will load a full scenegraph.
 **/

#ifndef __LOADER__
#define __LOADER__

#include <memory>

#include "group.h"

class scene;

/**
 * Load a given object file as a collection of geometry nodes in a group node.
 * 
 * @param filename Name of the objectfile to load. Should end with '.obj'.
 * @return Loaded object in a group.
 */
std::shared_ptr<group> load_3d_model_file(const std::string& filename);

/**
 * Load a given XML file containing scene data and store it in a scene pointer.
 * 
 * @param xml_file Filename of the XML file containing the scene data.
 * @parm scene The scene.
 */
bool load_scene_file(const std::string& xml_file, std::shared_ptr<scene>& scene);

#endif