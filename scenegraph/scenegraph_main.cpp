/**
 * Scenegraph main file
 * 
 * @Author      - Buster Hultgren Wärn
 * @Date        - 2020-03-17
 * @Description - The main file for an OpenGL scenegraph.
 **/

#include <iostream>
#include <memory>
#include <sstream>

#include <GL/glew.h>
#include <GL/glut.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vr/shaderUtils.h>
#include <vr/DrawText.h>
#include <vr/DrawText.h>
#include <vr/FileSystem.h>

#include "headers/shader_utils.h"
#include "headers/bounding_box.h"

#include "headers/scene.h"
#include "headers/loader.h"
#include "headers/group.h"
#include "headers/state.h"
#include "headers/light.h"

#define RUN_GLUT_MAIN_LOOP
#define GROUND

int last_mx = 0, last_my = 0, cur_mx = 0, cur_my = 0, arcball_on = false;

const int SCREEN_WIDTH = 1280, SCREEN_HEIGHT = 720;

enum MODES { MODE_OBJECT, MODE_CAMERA, MODE_LAST };
MODES view_mode = MODE_CAMERA;

int rotY_direction = 0, rotX_direction = 0, strife = 1;
int transX_direction = 0, transY_direction = 0, transZ_direction = 0;
float g_speed_factor = 1, g_speed_factor_scene_scale = 1;
glm::mat4 transforms[MODE_LAST];


float g_fps = 0;
int last_ticks = 0;
static unsigned int fps_start = glutGet(GLUT_ELAPSED_TIME), fps_frames = 0;


// Parent of all to be rendered
std::shared_ptr<scene> g_scene_root;

std::string g_loaded_filename, g_loaded_vshader, g_loaded_fshader;

// Forward declaring all functions so main can be on top.
bool init_resources(const std::string& model_filename, const std::string& vshader_filename, std::string& fshader_filename);
void init_view();
void reload_scene();
void transform_scene_by_input();
void compute_fps();
void draw();
void draw_fps();
void add_ground();
void on_display();
void on_keyboard(unsigned char c, int /*x*/, int /*y*/);
void on_special(int key, int x, int y);
void on_special_up(int key, int x, int y);
void on_mouse(int button, int state, int x, int y);
void on_motion(int x, int y);
void on_reshape(int width, int height);

/**
 * Main function.
 */
int main(int argc, char* argv[]) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutCreateWindow("scene_graph");
    GLenum glew_status = glewInit();
    
    if (glew_status != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
        return 1;
    }
    if (!GLEW_VERSION_2_0) {
        fprintf(stderr, "Error: your graphic card does not support OpenGL 2.0\n");
        return 1;
    }
    std::string model_filename = (char*) "scenes/party.xml";

    if (argc > 1)
        model_filename = argv[1];

    std::string v_shader_filename = "shaders/phong-shading.vert.glsl";
    std::string  f_shader_filename = "shaders/phong-shading.frag.glsl";

    if (argc < 2 )
        std::cerr << "Usage: " << argv[0] << " <model-file>" << std::endl;

    if (init_resources(model_filename, v_shader_filename, f_shader_filename)) {
        init_view();
        glutDisplayFunc(on_display);
        glutKeyboardFunc(on_keyboard);
        glutSpecialFunc(on_special);
        glutSpecialUpFunc(on_special_up);
        glutMouseFunc(on_mouse);
        glutMotionFunc(on_motion);
        glutReshapeFunc(on_reshape);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        //glDepthFunc(GL_LEQUAL);
        //glDepthRange(1, 0);
        last_ticks = glutGet(GLUT_ELAPSED_TIME);
        
        #ifdef RUN_GLUT_MAIN_LOOP
        glutMainLoop();
        #endif
    }

    return 0;
}

/**
 * Initiates resources.
**/
bool init_resources(const std::string& model_filename,
        const std::string& vshader_filename, std::string& fshader_filename) {

    g_loaded_vshader = vshader_filename;
    g_loaded_fshader = fshader_filename;
    g_loaded_filename = model_filename;

    g_scene_root = scene::get_instance();

    std::string ext = vr::FileSystem::getFileExtension(model_filename);

    // Ok lets load this as our own "scene file format"
    if (ext == "xml" || ext == "XML") {

        if (!load_scene_file(model_filename, g_scene_root)) {
            return false;
        }
        if (g_scene_root->get_nr_geometries() == 0) {
            std::cerr << "Empty scene, something when wrong when loading files" << std::endl;
            return false;
        }
    } else {
        std::shared_ptr<group> node = load_3d_model_file(model_filename);

        if (!node)
            return false;
        g_scene_root->add_node(node);
    }

    #ifdef GROUND

    add_ground();

    #endif

    if (g_scene_root->get_lights().empty()) {
        std::shared_ptr<light> default_light = std::shared_ptr<light>(new light);
        default_light->diffuse = glm::vec4(1,1,1,1);
        default_light->specular = glm::vec4(1,1,1,1);
        default_light->position = glm::vec4(3.0,-2.0,2.0,0.0);
        //default_light->create_geometry();
        g_scene_root->add_light(default_light);
        //g_scene_root->add_node(default_light->create_geometry());
    }

    if (!g_scene_root->init_shaders(vshader_filename, fshader_filename))
        return false;

    fps_start = glutGet(GLUT_ELAPSED_TIME);
    return 1;
}

/**
 * Initiates the camera to look at the scenegraph.
**/
void init_view() {

    bounding_box box = g_scene_root->calculate_bounding_box();
    float radius = box.getRadius();

    float distance = radius * 1.5f;
    g_speed_factor_scene_scale = distance * 0.5f;
    glm::vec3 eye = glm::vec3(0, radius, distance);

    glm::vec3 direction = glm::normalize(box.getCenter() - eye);

    std::shared_ptr<light> light = g_scene_root->get_lights().front();
    glm::vec4 position;
    position = glm::vec4(eye+glm::vec3(3,2,0), 1);
    light->position = position;

    g_scene_root->get_camera()->world2camera = glm::lookAt(
        eye,                            // eye
        direction,                      // direction
        glm::vec3(0.0,  1.0, 0.0));     // up

    g_scene_root->get_camera()->set_near_far( glm::vec2(0.1, distance*20));
}

/**
 * Reloads the scene
**/
void reload_scene() {

    init_resources(g_loaded_filename, g_loaded_vshader, g_loaded_fshader);
    init_view();
}

/**
 * Transform the scene based on user input.
 * 
 * If Object mode is selected then will rotate the whole scene after mouse
 * movement.
 * 
 * If Camera mode is selected then will rotate camera based on mouse movement
 * and translate world based on WASDE+QE keyboard input.
**/
void transform_scene_by_input() {

    /* Handle keyboard-based transformations */
    int delta_t = glutGet(GLUT_ELAPSED_TIME) - last_ticks;
    last_ticks = glutGet(GLUT_ELAPSED_TIME);

    if (view_mode == MODE_OBJECT) {

        /* Handle arcball */
        if (cur_mx != last_mx || cur_my != last_my) {
            glm::vec3 va = g_scene_root->get_camera()->get_arcball_vector(last_mx, last_my);
            glm::vec3 vb = g_scene_root->get_camera()->get_arcball_vector(cur_mx, cur_my);

            float angle = acos(glm::min(1.0f, glm::dot(va, vb)));
            glm::vec3 axis_in_camera_coord = glm::cross(va, vb);
            glm::mat3 camera2object = glm::inverse(glm::mat3(g_scene_root->get_camera()->world2camera) * glm::mat3(1.0f));
            glm::vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;
            
            glm::mat4 transformation = glm::rotate(glm::mat4(1.0f), angle, axis_in_object_coord);
            g_scene_root->transform_root(transformation);
            
            last_mx = cur_mx;
            last_my = cur_my;
        }

    } else if (view_mode == MODE_CAMERA) {
        // Camera is reverse-facing, so reverse Z translation and X rotation.
        // Plus, the View matrix is the inverse of the camera2world (it's
        // world->camera), so we'll reverse the transformations.
        // Alternatively, imagine that you transform the world, instead of positioning the camera.
        
        // Camera tranfsformation
        if (transX_direction != 0) {
            float delta_transX = transX_direction * delta_t / 1000.0f * 3 * g_speed_factor*g_speed_factor_scene_scale;  // 3 units per second
            g_scene_root->get_camera()->world2camera = glm::translate(glm::mat4(1.0), glm::vec3(delta_transX, 0.0f, 0.0f)) * g_scene_root->get_camera()->world2camera;
            transX_direction = 0;
        }
        if (transY_direction != 0) {
            float delta_transY = transY_direction * delta_t / 1000.0f * 3 * g_speed_factor*g_speed_factor_scene_scale;  // 3 units per second 
            g_scene_root->get_camera()->world2camera = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, delta_transY, 0.0f)) * g_scene_root->get_camera()->world2camera;
            transY_direction = 0;
        }
        if (transZ_direction != 0) {
            float delta_transZ = transZ_direction * delta_t / 1000.0f * 5 * g_speed_factor*g_speed_factor_scene_scale;  // 5 units per second
            g_scene_root->get_camera()->world2camera = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, delta_transZ)) * g_scene_root->get_camera()->world2camera;
            transZ_direction = 0;
        }
        

        // Camera rotation
        if (cur_my != last_my) {
            
            g_scene_root->get_camera()->rotate_x(float(last_mx - cur_mx) / 20);
            last_mx = cur_mx;
        }
        if (cur_my != last_my) {
            
            g_scene_root->get_camera()->rotate_y(float(last_my - cur_my) / 20);
            last_my = cur_my;
        }
    }


    g_scene_root->use_program();
    g_scene_root->apply_camera();

    glutPostRedisplay();
}

/**
 * Compute fps in scenegraph.
**/
void compute_fps() {
    /* FPS count */
    {
        fps_frames++;
        int delta_t = glutGet(GLUT_ELAPSED_TIME) - fps_start;
        if (delta_t > 50) {
            g_fps = 1000.0f * fps_frames / delta_t;

            fps_frames = 0;
            fps_start = glutGet(GLUT_ELAPSED_TIME);
        }
    }
}

/**
 * Draws the entire scene together with FPS.
**/
void draw() {

    glClearColor(0.45f, 0.45f, 0.45f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw_fps();
    g_scene_root->draw();
}

/**
 * Draws computed fps.
**/
void draw_fps() {

    std::ostringstream str;
    str << "FPS: " << g_fps << std::ends;

    vr::Text::setColor(glm::vec4(1, 1, 0, 0.8));
    vr::Text::setFontSize(20);
    vr::Text::drawText(10, 20, str.str().c_str());


    std::string mode = "";
    if (view_mode == MODE_CAMERA)
        mode = "Camera Mode";
    else if (view_mode == MODE_OBJECT)
        mode = "Object Mode";

    vr::Text::drawText(10, 40, mode.c_str());
}

/**
 * Adds a ground to the scene that should cover every geometry.
**/
void add_ground() {
    
    std::shared_ptr<geometry> ground(new geometry);

    bounding_box box = g_scene_root->calculate_bounding_box();
    glm::vec3 size = box.max() - box.min();

    glm::vec4 v1 = glm::vec4(-size.x, 0, -size.y, 1.0);   
    glm::vec4 v2 = glm::vec4(size.x, 0, -size.y, 1.0);   
    glm::vec4 v3 = glm::vec4(-size.x, 0, size.y, 1.0);   
    glm::vec4 v4 = glm::vec4(-size.x, 0, size.y, 1.0);   
    glm::vec4 v5 = glm::vec4(size.x, 0, -size.y, 1.0);   
    glm::vec4 v6 = glm::vec4(size.x, 0, size.y, 1.0);
    glm::vec3 normal = glm::vec3(0.0, 1.0, 0.0);
    ground->add_vertice_with_normal(v1, normal);
    ground->add_vertice_with_normal(v2, normal);
    ground->add_vertice_with_normal(v3, normal);
    ground->add_vertice_with_normal(v4, normal);
    ground->add_vertice_with_normal(v5, normal);
    ground->add_vertice_with_normal(v6, normal);

    ground->get_state()->set_cull_face_mode(GL_FRONT);
    ground->get_state()->set_material(std::shared_ptr<material>(new material));

    std::shared_ptr<group> ground_node = std::shared_ptr<group>(new group);
    ground_node->add_child(ground);

    // ground_node->set_initial_transform(glm::translate(glm::mat4(),
    //     glm::vec3(box.getCenter().x, box.min().y, box.getCenter().z))); TODO initial transform?

    g_scene_root->add_node(ground_node);
}

/**
 * Handles when OpenGL calls for scenegraph to display.
**/
void on_display() {
    
    compute_fps();
    transform_scene_by_input();
    draw();
    glutSwapBuffers();
}

/**
 * Handles keyboard input.
**/
void on_keyboard(unsigned char c, int /*x*/, int /*y*/) {
    switch (c) {
        case 'r':
            reload_scene();
            break;
        // Camera translation
        case 'a':
        case 'A':
            transX_direction = 1;
            break;
        case 'd':
        case 'D':
            transX_direction = -1;
            break;
        case 'q':
        case 'Q':
            transY_direction = 1;
            break;
        case 'e':
        case 'E':
            transY_direction = -1;
            break;
        case 'w':
        case 'W':
            transZ_direction = 1;
            break;
        case 's':
        case 'S':
            transZ_direction = -1;
            break;
        case 27:
            exit(0);
            break;
        default:
            break;
    }
}

/**
 * Handles special keyboard input.
**/
void on_special(int key, int x, int y) {

    int modifiers = glutGetModifiers();
    if ((modifiers & GLUT_ACTIVE_ALT) == GLUT_ACTIVE_ALT)
        strife = 0;
    else
        strife = 0;
    if ((modifiers & GLUT_ACTIVE_SHIFT) == GLUT_ACTIVE_SHIFT)
        g_speed_factor = 1.0f;
    else
        g_speed_factor = 0.2f;
    switch (key) {
        case GLUT_KEY_F1:
            view_mode = MODE_OBJECT;
            break;
        case GLUT_KEY_F2:
            view_mode = MODE_CAMERA;
            break;
        case GLUT_KEY_LEFT:
            rotY_direction = 1;
            break;
        case GLUT_KEY_RIGHT:
            rotY_direction = -1;
            break;
        case GLUT_KEY_UP:
            transZ_direction = 1;
            break;
        case GLUT_KEY_DOWN:
            transZ_direction = -1;
            break;
        case GLUT_KEY_END:
            init_view();
            break;
        case GLUT_KEY_HOME:
            init_view();
            break;
        default:
            break;
    }
}

/**
 * Handles release of special keyboard input.
**/
void on_special_up(int key, int x, int y) {

    switch (key) {
        case GLUT_KEY_LEFT:
        case GLUT_KEY_RIGHT:
            rotY_direction = 0;
            break;
        case GLUT_KEY_UP:
        case GLUT_KEY_DOWN:
            transZ_direction = 0;
            break;
        case GLUT_KEY_PAGE_UP:
        case GLUT_KEY_PAGE_DOWN:
            rotX_direction = 0;
            break;
    }
}

/**
 * Handles mouse button input.
**/
void on_mouse(int button, int state, int x, int y) {
    
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        arcball_on = true;
        last_mx = cur_mx = x;
        last_my = cur_my = y;
    } else {
        arcball_on = false;
    }
}

/**
 * Handles mouse input if moved.
**/
void on_motion(int x, int y) {
    if (arcball_on) {  // if left button is pressed
        cur_mx = x;
        cur_my = y;
    }
}

/**
 * Handles if screen is resized.
**/
void on_reshape(int width, int height) {

    g_scene_root->get_camera()->set_screen_size(glm::uvec2(width, height));
    glViewport(0, 0, width, height);
}