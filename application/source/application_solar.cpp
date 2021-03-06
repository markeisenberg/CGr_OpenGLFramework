#include "application_solar.hpp"
#include "launcher.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"
#include "texture_loader.hpp"
#include "structs.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding
using namespace gl;

//setting low and high values for the vertices

int lowv = -70;
int highv = 70;

//Setting low and high values for colors

int lowv_color = 0;
int highv_color = 1;
std::string shader = "planet";

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

struct quadOb {
    GLuint vertex_AO = 0;
    GLuint vertex_BO = 0;
    GLuint element_BO = 0;
};

struct fb_texOb {
    GLenum context = GL_TEXTURE0;
    GLenum target = GL_TEXTURE_2D;
    GLuint obj_ptr = 0;
};

fb_texOb screen_quad_texture;

GLuint rb_handle;
GLuint fbo_handle;



quadOb screen_quad_object;

//Start planet
model planet_model{};
model star_model{};

//based on values from http://nssdc.gsfc.nasa.gov/planetary/factsheet/planet_table_ratio.html
ApplicationSolar::orb skyBox   {"skybox", 75.0f, {0.97f, 0.98f, 0.45f},  0.0f, 0.0f,};
ApplicationSolar::orb sun {"sun", 0.7f, {0.99f, 0.95f, 0.43f},  0.0f,    0.0f, };
ApplicationSolar::orb mercury {"mercury", 0.05f, {0.92f, 0.71f, 0.26f}, 365/88.0f, 15.0f, };
ApplicationSolar::orb venus {"venus", 0.2f, {1.00f, 0.94f, 0.75f}, 365/225.0f, 18.0f, };
ApplicationSolar::orb earth {"earth", 0.15f, {0.05f, 0.73f, 0.93f}, 1.0f, 21.0f, true};
ApplicationSolar::orb mars {"mars", 0.1f, {0.77f, 0.21f, 0.12f}, 365/687.0f, 26.0f, };
ApplicationSolar::orb jupiter {"jupiter", 0.35f, {0.85f, 0.43f, 0.20f}, 365/4332.0f, 31.0f, };
ApplicationSolar::orb saturn {"saturn", 0.2f, {0.91f, 0.86f, 0.67f}, 365/10759.0f, 36.0f, };
ApplicationSolar::orb uranus {"uranus", 0.2f, {0.63f, 0.85f, 0.77f}, 365/30688.0f, 40.0f, };
ApplicationSolar::orb neptune {"neptune", 0.2f, {0.07f, 0.40f, 0.67f}, 365/60182.0f, 45.0f, };

std::vector<ApplicationSolar::orb> orbContainer = {skyBox, sun, mercury, earth , venus, mars, jupiter, saturn, uranus, neptune };


std::vector<float> vec_starPos;

std::vector<texture_object> vec_Surfaces;

bool flipVert = false;
bool flipHor = false;
bool setGrey = false;
bool gaussian = false;

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
:Application{resource_path}
,planet_object{}, star_object{}
{
    
    std::string textureDir = resource_path + "textures/";
    // Load texture maps
    
     auto skyBox   = utils::create_texture_object(texture_loader::file(textureDir + "skybox.png"));
    auto solarSurface    = utils::create_texture_object(texture_loader::file(textureDir + "sun.png"));
    auto mercurySurface  = utils::create_texture_object(texture_loader::file(textureDir + "mercury.png"));
    auto venusSurface    = utils::create_texture_object(texture_loader::file(textureDir + "venus.png"));
    auto earthSurface    = utils::create_texture_object(texture_loader::file(textureDir + "earth.png"));
    auto marsSurface     = utils::create_texture_object(texture_loader::file(textureDir + "mars.png"));
    auto jupiterSurface  = utils::create_texture_object(texture_loader::file(textureDir + "jupiter.png"));
    auto saturnSurface   = utils::create_texture_object(texture_loader::file(textureDir + "saturn.png"));
    auto uranusSurface   = utils::create_texture_object(texture_loader::file(textureDir + "uranus.png"));
    auto neptuneSurface   = utils::create_texture_object(texture_loader::file(textureDir + "neptune.png"));
    auto lunarSurface     = utils::create_texture_object(texture_loader::file(textureDir + "lunar.png"));
    
    
   
    vec_Surfaces.push_back(skyBox);
    vec_Surfaces.push_back(solarSurface);
    vec_Surfaces.push_back(mercurySurface);
    vec_Surfaces.push_back(venusSurface);
    vec_Surfaces.push_back(earthSurface);
    vec_Surfaces.push_back(marsSurface);
    vec_Surfaces.push_back(jupiterSurface);
    vec_Surfaces.push_back(saturnSurface);
    vec_Surfaces.push_back(uranusSurface);
    vec_Surfaces.push_back(neptuneSurface);
    vec_Surfaces.push_back(lunarSurface);
    
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
    
    star_model  = {vec_starPos, model::POSITION | model::NORMAL};
    planet_model = model_loader::obj(resource_path + "models/sphere.obj", model::NORMAL | model::TEXCOORD);
    
    initializeGeometry(planet_model, planet_object);
    initializeGeometry(star_model, star_object);
   // initializeGeometry();
    initializeShaderPrograms();
    
    screenQuad();
}
//glm::fmat4 modelTransformationStack; //For later

void ApplicationSolar::upload_planet_transforms(orb &p,  texture_object const& tex_obj) const{
    glUseProgram(m_shaders.at(shader).handle);
    
    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_obj.handle);
    glUniform1i(m_shaders.at(shader).u_locs.at("colTex"), tex_obj.handle);
    
    //rotation period based on time set below
    glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime() * p.rotation), glm::fvec3{0.0f, 1.0f, 0.0f});
    
    //applying necessary scaling and translation
    model_matrix = glm::scale(model_matrix, glm::vec3(p.scaling));
    
    //below the distance difference is changed to declared value from orb struct
    model_matrix = glm::translate(model_matrix, glm::fvec3{p.distance_to_origin, 0.0f, -1.0f});
    
    //Originally from the render activity
    glUniformMatrix4fv(m_shaders.at(shader).u_locs.at("ModelMatrix"),
                       1, GL_FALSE, glm::value_ptr(model_matrix));
    
    // extra matrix for normal transformation to keep them orthogonal to surface
    glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
    glUniformMatrix4fv(m_shaders.at(shader).u_locs.at("NormalMatrix"),
                       1, GL_FALSE, glm::value_ptr(normal_matrix));
    
    //set planet colour
    glUniform3f(m_shaders.at(shader). u_locs.at("colorVec"), p.color.x, p.color.y, p.color.z);
    
    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);
    
    // draw bound vertex array using bound shader
    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
    
    //Changing the movement around a planet and the sun if the model is a moon
    if(p.lunar)
    {
        // bind new shader
        glUseProgram(m_shaders.at(shader).handle);
        
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, vec_Surfaces[9].handle);
        glUniform1i(m_shaders.at(shader).u_locs.at("colTex"), vec_Surfaces[9].handle);
        
        model_matrix = glm::rotate(model_matrix, float(glfwGetTime() * 10.0f), glm::fvec3{0.0f, 1.0f, 0.1f});
        
        model_matrix = glm::translate(model_matrix, glm::fvec3{1.5f, 0.0f, -1.0f});
        
        model_matrix = glm::scale(model_matrix, glm::vec3(0.35f));
        
        //Originally from the render activity
        
        glUniformMatrix4fv(m_shaders.at(shader).u_locs.at("ModelMatrix"),
                           1, GL_FALSE, glm::value_ptr(model_matrix));
        
        // extra matrix for normal transformation to keep them orthogonal to surface
        glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
        glUniformMatrix4fv(m_shaders.at(shader).u_locs.at("NormalMatrix"),
                           1, GL_FALSE, glm::value_ptr(normal_matrix));
        
        glUniform3f(m_shaders.at(shader).u_locs.at("colorVec"), 1.0f, 1.0f, 0.0f);
        
        // bind the VAO to draw
        glBindVertexArray(planet_object.vertex_AO);
        
        // draw bound vertex array using bound shader
        glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
    }
}

void ApplicationSolar::render() const {
    
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_handle);
    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(m_shaders.at("star").handle);
    glBindVertexArray(star_object.vertex_AO);
    glDrawArrays(GL_POINTS, 0, 1000);
    
    // bind shader to upload uniforms
    glUseProgram(m_shaders.at(shader).handle);
    
    glBindVertexArray(planet_object.vertex_AO);
    
    int index_ = 1;
    std::vector<texture_object>::iterator j = vec_Surfaces.begin();
    for (std::vector<orb>::iterator i = orbContainer.begin(); i != orbContainer.end(); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + index_);
        upload_planet_transforms(*i, *j);
        ++j;
        ++index_;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    renderScreenQuad();
    
}

void ApplicationSolar::renderScreenQuad() const{
    glUseProgram(m_shaders.at("quad").handle);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screen_quad_texture.obj_ptr);
    glUniform1i(m_shaders.at("quad").u_locs.at("colTex"), 0);
    
    glBindVertexArray(screen_quad_object.vertex_AO);
    utils::validate_program(m_shaders.at("quad").handle);
    // glDrawElements(GL_TRIANGLES, GLsizei(6), GL_UNSIGNED_INT, NULL);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void ApplicationSolar::updateView() {
    
    rendBuffer(600, 400);
    frameBuffer(600, 400);
    // vertices are transformed in camera space, so camera transform must be inverted
    glm::fmat4 view_matrix = glm::inverse(m_view_transform);
    
    //glUseProgram(m_shaders.at(shader).handle);
    // add light vector to sun
    glm::vec4 solar = {0.0f, 0.0f, 0.0f, 1.0f};
    solar = view_matrix * solar;
    glUniform3f(m_shaders.at(shader).u_locs.at("lightOrigin"), solar.x, solar.y, solar.z);
    
    //new
    glUseProgram(m_shaders.at(shader).handle);
    
    glUniformMatrix4fv(m_shaders.at(shader).u_locs.at("ViewMatrix"),
                       1, GL_FALSE, glm::value_ptr(view_matrix));
    glUseProgram(m_shaders.at("star").handle);
    glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
                       1, GL_FALSE, glm::value_ptr(view_matrix));
    
    //new
    glUseProgram(m_shaders.at(shader).handle);
    
    glUseProgram(m_shaders.at("quad").handle);
    glUniformMatrix4fv(m_shaders.at("quad").u_locs.at("ViewMatrix"), 1, GL_FALSE, glm::value_ptr(view_matrix));
    glUniform2f(m_shaders.at("quad").u_locs.at("res_screen"), GLfloat(600), GLfloat(400));
}

void ApplicationSolar::updateProjection() {
    
    glUseProgram(m_shaders.at(shader).handle);
    
    glUniformMatrix4fv(m_shaders.at(shader).u_locs.at("ProjectionMatrix"),
                       1, GL_FALSE, glm::value_ptr(m_view_projection));
    
    glUseProgram(m_shaders.at("star").handle);
    
    glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"),
                       1, GL_FALSE, glm::value_ptr(m_view_projection));
    
    glUseProgram(m_shaders.at(shader).handle);
    
}

void ApplicationSolar::uploadUniforms() {
    updateUniformLocations();
    glUseProgram(m_shaders.at(shader).handle);
    updateView();
    updateProjection();
    
}

// handle key input
void ApplicationSolar::keyCallback(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        glUseProgram(m_shaders.at("planet").handle);
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.1f});
        updateView();
    }
    else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        glUseProgram(m_shaders.at("planet").handle);
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.1f});
        updateView();
    }
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
        glUseProgram(m_shaders.at("planet").handle);
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{1.0f, 0.0f, 0.0f});
        updateView();
    }
    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
        glUseProgram(m_shaders.at("planet").handle);
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{-1.0f, 0.0f, 0.0f});
        updateView();
    }
    else if (key == GLFW_KEY_UP && action == GLFW_PRESS){
        glUseProgram(m_shaders.at("planet").handle);
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, -1.0f, 0.0f});
        updateView();
    }
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS){
        glUseProgram(m_shaders.at("planet").handle);
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 1.0f, 0.0f});
        updateView();
    }
    else if(key == GLFW_KEY_X && action == GLFW_PRESS){
        glUseProgram(m_shaders.at("planet").handle);
        m_view_transform = glm::rotate(m_view_transform, 0.05f, glm::fvec3{0.0f, -1.0f, 0.0f});
        updateView();
    }
    else if(key == GLFW_KEY_V && action == GLFW_PRESS){
        glUseProgram(m_shaders.at("planet").handle);
        m_view_transform = glm::rotate(m_view_transform, 0.05f, glm::fvec3{0.0f, 1.0f, 0.0f});
        updateView();
    }
    //updateView();
    else if (key == GLFW_KEY_7 && action == GLFW_PRESS)
    { // greyscale active
        setGrey = !setGrey;
        glUseProgram(m_shaders.at("quad").handle);
        glUniform1i(m_shaders.at("quad").u_locs.at("setGrey"), setGrey);
    }
    else if (key == GLFW_KEY_8 && action == GLFW_PRESS)
    { // horizontal flip
        flipHor = !flipHor;
        glUseProgram(m_shaders.at("quad").handle);
        glUniform1i(m_shaders.at("quad").u_locs.at("setHor"), flipHor);
    }
    else if (key == GLFW_KEY_9 && action == GLFW_PRESS)
    { // vertical flip
        flipVert = !flipVert;
        glUseProgram(m_shaders.at("quad").handle);
        glUniform1i(m_shaders.at("quad").u_locs.at("setVert"), flipVert);
    }
    else if (key == GLFW_KEY_0 && action == GLFW_PRESS)
    { // gaussian smooth
        gaussian = !gaussian;
        glUseProgram(m_shaders.at("quad").handle);
        glUniform1i(m_shaders.at("quad").u_locs.at("setGaussian"), gaussian);
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
    
    m_shaders.at("planet").u_locs["colorVec"] = -1;
    m_shaders.at("planet").u_locs["lightOrigin"] = -1;
    m_shaders.at("planet").u_locs["colTex"] = -1;
    
    m_shaders.emplace("star", shader_program{m_resource_path + "shaders/stars.vert",
        m_resource_path + "shaders/stars.frag"});
    // request uniform locations for shader program
    m_shaders.at("star").u_locs["ViewMatrix"] = -1;
    m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;
    
    m_shaders.emplace("quad", shader_program{m_resource_path + "shaders/quad.vert",
        m_resource_path + "shaders/quad.frag"});
    // request uniform locations for shader program
    m_shaders.at("quad").u_locs["ModelMatrix"] = -1;
    m_shaders.at("quad").u_locs["ViewMatrix"] = -1;
    m_shaders.at("quad").u_locs["ProjectionMatrix"] = -1;
    m_shaders.at("quad").u_locs["colTex"] = -1;
    m_shaders.at("quad").u_locs["res_screen"] = -1;
    m_shaders.at("quad").u_locs["setGrey"] = -1;
    m_shaders.at("quad").u_locs["setHor"] = -1;
    m_shaders.at("quad").u_locs["setVert"] = -1;
    m_shaders.at("quad").u_locs["setGaussian"] = -1;
    
}

// load models
void ApplicationSolar::initializeGeometry(model& mdl, model_object& object) {
    //model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);
    
    // generate vertex array object
    glGenVertexArrays(1, &object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(object.vertex_AO);
    
    // generate generic buffer
    glGenBuffers(1, &object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, object.vertex_BO);
    // configure currently bound array buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mdl.data.size(), mdl.data.data(), GL_STATIC_DRAW);
    
    // activate first attribute on gpu
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride
    glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes, mdl.offsets[model::POSITION]);
    // activate second attribute on gpu
    glEnableVertexAttribArray(1);
    // second attribute is 3 floats with no offset & stride
    glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, mdl.vertex_bytes, mdl.offsets[model::NORMAL]);
    // activate third attribute on gpu
    glEnableVertexAttribArray(2);
    // first attribute is 3 floats with no offset & stride
    glVertexAttribPointer(2, model::TEXCOORD.components, model::TEXCOORD.type, GL_FALSE, mdl.vertex_bytes, mdl.offsets[model::TEXCOORD]);
    
    // generate generic buffer
    glGenBuffers(1, &object.element_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.element_BO);
    // configure currently bound array buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * mdl.indices.size(), mdl.indices.data(), GL_STATIC_DRAW);
    
    // store type of primitive to draw
    object.draw_mode = GL_TRIANGLES;
    // transfer number of indices to model object
    object.num_elements = GLsizei(mdl.indices.size());
    
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

void ApplicationSolar::rendBuffer(GLsizei width, GLsizei height){
    glGenRenderbuffers(1, &rb_handle);
    glBindRenderbuffer(GL_RENDERBUFFER, rb_handle);
    glRenderbufferStorage(GL_RENDERBUFFER,
                          GL_DEPTH_COMPONENT24,
                          width,
                          height
                          );
}
void ApplicationSolar::frameBuffer(GLsizei width, GLsizei height){
    glGenTextures(1, &screen_quad_texture.obj_ptr);
    glBindTexture(GL_TEXTURE_2D, screen_quad_texture.obj_ptr);
    
    // set texture sampling parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GLint(GL_LINEAR));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GLint(GL_LINEAR));
    glTexImage2D(GL_TEXTURE_2D, 0, GLint(GL_RGBA8), width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    
    glGenFramebuffers(1, &fbo_handle);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_handle);
    
    glFramebufferTexture(
                         GL_FRAMEBUFFER,
                         GL_COLOR_ATTACHMENT0,        // GL_DEPTH_ATTACHMENT
                         screen_quad_texture.obj_ptr,
                         0
                         );
    
    glFramebufferRenderbuffer(
                              GL_FRAMEBUFFER,
                              GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER_EXT,
                              rb_handle
                              );
    
    GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, draw_buffers);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Error with framebuffer...");
    }
}
void ApplicationSolar::screenQuad(){
    std::vector<GLfloat> vertices {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // v1
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  // v2
        
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // v4
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f   // v3
    };
    
    std::vector<GLuint> indices {
        0, 1, 2, // t1
        0, 2, 3  // t2
    };
    
    auto num_bytes = 5 * sizeof(GLfloat);
    
    // generate vertex array object
    glGenVertexArrays(1, &screen_quad_object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(screen_quad_object.vertex_AO);
    
    // generate generic buffer
    glGenBuffers(1, &screen_quad_object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, screen_quad_object.vertex_BO);
    // configure currently bound array buffer
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(GLsizei(sizeof(float) * vertices.size())), vertices.data(), GL_STATIC_DRAW);
    
    // activate first attribute on gpu
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride
    uintptr_t offset0 = 0 * sizeof(GLfloat);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, GLsizei(num_bytes), (const GLvoid*) offset0);
    
    // activate third attribute on gpu
    glEnableVertexAttribArray(1);
    // second attribute is 2 floats with no offset & stride
    uintptr_t offset1 = 3 * sizeof(GLfloat);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, GLsizei(num_bytes), (const GLvoid*) offset1);
    
    std::cout << "Running screen quad init!" << std::endl;
}

ApplicationSolar::~ApplicationSolar() {
    
    glDeleteBuffers(1, &star_object.vertex_BO);
    glDeleteBuffers(1, &star_object.element_BO);
    glDeleteVertexArrays(1, &star_object.vertex_AO);
    
    glDeleteBuffers(1, &planet_object.vertex_BO);
    glDeleteBuffers(1, &planet_object.element_BO);
    glDeleteVertexArrays(1, &planet_object.vertex_AO);
}

// exe entry point
int main(int argc, char* argv[]) {
    Launcher::run<ApplicationSolar>(argc, argv);
}
