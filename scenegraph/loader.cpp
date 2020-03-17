#include <iostream>
#include <stack>
#include <memory>

#include <sstream>
#include <fstream>
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>

#include <GL/glew.h>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h> 

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vr/FileSystem.h>
#include <vr/shaderUtils.h>
#include <vr/glUtils.h>

#include "headers/material.h"
#include "headers/scene.h"
#include "headers/loader.h"
#include "headers/state.h"
#include "headers/light.h"
#include "headers/group.h"
#include "headers/transform.h"
#include "headers/geometry.h"
#include "headers/update_callback_transform.h"

size_t extract_materials(const aiScene *scene, std::vector<std::shared_ptr<material>>& materials) {

    uint32_t num_materials = scene->mNumMaterials;
    aiMaterial *ai_material;
    aiColor4D color(0.0f, 0.0f, 0.0f, 1.0f);
    GLfloat shiniess;
    aiString path;

    for (uint32_t i = 1; i < num_materials; i++) {

        std::shared_ptr<material> mat = std::shared_ptr<material>(new material);

        ai_material = scene->mMaterials[i];
        
        //Assimp cannot read when no material are used and will create its own with bugged valued. This is hardcoded
        //to remove those and use programs instead.
        bool bugged_material = true;

        ai_material->Get(AI_MATKEY_COLOR_AMBIENT, color);
        mat->set_ambient(glm::vec4(color.r, color.g, color.b, color.a));
        if (color.r != 0.0f || color.b != 0.0f || color.g != 0.0f)
            bugged_material = false;

        ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        mat->set_diffuse(glm::vec4(color.r, color.g, color.b, color.a));
        if (color.r != 0.6f || color.b != 0.6f || color.g != 0.6f)
            bugged_material = false;
        
        ai_material->Get(AI_MATKEY_COLOR_SPECULAR, color);
        mat->set_specular(glm::vec4(color.r, color.g, color.b, color.a));
        if (color.r != 0 || color.b != 0 || color.g != 0)
            bugged_material = false;
        
        ai_material->Get(AI_MATKEY_SHININESS, shiniess);
        mat->set_shininess(shiniess);
        if (shiniess != 0)
            bugged_material = false;
        
        if (!bugged_material) {
            materials.push_back(mat);
        }
    }

    return materials.size();
}

glm::mat4 assimpToGlmMatrix(const aiMatrix4x4 &ai_matrix) {
  
    glm::mat4 glm_matrix;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            glm_matrix[i][j] = ai_matrix[j][i];
        }
    }

    return glm_matrix;
}

template<class T>
T readValue(const std::string& string) {
    std::stringstream ss;
    ss << string;
    T result;
    ss >> result;
    return result;
}

template < class ContainerT >
void tokenize(const std::string& str, ContainerT& tokens,
    const std::string& delimiters = " ", bool trimEmpty = false) {

    std::string::size_type pos, lastPos = 0, length = str.length();

    using value_type = typename ContainerT::value_type;
    using size_type = typename ContainerT::size_type;

    while (lastPos < length + 1) {
    
        pos = str.find_first_of(delimiters, lastPos);

        if (pos == std::string::npos)
            pos = length;

        if (pos != lastPos || !trimEmpty)
            tokens.push_back(value_type(str.data() + lastPos, (size_type)pos - lastPos));

        lastPos = pos + 1;
    }
}

template<class T>
bool get_vec(T& vec, const std::string& string, const T& def = T()) {
    
    vec = def;

    std::vector<std::string> tokens;
    tokenize(string, tokens, " ", true);

    if (tokens.size() > 0 && tokens.size() != T::length())
        return false;

    int i = 0;
    for (auto v : tokens) {
        float val = readValue<float>(v);
        vec[i++] = val;
    }

    return true;
}


std::string getAttribute(rapidxml::xml_node<> * node, const std::string& attribute) {
    if (!node)
        return "";

    rapidxml::xml_attribute<> *attrib = node->first_attribute(attribute.c_str());
    if (!attrib)
        return "";

    return attrib->value();
}

std::string path_to_string(std::vector<std::string>& path) {

  std::string result;
  for (auto s : path)
    result = result + "/" + s;

  return result;
}

void parseNodes(aiNode *root_node, std::vector<std::shared_ptr<material>>& materials, std::stack<glm::mat4>& transformStack,
        std::shared_ptr<group>& node, const aiScene *aiScene) {

    glm::mat4 transform_matrix = assimpToGlmMatrix(root_node->mTransformation);

    glm::mat4 m = transformStack.top()*transform_matrix;
    transformStack.push(m);

    uint32_t num_meshes = root_node->mNumMeshes;

    std::vector<glm::vec3> tangents;
    std::vector<glm::vec2> tex_coords;

    for (uint32_t i = 0; i < num_meshes; i++) {
        
        std::shared_ptr<geometry> loadedMesh(new geometry);

        aiMesh *mesh = aiScene->mMeshes[root_node->mMeshes[i]];
        uint32_t num_vertices = mesh->mNumVertices;

        loadedMesh->vertices.resize(num_vertices);
        loadedMesh->normals.resize(num_vertices);
        tangents.resize(num_vertices);
        tex_coords.resize(num_vertices);

        for (uint32_t j = 0; j < num_vertices; j++) {
                
            loadedMesh->vertices[j] = glm::vec4(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z, 1);
            loadedMesh->normals[j] = glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
            glm::vec3 tangent;
            if (mesh->HasTangentsAndBitangents()) {
                tangent.x = mesh->mTangents[j].x;
                tangent.y = mesh->mTangents[j].y;
                tangent.z = mesh->mTangents[j].z;
            }
            tangents[i] = tangent;

            glm::vec2 tex_coord;
            if (mesh->mTextureCoords[0]) {

                tex_coord.x = mesh->mTextureCoords[0][j].x;
                tex_coord.y = mesh->mTextureCoords[0][j].y;
                loadedMesh->texture.push_back(tex_coord);
            }
        }
        uint32_t num_faces = mesh->mNumFaces;
        loadedMesh->elements.resize(0);

        for (uint32_t j = 0; j < num_faces; j++) {

            aiFace face = mesh->mFaces[j];
            uint32_t num_indices = face.mNumIndices;

            for (uint32_t k = 0; k < num_indices; k++) {

                loadedMesh->elements.push_back(face.mIndices[k]);
            }
        }
        //loadedMesh->obj2world = transformStack.top();

        if (!materials.empty()) {
            loadedMesh->get_state()->set_material(materials[mesh->mMaterialIndex - 1]);
        } else {
            loadedMesh->get_state()->set_material(std::shared_ptr<material>(new material));
        }

        node->add_child(loadedMesh);
    }

    for (uint32_t i = 0; i < root_node->mNumChildren; i++) {

        parseNodes(root_node->mChildren[i], materials, transformStack, node, aiScene);
    }
    transformStack.pop();
}

std::shared_ptr<group> load_3d_model_file(const std::string& filename) {

    std::string filepath = filename;
    bool exist = vr::FileSystem::exists(filepath);

    std::string vrPath = vr::FileSystem::getEnv("VR_PATH");
    if (vrPath.empty())
        std::cerr << "The environment variable VR_PATH is not set. It should point to the directory where the vr library is (just above models)" << std::endl;

    if (!exist && !vrPath.empty()) {
        filepath = std::string(vrPath) + "/" + filename;
        exist = vr::FileSystem::exists(filepath);
    }

    if (!exist) {
        std::cerr << "The file " << filename << " does not exist" << std::endl;
        return nullptr;
    }

    std::vector<std::shared_ptr<material>> materials;


    Assimp::Importer importer;
    const aiScene* aiScene = importer.ReadFile(filepath,
        aiProcess_CalcTangentSpace |
        aiProcess_GenSmoothNormals |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);

    if (!aiScene) {
        std::cerr << "Couldn't load model: " << filepath << " Error Importing Asset: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    aiNode *root_node = aiScene->mRootNode;

    extract_materials(aiScene, materials);

    std::stack<glm::mat4> transformStack;
    transformStack.push(glm::mat4());

    std::shared_ptr<group> group_node = std::shared_ptr<group>(new group);
    group_node->set_name("root");
    parseNodes(root_node, materials, transformStack, group_node, aiScene);
    transformStack.pop();

    if (group_node->get_children().empty())
        std::cerr << " File " << filepath << " did not contain any mesh data" << std::endl;
    
    return group_node;
}

bool load_scene_file(const std::string& xml_file, std::shared_ptr<scene>& scene) {
    std::string filepath = xml_file;
    bool exist = vr::FileSystem::exists(filepath);

    std::string vrPath = vr::FileSystem::getEnv("VR_PATH");
    if (vrPath.empty())
        std::cerr << "The environment variable VR_PATH is not set. It should point to the directory where the vr library is (just above models)" << std::endl;

    if (!exist && !vrPath.empty()) {
        filepath = std::string(vrPath) + "/" + xml_file;
        exist = vr::FileSystem::exists(filepath);
    }

    if (!exist) {
        std::cerr << "The file " << xml_file << " does not exist" << std::endl;
        return false;
    }


    rapidxml::xml_node<>* root_node = nullptr;
    std::vector<std::string> xmlpath;

    try {
        rapidxml::file<> xmlFile(filepath.c_str()); // Default template is char
        rapidxml::xml_document<> doc;

        doc.parse<rapidxml::parse_trim_whitespace
                | rapidxml::parse_normalize_whitespace
                | rapidxml::parse_full> (xmlFile.data());

        root_node = doc.first_node("scene");
        if (!root_node)
            throw std::runtime_error("File missing scene/");

        xmlpath.push_back("scene");


        /*              Iterate over the nodes              */
        for (rapidxml::xml_node<> * node_node = root_node->first_node("node");
                node_node; node_node = node_node->next_sibling()) {
            
            xmlpath.push_back("node");

            if (node_node->type() == rapidxml::node_comment || node_node->type() == rapidxml::node_doctype)
                continue;





            /*              name              */
            std::string name = getAttribute(node_node, "name"); 

            if (name.rfind("light") == 0) {
                xmlpath.pop_back(); // scene_node
                continue;
            }

            rapidxml::xml_node<>* file = node_node->first_node("file");
            if (!file)
                throw std::runtime_error("Missing file: " + path_to_string(xmlpath));

            xmlpath.push_back("file");

            std::string path = file->first_attribute("path")->value();
            if (path.empty())
                throw std::runtime_error("Empty path: " + path_to_string(xmlpath));

            xmlpath.pop_back(); // file



            
            /*              node              */
            std::shared_ptr<group> group_node = load_3d_model_file(path);
            if (!group_node)
                std::cerr << "Unable to load node \'" << name << "\' path: " << path << std::endl;
            std::shared_ptr<state> node_state = group_node->get_state();
            
            rapidxml::xml_node<> * no_light = node_node->first_node("no_light");
            if (no_light) {

                std::vector< std::shared_ptr<node>> children = group_node->get_children();

                xmlpath.push_back("no_light");
                std::string id = getAttribute(no_light, "id");
                std::stringstream iss( id );

                int number;
                while ( iss >> number ) {
                    for (auto child : group_node->get_children()) {
                        child->get_state()->disable_light(number);
                    }
                }

                xmlpath.pop_back(); // no_light
            }

            /*              draw              */
            rapidxml::xml_node<>* draw = node_node->first_node("draw");
            
            if (draw) {
                xmlpath.push_back("draw");

                std::string attribute_value = getAttribute(draw, "texture_path");

                if (!attribute_value.empty()) {
                    for (auto child : group_node->get_children()) {
                        child->get_state()->set_texture(std::shared_ptr<texture>(new texture(attribute_value)));
                    }
                }
                    //node_state->set_texture(std::shared_ptr<texture>(new texture(attribute_value)));
                
                attribute_value = getAttribute(draw, "polygon_mode");
                if (!attribute_value.empty()) {
                    if (attribute_value.compare("wireframe") == 0)
                        node_state->set_polygon_mode(GL_LINE);
                    else if (attribute_value.compare("points") == 0)
                        node_state->set_polygon_mode(GL_POINT);
                    else if (attribute_value.compare("filled") == 0)
                        node_state->set_polygon_mode(GL_FILL);
                }

                attribute_value = getAttribute(draw, "cull_face_mode");
                if (!attribute_value.empty()) {
                    if (attribute_value.compare("front") == 0) {
                        node_state->set_cull_face_mode(GL_FRONT);
                    } else if (attribute_value.compare("back") == 0) {
                        node_state->set_cull_face_mode(GL_BACK);
                    } else if (attribute_value.compare("front_and_back") == 0)
                        node_state->set_cull_face_mode(GL_FRONT_AND_BACK);
                }
                xmlpath.pop_back(); // draw
            }



            /*              transform              */
            rapidxml::xml_node<> * xml_transform = node_node->first_node("transform");
            if (xml_transform) {
                std::shared_ptr<transform> transform_node = std::shared_ptr<transform>(new transform(group_node));

                xmlpath.push_back("transform");

                std::string translate = getAttribute(xml_transform, "translate");
                glm::vec3 t_vec;
                if (!get_vec<glm::vec3>(t_vec, translate))
                    throw std::runtime_error("Node (" + name + ") Invalid translate in: " + path_to_string(xmlpath));

                std::string rotate = getAttribute(xml_transform, "rotate");
                glm::vec3 r_vec;
                if (!get_vec<glm::vec3>(r_vec, rotate))
                    throw std::runtime_error("Node (" + name + ") Invalid rotate in: " + path_to_string(xmlpath));

                std::string scale = getAttribute(xml_transform, "scale");
                glm::vec3 s_vec;
                if (!get_vec<glm::vec3>(s_vec, scale, glm::vec3(1)))
                    throw std::runtime_error("Node (" + name + ") Invalid scale in: " + path_to_string(xmlpath));
                
                std::string spin = getAttribute(xml_transform, "spin");
                glm::vec3 spin_vec;
                bool add_update_callback = false;
                if (!get_vec<glm::vec3>(spin_vec, spin))
                    throw std::runtime_error("Node (" + name + ") Invalid spin in: " + path_to_string(xmlpath));
                else
                    add_update_callback = true;

                if (!transform_node) {
                    std::cerr << "Unable to load node \'" << name << "\' path: " << path << std::endl;
                } else {
                    glm::mat4 mt = glm::translate(glm::mat4(), t_vec);
                    glm::mat4 ms = glm::scale(glm::mat4(), s_vec);
                    glm::mat4 rx = glm::rotate(glm::mat4(), glm::radians(r_vec.x), glm::vec3(1, 0, 0));
                    glm::mat4 ry = glm::rotate(glm::mat4(), glm::radians(r_vec.y), glm::vec3(0, 1, 0));
                    glm::mat4 rz = glm::rotate(glm::mat4(), glm::radians(r_vec.z), glm::vec3(0, 0, 1));
                    
                    transform_node->set_transformation(std::shared_ptr<glm::mat4>(new glm::mat4(mt * ms * rz * ry * rx)));
                    transform_node->set_name("Transformation");
                    
                    if (add_update_callback) {
                        rx = glm::rotate(glm::mat4(), glm::radians(spin_vec.x), glm::vec3(1, 0, 0));
                        rx = rx * glm::rotate(glm::mat4(), glm::radians(spin_vec.y), glm::vec3(0, 1, 0));
                        rx = rx * glm::rotate(glm::mat4(), glm::radians(spin_vec.z), glm::vec3(0, 0, 1));
                        std::shared_ptr<update_callback_transform> u_c_t =
                            std::shared_ptr<update_callback_transform>(new update_callback_transform(rx));

                        transform_node->set_update_callback(u_c_t);
                    }
                    
                }
                scene->add_node(transform_node);
                xmlpath.pop_back(); // transform

            } else {

                scene->add_node(group_node);
            }


            xmlpath.pop_back(); // node
        }

        /*              Iterate over the lights              */
        for (rapidxml::xml_node<> * node_node = root_node->first_node("light");
                node_node; node_node = node_node->next_sibling()) {
            
            xmlpath.push_back("light");

            if (node_node->type() == rapidxml::node_comment || node_node->type() == rapidxml::node_doctype)
                continue;

            std::string name = getAttribute(node_node, "name"); 

            if (name.rfind("light") != 0) {
                xmlpath.pop_back(); // light
                continue;
            }

            std::string position = getAttribute(node_node, "position");
            std::string diffuse = getAttribute(node_node, "diffuse");
            std::string specular = getAttribute(node_node, "specular");
            std::string spin = getAttribute(node_node, "spin");

            glm::vec3 pos_vec;
            glm::vec3 diff_vec;
            glm::vec3 spec_vec;
            glm::vec3 spin_vec;
            
            if (!get_vec<glm::vec3>(pos_vec, position))
                throw std::runtime_error("Light (" + name + ") Invalid position in: " + path_to_string(xmlpath));
            
            if (!get_vec<glm::vec3>(diff_vec, diffuse))
                throw std::runtime_error("Light (" + name + ") Invalid diffuse in: " + path_to_string(xmlpath));
            
            if (!get_vec<glm::vec3>(spec_vec, specular))
                throw std::runtime_error("Light (" + name + ") Invalid specular in: " + path_to_string(xmlpath));

            if (!get_vec<glm::vec3>(spin_vec, specular))
                throw std::runtime_error("Light (" + name + ") Invalid spin in: " + path_to_string(xmlpath));
            
            std::shared_ptr<light> new_light = std::shared_ptr<light>(new light);
            new_light->position = glm::vec4(pos_vec, 0.0);
            new_light->diffuse = glm::vec4(diff_vec, 1);
            new_light->specular = glm::vec4(spec_vec, 1);

            scene->add_light(new_light);
            std::shared_ptr<geometry> light_geometry = new_light->create_geometry();

            std::shared_ptr<transform> light_transform = std::shared_ptr<transform>(new transform);
            light_transform->set_transformation(std::shared_ptr<glm::mat4>(new glm::mat4(
                glm::translate(glm::mat4(), glm::vec3(new_light->position))
                * glm::scale(glm::mat4(), glm::vec3(0.2f, 0.2f, 0.2f))
            )));
            light_transform->add_child(light_geometry);

            if (!spin.empty()) {             

                std::shared_ptr<transform> transform_node = std::shared_ptr<transform>(new transform);
                glm::mat4 t = t * glm::translate(glm::mat4(), glm::vec3(
                    spin_vec.x / 2,
                    spin_vec.y / 2,
                    spin_vec.z / 2));
                transform_node->multiply_transformation(t);

                glm::mat4 rx = glm::rotate(glm::mat4(), glm::radians(spin_vec.x), glm::vec3(1.0f, 0.0f, 0.0f));
                rx = rx * glm::rotate(glm::mat4(), glm::radians(spin_vec.y), glm::vec3(0.0f, 1.0f, 0.0f));
                rx = rx * glm::rotate(glm::mat4(), glm::radians(spin_vec.z), glm::vec3(0.0f, 0.0f, 1.0f));
                
                std::shared_ptr<update_callback_transform> u_c_t =
                            std::shared_ptr<update_callback_transform>(new update_callback_transform(rx));
                transform_node->set_update_callback(u_c_t);

                transform_node->add_child(light_transform);
                light_transform = transform_node;
            }
            
            scene->add_node(light_transform);
            
            xmlpath.pop_back(); // light
        }
        xmlpath.pop_back(); // scene

    } catch (rapidxml::parse_error& error) {
        std::cerr << "XML parse error: " << error.what() << std::endl;
        return false;
    } catch (std::runtime_error& error) {
        std::cerr << "XML parse error: " << error.what() << std::endl;
        return false;
    }
    return true;
}
