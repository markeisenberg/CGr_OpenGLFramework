#include "application_solar.hpp"
#include "launcher.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding
using namespace gl;

//setting low and high values for the vertices

int lowv = -70;
int highv = 70;

//Setting low and high values for colors

int lowv_color = 0;
int highv_color = 1;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>                                         //To create random numbers
#include <time.h>                                          //To use system clock as "seed" for random numbers

#define PI 3.1415926535897932384626433832795028841971

#include <iostream>

//Start planet
model planet_model{};
model star_model{};

//based on values from http://nssdc.gsfc.nasa.gov/planetary/factsheet/planet_table_ratio.html
ApplicationSolar::orb sun {0.7f, 0.0f,    0.0f, };
ApplicationSolar::orb mercury {0.05f, 365/88.0f, 15.0f, };
ApplicationSolar::orb venus {0.2f, 365/225.0f, 18.0f, };
ApplicationSolar::orb earth {0.15f, 1.0f, 21.0f, true};
ApplicationSolar::orb mars {0.1f, 365/687.0f, 26.0f, };
ApplicationSolar::orb jupiter {0.35f, 365/4332.0f, 31.0f, };
ApplicationSolar::orb saturn {0.2f, 365/10759.0f, 36.0f, };
ApplicationSolar::orb uranus {0.2f, 365/30688.0f, 40.0f, };
ApplicationSolar::orb neptune {0.2f, 365/60182.0f, 45.0f, };

std::vector<ApplicationSolar::orb> orbContainer = {sun, mercury, earth , venus, mars, jupiter, saturn, uranus, neptune};


std::vector<float> vec_starPos;

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
:Application{resource_path}
,planet_object{}, star_object{}
{
    
    //Random function taken from: http://forums.codeguru.com/showthread.php?351834-how-do-i-generate-a-random-float-between-0-and-1
    
    float x, y, z;
    int r, g, b;
    for (int i = 0; i < 100; i++) {
        //Assign position for a star
        x = ((highv - lowv)*((float)rand() / RAND_MAX)) + lowv;
        y = ((highv - lowv)*((float)rand() / RAND_MAX)) + lowv;
        z = ((highv - lowv)*((float)rand() / RAND_MAX)) + lowv;
        vec_starPos.push_back(x);
        vec_starPos.push_back(y);
        vec_starPos.push_back(z);
        
        //Assign color for a star
        r = rand() % (highv_color - lowv_color + 1) + lowv_color;
        g = rand() % (highv_color - lowv_color + 1) + lowv_color;
        b = rand() % (highv_color - lowv_color + 1) + lowv_color;
        vec_starPos.push_back(r);
        vec_starPos.push_back(g);
        vec_starPos.push_back(b);
        
    }
    
    planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);
    star_model  = {vec_starPos, model::POSITION | model::NORMAL};
    
    // initializeGeometry(planet_model, planet_object);
    // initializeGeometry(star_model, star_object);
    initializeGeometry();
    initializeShaderPrograms();
}
//glm::fmat4 modelTransformationStack; //For later

void ApplicationSolar::upload_planet_transforms(orb &p) const{
    glUseProgram(m_shaders.at("planet").handle);
    
    //rotation period based on time set below
    glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime() * p.rotation), glm::fvec3{0.0f, 1.0f, 0.0f});
    
    //applying necessary scaling and translation
    model_matrix = glm::scale(model_matrix, glm::vec3(p.scaling));
    
    //below the distance difference is changed to declared value from orb struct
    model_matrix = glm::translate(model_matrix, glm::fvec3{p.distance_to_origin, 0.0f, -1.0f});
    
    //Originally from the render activity
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                       1, GL_FALSE, glm::value_ptr(model_matrix));
    
    // extra matrix for normal transformation to keep them orthogonal to surface
    glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                       1, GL_FALSE, glm::value_ptr(normal_matrix));
    
    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);
    
    // draw bound vertex array using bound shader
    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
    
    //Changing the movement around a planet and the sun if the model is a moon
    if(p.lunar)
    {
        // bind new shader
        glUseProgram(m_shaders.at("planet").handle);
        
        model_matrix = glm::rotate(model_matrix, float(glfwGetTime() * 10.0f), glm::fvec3{0.0f, 1.0f, 0.1f});
        
        model_matrix = glm::translate(model_matrix, glm::fvec3{1.5f, 0.0f, -1.0f});
        
        model_matrix = glm::scale(model_matrix, glm::vec3(0.35f));
        
        //Originally from the render activity
        
        glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                           1, GL_FALSE, glm::value_ptr(model_matrix));
        
        // extra matrix for normal transformation to keep them orthogonal to surface
        glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
        glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                           1, GL_FALSE, glm::value_ptr(normal_matrix));
        
        // bind the VAO to draw
        glBindVertexArray(planet_object.vertex_AO);
        
        // draw bound vertex array using bound shader
        glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
    }
}

void ApplicationSolar::render() const {
    
    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("planet").handle);
    
    glBindVertexArray(planet_object.vertex_AO);
    
    for(auto & orb : orbContainer){
        upload_planet_transforms(orb);
    }
    
    glUseProgram(m_shaders.at("star").handle);
    glBindVertexArray(star_object.vertex_AO);
    glDrawArrays(GL_POINTS, 0, 1000);
   // glDisableClientState(GL_VERTEX_ARRAY);
}

void ApplicationSolar::updateView() {
    // vertices are transformed in camera space, so camera transform must be inverted
    glm::fmat4 view_matrix = glm::inverse(m_view_transform);
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                       1, GL_FALSE, glm::value_ptr(view_matrix));
    glUseProgram(m_shaders.at("star").handle);
    glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
                       1, GL_FALSE, glm::value_ptr(view_matrix));
    
    glUseProgram(m_shaders.at("planet").handle);
}

void ApplicationSolar::updateProjection() {
    
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                       1, GL_FALSE, glm::value_ptr(m_view_projection));
    
    glUseProgram(m_shaders.at("star").handle);
    
    glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"),
                       1, GL_FALSE, glm::value_ptr(m_view_projection));
    
    glUseProgram(m_shaders.at("planet").handle);
    
}

void ApplicationSolar::uploadUniforms() {
    updateUniformLocations();
    glUseProgram(m_shaders.at("planet").handle);
    updateView();
    updateProjection();
    
}

// handle key input
void ApplicationSolar::keyCallback(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.1f});
        updateView();
    }
    else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.1f});
        updateView();
    }
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{1.0f, 0.0f, 0.0f});
        updateView();
    }
    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{-1.0f, 0.0f, 0.0f});
        updateView();
    }
    else if (key == GLFW_KEY_UP && action == GLFW_PRESS){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, -1.0f, 0.0f});
        updateView();
    }
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 1.0f, 0.0f});
        updateView();
    }
    else if(key == GLFW_KEY_X && action == GLFW_PRESS){
        m_view_transform = glm::rotate(m_view_transform, 0.05f, glm::fvec3{0.0f, -1.0f, 0.0f});
        updateView();
    }
    else if(key == GLFW_KEY_V && action == GLFW_PRESS){
        m_view_transform = glm::rotate(m_view_transform, 0.05f, glm::fvec3{0.0f, 1.0f, 0.0f});
        updateView();
    }
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
    // mouse handling
}

// load shader programs
void ApplicationSolar::initializeShaderPrograms() {
    // store shader program objects in container
    m_shaders.emplace("planet", shader_program{m_resource_path + "shaders/simple.vert",
        m_resource_path + "shaders/simple.frag"});
    // request uniform locations for shader program
    m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
    m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
    m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
    m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
    
    m_shaders.emplace("star", shader_program{m_resource_path + "shaders/stars.vert",
        m_resource_path + "shaders/stars.frag"});
    // request uniform locations for shader program
    m_shaders.at("star").u_locs["ViewMatrix"] = -1;
    m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;
}

// load models
void ApplicationSolar::initializeGeometry() {
    model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);
    
    // generate vertex array object
    glGenVertexArrays(1, &planet_object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(planet_object.vertex_AO);
    
    // generate generic buffer
    glGenBuffers(1, &planet_object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
    // configure currently bound array buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planet_model.data.size(), planet_model.data.data(), GL_STATIC_DRAW);
    
    // activate first attribute on gpu
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride
    glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::POSITION]);
    // activate second attribute on gpu
    glEnableVertexAttribArray(1);
    // second attribute is 3 floats with no offset & stride
    glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::NORMAL]);
    
    // generate generic buffer
    glGenBuffers(1, &planet_object.element_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
    // configure currently bound array buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * planet_model.indices.size(), planet_model.indices.data(), GL_STATIC_DRAW);
    
    // store type of primitive to draw
    planet_object.draw_mode = GL_TRIANGLES;
    // transfer number of indices to model object
    planet_object.num_elements = GLsizei(planet_model.indices.size());
    
    //
    //
    
    //
    
    // generate vertex array object
    glGenVertexArrays(1, &star_object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(star_object.vertex_AO);
    
    // generate generic buffer
    glGenBuffers(1, &star_object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, star_object.vertex_BO);
    // configure currently bound array buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * star_model.data.size(), star_model.data.data(), GL_STATIC_DRAW);
    
    // activate first attribute on gpu
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride
    glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, star_model.vertex_bytes, star_model.offsets[model::POSITION]);
    // activate second attribute on gpu
    glEnableVertexAttribArray(1);
    // second attribute is 3 floats with no offset & stride
    glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, star_model.vertex_bytes, star_model.offsets[model::NORMAL]);
    
    // generate generic buffer
    glGenBuffers(1, &star_object.element_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, star_object.element_BO);
    // configure currently bound array buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * star_model.indices.size(), star_model.indices.data(), GL_STATIC_DRAW);
    
    
}

ApplicationSolar::~ApplicationSolar() {
    glDeleteBuffers(1, &planet_object.vertex_BO);
    glDeleteBuffers(1, &planet_object.element_BO);
    glDeleteVertexArrays(1, &planet_object.vertex_AO);
    
    glDeleteBuffers(1, &star_object.vertex_BO);
    glDeleteBuffers(1, &star_object.element_BO);
    glDeleteVertexArrays(1, &star_object.vertex_AO);
}

// exe entry point
int main(int argc, char* argv[]) {
    Launcher::run<ApplicationSolar>(argc, argv);
}
