#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"

// gpu representation of model
class ApplicationSolar : public Application {
 public:
  struct orb{
    
    //Declaring the properties
      bool lunar;
      bool stars;
      glm::vec3 color;
    float scaling;
    float rotation;
    float distance_to_origin;

    //assigning necessary value types
    /*orb (float s, float r, float d, bool m = false, bool n = false):
      color{1.0f, 1.0f, 1.0f},
    scaling{s},
    rotation{r},
    distance_to_origin{d},
    lunar{m} {}; */
      
      orb(float s,  glm::vec3 c, float r, float d, bool m = false):
      scaling{s},
      color{c.x, c.y, c.z},
      rotation{r},
      distance_to_origin{d},
      lunar{m} {};
      
  };
  // allocate and initialize objects
  ApplicationSolar(std::string const& resource_path);
  // free allocated objects
  ~ApplicationSolar();

  // update uniform locations and values
  void uploadUniforms();
  // update projection matrix
  void updateProjection();
    void starProjUp();
    void uniformStarUp();
  // react to key input
  void keyCallback(int key, int scancode, int action, int mods);
  //handle delta mouse movement input
  void mouseCallback(double pos_x, double pos_y);

  // draw all objects
  void render() const;

 protected:
  void initializeShaderPrograms();
  void initializeGeometry();
  void updateView();
    void starUpdateView();
    
   void loadTextures();
   void initializeTextures();

  void upload_planet_transforms(orb  &p) const;

  // cpu representation of model
  model_object planet_object, star_object;
};

#endif
